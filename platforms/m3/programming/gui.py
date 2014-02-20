#!/usr/bin/env python2
# vim: sts=4 ts=4 sw=4 noet:

import threading
import sys, os, platform, time, errno
import subprocess
import logging
import inspect
from datetime import datetime
import glob
import ConfigParser
import Queue
import argparse
import Tkinter as Tk
import ttk
import tkFileDialog, tkMessageBox
from idlelib.WidgetRedirector import WidgetRedirector

import m3_logging
logger = m3_logging.get_logger(__name__)
logger.debug('Got gui.py logger')


from ice import ICE
for name,method in inspect.getmembers(ICE, inspect.ismethod):
	setattr(ICE, name, m3_logging.trace(getattr(ICE, name)))

# Import serial after ice as ice prints a nice message about pyserial
import serial

from m3_common import m3_common

if sys.hexversion < 0x02070000:
	logger.error('Python Version 2.7+ is required')
	sys.exit(1)

orig_init = Tk.Widget.__init__
def new_init(self, *args, **kwargs):
	#logger.trace('new_init self {}'.format(repr(self)))
	bound = False
	for arg in args:
		try:
			if 'command' in arg:
				bound = True
				arg['command'] = m3_logging.trace(arg['command'])
		except TypeError:
			pass
	try:
		kwargs['kw']['command'] = m3_logging.trace(kwargs['kw']['command'])
		bound = True
	except KeyError:
		pass
	#if bound:
	#	logger.trace('Bound a method')
	#else:
	#	logger.trace('Did not bind a method')
	#	logger.trace('new_init. args {} ||| kwargs{}'.format(args, kwargs))
	orig_init(self, *args, **kwargs)

	orig_bind = self.bind
	def new_bind(self, fn):
		#logger.trace('new_bind. args{} ||| kwargs{}'.format(args, kwargs))
		#logger.trace('new_bind self {}'.format(repr(self)))
		orig_bind(self, m3_logging.trace(fn))
	self.bind = new_bind
Tk.Widget.__init__ = new_init

def event_lambda(f, *args, **kwargs):
	"""Helper function to wrap lambdas for events in a one-liner interface"""
	return lambda event, f=f, args=args, kwargs=kwargs : f(*args, **kwargs)

def report_event(event):
	event_name = {
			"2": "KeyPress",
			"4": "ButtonPress",
			}
	print("EventTime={}".format(event.time),
			"EventType={}".format(event.type),
			event_name[str(event.type)],
			"EventWidgetId={}".format(event.widget),
			"EventKeySymbol={}".format(event.keysym)
			)

def make_modal(window, parent):
	window.transient(parent)
	window.grab_set()
	parent.wait_window(window)

def add_returns(widget, callback):
	widget.bind("<Return>", event_lambda(callback))
	widget.bind("<KP_Enter>", event_lambda(callback))

def add_escape(widget, callback):
	widget.bind("<Escape>", event_lambda(callback))

def pretty_time(unix_time):
	return datetime.fromtimestamp(unix_time).strftime("%Y-%m-%d %H:%M:%S")

def fname_time(unix_time):
	return datetime.fromtimestamp(unix_time).strftime("%Y-%m-%d-%H-%M-%S")

class ButtonWithReturns(ttk.Button):
	# n.b.: ttk.Button is an old-style class
	def __init__(self, *args, **kwargs):
		#logger.trace('ButtonWithReturns self {}'.format(repr(self)))
		ttk.Button.__init__(self, *args, **kwargs)
		try:
			add_returns(self, kwargs['command'])
		except KeyError:
			pass

# From tkinter.unpythonic.net/wiki/ReadOnlyText
class ReadOnlyText(Tk.Text):
	def __init__(self, *args, **kwargs):
		Tk.Text.__init__(self, *args, **kwargs)
		self.redirector = WidgetRedirector(self)
		self.insert = self.redirector.register("insert", lambda *args, **kw: "break")
		self.delete = self.redirector.register("delete", lambda *args, **kw: "break")

class M3Gui(object):
	BUTTON_WIDTH = 25

	BUTTON_PADX  = "2m"
	BUTTON_PADY  = "1m"

	LABEL_PADX   = "2m"
	LABEL_PADY   = "1m"

	FRAME_PADX   = "3m"
	FRAME_PADY   = "2m"
	FRAME_IPADX  = "3m"
	FRAME_IPADY  = "1m"

	DOUBLE_CLICK_TIMEOUT = 250

	ONE_MINUTE   = 60
	ONE_HOUR     = 60 * 60
	ONE_DAY      = 24 * 60 * 60
	ONE_YEAR     = 365 * 24 * 60 * 60

class Configuration(M3Gui):
	def __init__(self, parent, config_file=None):
		self.parent = parent

		self.uniqname_var = Tk.StringVar()
		self.ws_var = Tk.StringVar()
		self.cs_var = Tk.StringVar()
		self.notes_var = Tk.StringVar()

		if config_file:
			self.config_file = os.path.abspath(config_file)
			self.config_dir = os.path.dirname(self.config_file)
			self.uniqname_var.set(
					os.path.splitext(os.path.basename(self.config_file))[0])
			self.parse_config()
		else:
			self.select_user()

		if hasattr(self, 'quit'):
			return

		self.config.sync = lambda self=self :\
			self.config.write(open(self.config_file, 'w'))

		old_set = self.config.set
		def new_set(cat, key, val):
			old_set(cat, key, val)
			self.config.sync()
		self.config.set = new_set

	def select_user(self):
		def quit():
			self.top.destroy()
			self.quit = True
		self.top = Tk.Toplevel(self.parent)
		self.top.title('Select User')

		self.listbox = Tk.Listbox(self.top)
		self.listbox.bind('<<ListboxSelect>>', self.listbox_selection)
		self.listbox.bind('<Button-1>', self.listbox_click)
		self.listbox.bind('<Return>', self.listbox_return)
		self.listbox.bind('<KP_Enter>', self.listbox_return)
		self.listbox.pack()

		self.status_label_text = Tk.StringVar(self.top)
		self.status_label_text.set("< Uninitialized Text >")
		self.status_label = ttk.Label(self.top, textvariable=self.status_label_text)
		self.status_label.pack()

		self.config_dir = os.path.join(os.getcwd(), 'configs')

		self.quit_btn = ButtonWithReturns(self.top,
				text="Quit",
				command=lambda : quit(),
				)
		self.quit_btn.pack(side=Tk.LEFT, fill=Tk.Y)

		self.change_directory_btn = ButtonWithReturns(self.top,
				text="Change Directory",
				command=self.change_directory,
				)
		self.change_directory_btn.pack(side=Tk.LEFT, fill=Tk.Y)

		self.new_user_btn = ButtonWithReturns(self.top,
				text="New User",
				command=self.new_user,
				)
		self.new_user_btn.pack(side=Tk.LEFT, fill=Tk.Y)

		self.select_and_config_btn = ButtonWithReturns(self.top,
				text="Login and Edit\nMy Configuration",
				command=lambda : self.use_selected(force_edit=True),
				state=Tk.DISABLED,
				)
		self.select_and_config_btn.pack(side=Tk.LEFT, fill=Tk.Y)

		self.select_user_btn = ButtonWithReturns(self.top,
				text="Use Selected",
				command=self.use_selected,
				state=Tk.DISABLED,
				)
		self.select_user_btn.pack(side=Tk.LEFT, fill=Tk.Y)

		self.populate_users_list()

		self.top.bind("<Escape>", lambda event : quit())
		self.top.grab_set()
		if platform.system().lower() == 'darwin':
			os.system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')
		self.parent.wait_window(self.top)

	def populate_users_list(self):
		users = glob.glob(os.path.join(self.config_dir, '*.ini'))
		users.sort()
		self.select_user_btn.configure(state=Tk.DISABLED)
		self.select_and_config_btn.configure(state=Tk.DISABLED)
		self.listbox.delete(0, Tk.END)
		if len(users) == 0:
			self.status_label_text.set("No config files found. Create a "\
			"new one or change the search directory.")
			self.new_user_btn.focus_set()
		else:
			self.status_label_text.set("Select user or create new user.")
			self.users = []
			for u in users:
				u = os.path.splitext(os.path.basename(u))[0]
				self.listbox.insert(Tk.END, u)
				self.users.append(u)
			self.listbox.focus_set()

	def listbox_selection(self, event):
		if len(self.listbox.curselection()):
			self.select_user_btn.configure(state=Tk.NORMAL)
			self.select_and_config_btn.configure(state=Tk.NORMAL)
		else:
			self.select_user_btn.configure(state=Tk.DISABLED)
			self.select_and_config_btn.configure(state=Tk.DISABLED)

	def listbox_click(self, event):
		try:
			if event.time - self.last_listbox_click_time < M3Gui.DOUBLE_CLICK_TIMEOUT:
				if len(self.listbox.curselection()):
					try:
						if event.time - self.last_listbox_click_selection_time < 10 * M3Gui.DOUBLE_CLICK_TIMEOUT:
							self.use_selected()
					except AttributeError:
						pass
					self.last_listbox_click_selection_time = event.time
		except AttributeError:
			pass
		self.last_listbox_click_time = event.time

	def listbox_return(self, event):
		if len(self.listbox.curselection()):
			self.use_selected()

	def change_directory(self):
		new_dir = tkFileDialog.askdirectory(
				initialdir=self.config_dir,
				mustexist=True,
				parent=self.top,
				#title="Configs directory",
				)
		if new_dir == '':
			return
		elif os.path.exists(new_dir):
			self.config_dir = new_dir
			self.populate_users_list()
			return
		else:
			logger.error('Illegar dir: ' + str(new_dir))
			tkMessageBox.showerror("Illegal Directory", "Please select a "\
			"directory that actually exists. (As an aside: How did you "\
			"manage to select one that doesn't exist?)")
			return self.change_directory()

	def new_user(self):
		def create_new_user():
			uniq = entry.get()
			if uniq in self.users:
				tkMessageBox.showerror("Duplicate User",
						"A user with that uniqname already exists.")
			elif len(uniq):
				new.destroy()
				self.uniqname_var.set(uniq)
				self.config_file = os.path.join(self.config_dir, uniq + '.ini')
				self.config = ConfigParser.SafeConfigParser()
				self.edit_configuration(cancellable=True)
			else:
				tkMessageBox.showerror("Blank uniqname",
						"Please enter a uniqname.")

		new = Tk.Toplevel(self.top)
		new.title('Create New User')

		row0 = ttk.Frame(new)
		row0.pack()

		label = ttk.Label(row0, text="uniqname")
		label.pack(side=Tk.LEFT)

		entry = ttk.Entry(row0)
		entry.pack(fill=Tk.X)
		add_returns(entry, create_new_user)
		entry.focus_set()

		row1 = ttk.Frame(new)
		row1.pack(fill=Tk.X)

		create = ButtonWithReturns(row1, text="Create", command=create_new_user)
		create.pack(side=Tk.RIGHT, anchor='e')

		cancel = ButtonWithReturns(row1, text="Cancel",
				command=lambda : new.destroy())
		cancel.pack(side=Tk.RIGHT, anchor='e')

		new.bind("<Escape>", lambda event : new.destroy())
		new.grab_set()
		self.top.wait_window(new)
		#make_modal(new)

	def use_selected(self, force_edit=False):
		logger.debug('use_selected(force_edit={})'.format(force_edit))
		self.uniqname_var.set(self.listbox.get(self.listbox.curselection()[0]))
		self.config_file = os.path.join(self.config_dir, self.uniqname_var.get() + '.ini')
		self.parse_config(force_edit=force_edit)

	def parse_config(self, force_edit=False):
		logger.debug('parse_config(force_eidt={})'.format(force_edit))
		self.config = ConfigParser.SafeConfigParser()
		self.config.read(self.config_file)
		if force_edit:
			self.edit_configuration(cancellable=False)
		else:
			try:
				self.last_updated = self.config.getint('DEFAULT', 'last-updated')
				cur_time = int(time.time())
				if cur_time - self.last_updated > M3Gui.ONE_DAY:
					tkMessageBox.showinfo("Stale config file",
							"Configuration file has not been"\
							" updated in over 24 hours. Please verify that"\
							" all information is still correct.")
					self.edit_configuration(cancellable=False)
				self.ws_var.set(self.config.get('DEFAULT', 'workstation'))
				self.cs_var.set(self.config.get('DEFAULT', 'chips'))
				self.notes_var.set(self.config.get('DEFAULT', 'notes'))
			except ConfigParser.NoOptionError:
				tkMessageBox.showwarning("Bad config file",
						"Configuration file corrupt."\
						" Please update your configuration.")
				self.edit_configuration(cancellable=False)
		try:
			self.top.destroy()
		except AttributeError:
			pass

	def log_configuration(self):
		logger.info("Configuration:")
		logger.info("\tUser: {}".format(self.uniqname_var.get()))
		logger.info("\tWorkstation: {}".format(self.ws_var.get()))
		logger.info("\tChips / Stacks: {}".format(
			self.cs_var.get().replace('\n',' ')))
		logger.info("\tNotes: {}".format(self.notes_var.get()))

	def edit_configuration(self, cancellable):
		default_notes = '< Write some notes about what you are doing this session >'

		def edit_save_conf():
			ws = ws_var_new.get()
			cs = cs_var_new.get()
			notes = notes_text.get(1.0, Tk.END).strip()
			if ws[0] == '<':
				tkMessageBox.showerror("No Workstation Selected",
						"Please select a workstation")
				return
			if cs[0] == '<':
				tkMessageBox.showerror("No Chips / Stack Selected",
						"Please select the chips / stacks you are currently"\
						" using for testing.")
				return
			if len(notes) < 10 or notes.strip() == default_notes:
				tkMessageBox.showerror("No testing notes added",
						"Please add some notes on what you are currently"\
						" working on. Add some detail -- it may be important"\
						" for you to be able to look this up in the future")
				return
			self.ws_var.set(ws)
			self.config.set('DEFAULT', 'workstation', ws)
			self.cs_var.set(cs)
			self.config.set('DEFAULT', 'chips', cs)
			self.notes_var.set(notes)
			self.config.set('DEFAULT', 'notes', notes)
			self.last_updated = int(time.time())
			self.config.set('DEFAULT', 'last-updated', str(self.last_updated))
			# mkdir -p
			try:
				os.mkdir(os.path.dirname(self.config_file))
			except OSError as e:
				if e.errno != errno.EEXIST:
					raise
			self.config.write(open(self.config_file, 'w'))
			self.log_configuration()
			try:
				self.top.destroy()
			except AttributeError:
				edit.destroy()

		def notes_clear_default_text(event):
			text = notes_text.get(1.0, Tk.END).strip()
			if text == default_notes:
				notes_text.delete(1.0, Tk.END)

		try:
			edit = Tk.Toplevel(self.top)
		except AttributeError:
			edit = Tk.Toplevel(self.parent)
		edit.title("Edit Configuration")
		focused = False

		def exit_handler():
			if cancellable:
				edit.destroy()
			else:
				self.top.destroy()
				sys.exit()
		edit.protocol("WM_DELETE_WINDOW", exit_handler)
		edit.bind("<Escape>", lambda event : exit_handler())

		label = ttk.Label(edit,
				text="Editing configuration for " + self.uniqname_var.get())
		label.grid(row=0, columnspan=2)

		ws_label = ttk.Label(edit, text="Workstation")
		ws_label.grid(row=1, column=0, sticky='e')

		ws_var_new = Tk.StringVar(edit)

		ws_file = os.path.join(os.path.dirname(self.config_file), 'workstations.txt')
		ws_list = []
		try:
			for line in open(ws_file):
				line = line.strip()
				if line[0] == '#':
					continue
				ws_list.append(line)
		except IOError:
			tkMessageBox.showerror('Missing Workstation List',
					'Configs directory is missing required file:'\
					' "workstations.txt".')
			self.parent.destroy()
			sys.exit()
		if len(ws_list) == 0:
			tkMessageBox.showerror('Empty Workstation List',
					'There must be at least one entry in workstations.txt')
			self.parent.destroy()
			sys.exit()
		ws_option = Tk.OptionMenu(edit, ws_var_new, *ws_list)
		ws_option.grid(row=1, column=1)

		try:
			ws_var_new.set(self.config.get('DEFAULT', 'workstation'))
		except ConfigParser.NoOptionError:
			ws_var_new.set('< Select Workstation >')
			if not focused:
				ws_option.focus_set()
				focused = True

		def chip_stack_selector(selected_label):
			def use_selected_chips_stacks():
				def s(t):
					r = list(t.selection())
					r.sort()
					return r
				l = '\n'.join(s(chips_tree) + s(stacks_tree))
				if len(l) == 0:
					tkMessageBox.showerror('No Chips Selected', 'You must'\
							' select at least on chip or stack.')
					return
				selected_label.set(l)
				selector.destroy()

			selector = Tk.Toplevel(edit)
			selector.title("Chip / Stack Selector")

			# XXX Config
			chips = (
					('PRCv8', range(5)),
					('CTLv7', range(10)),
					('MDv3', range(2,8)),
					)
			stacks = (
					('STv1', range(5)),
					('STv2', range(2,4)),
					)

			title = ttk.Label(selector, text="Select the chips and / or"\
					" stacks you are currently testing. Hold the Shift or"\
					" Control keys to select multiple.")
			title.grid(row=0, columnspan=2, stick='we')

			chips_tree = ttk.Treeview(selector, height=40)
			for model, numbers in chips:
				chips_tree.insert('', 'end', model, text=model, open=True)
				for n in numbers:
					name = model + '-' + str(n)
					chips_tree.insert(model, 'end', name, text=name)
			chips_tree.grid(row=1, column=0, sticky='ns')

			stacks_tree = ttk.Treeview(selector, height=40)
			for model, numbers in stacks:
				stacks_tree.insert('', 'end', model, text=model, open=True)
				for n in numbers:
					name = model + '-' + str(n)
					stacks_tree.insert(model, 'end', name, text=name)
			stacks_tree.grid(row=1, column=1, sticky='ns')

			chips_clear = ButtonWithReturns(selector, text="Clear Selections",
					command=lambda t=chips_tree : t.selection_set(''))
			chips_clear.grid(row=2, column=0, sticky='we')

			stacks_clear = ButtonWithReturns(selector, text="Clear Selections",
					command=lambda t=stacks_tree : t.selection_set(''))
			stacks_clear.grid(row=2, column=1, sticky='we')

			select = ButtonWithReturns(selector, text="Use Selected",
					command=use_selected_chips_stacks)
			select.grid(row=3, column=1, sticky='e')

			cancel = ButtonWithReturns(selector, text="Cancel",
					command=lambda w=selector : selector.destroy())
			cancel.grid(row=3, column=0, sticky='e')


		cs_label = ttk.Label(edit, text="Chips / Stacks")
		cs_label.grid(row=2, column=0, sticky='ne')

		cs_var_new = Tk.StringVar(edit)

		cs_active_label = ttk.Label(edit, textvariable=cs_var_new)
		cs_active_label.grid(row=2, column=1)

		cs_btn = ButtonWithReturns(edit, text="Select Chips / Stacks",
				command=lambda l=cs_var_new : chip_stack_selector(l))
		cs_btn.grid(row=3, columnspan=2, sticky='we')

		try:
			cs_var_new.set(self.config.get('DEFAULT', 'chips'))
		except ConfigParser.NoOptionError:
			cs_var_new.set('< No Chips / Stacks Selected >')
			if not focused:
				cs_btn.focus_set()
				focused = True

		notes_label = ttk.Label(edit, text="Notes:")
		notes_label.grid(row=4, columnspan=2, sticky='w')

		notes_text = Tk.Text(edit)
		notes_text.bind('<FocusIn>', notes_clear_default_text)
		notes_text.grid(row=5, columnspan=2, sticky='we')

		try:
			notes_text.insert(Tk.INSERT, self.config.get('DEFAULT', 'notes'))
		except ConfigParser.NoOptionError:
			notes_text.insert(Tk.INSERT, default_notes)
			if not focused:
				notes_text.focus_set()
				focused = True

		save_conf_btn = ButtonWithReturns(edit, text="Save Configuration",
				command=edit_save_conf)
		save_conf_btn.grid(row=6, column=1, sticky='e')
		if not focused:
			save_conf_btn.focus_set()

		if cancellable:
			cancel_btn = ButtonWithReturns(edit, text="Cancel",
					command=lambda : edit.destroy())
			cancel_btn.grid(row=6, column=0, sticky='w')

class ConfigPane(M3Gui):
	def __init__(self, parent, args):
		self.parent = parent
		self.args = args

		if hasattr(args, 'config'):
			self.configuration = Configuration(parent, args.config)
		else:
			self.configuration = Configuration(parent)
		if hasattr(self.configuration, 'quit'):
			sys.exit()

		self.config_container = ttk.Frame(parent,
				height=800,
				width=200,
				borderwidth=5,
				relief=Tk.RIDGE,
				)
		self.config_container.pack(
				ipadx=self.FRAME_IPADX,
				ipady=self.FRAME_IPADY,
				padx=self.FRAME_PADX,
				pady=self.FRAME_PADY,
				fill=Tk.Y,
				expand=Tk.NO,
				side=Tk.LEFT,
				)

		ButtonWithReturns(self.config_container, text="Edit Configuration",
				command=lambda :\
				self.configuration.edit_configuration(cancellable=True),
				).pack(fill=Tk.X)

		ttk.Label(self.config_container, text="User:").pack(anchor='w')
		self.user_label = ttk.Label(self.config_container,
			textvariable=self.configuration.uniqname_var)
		self.user_label.pack(padx='5m', anchor='w')

		ttk.Label(self.config_container, text="Workstation:").pack(anchor='w')
		self.ws_label = Tk.Label(self.config_container,
				textvariable=self.configuration.ws_var)
		self.ws_label.pack(padx='5m', anchor='w')

		ttk.Label(self.config_container, text="Chips / Stacks:").pack(anchor='w')
		self.chips_label = ttk.Label(self.config_container,
			textvariable=self.configuration.cs_var)
		self.chips_label.pack(padx='5m', anchor='w')

		ttk.Label(self.config_container, text="Notes:").pack(anchor='w')
		self.notes_label = ttk.Label(self.config_container,
			textvariable=self.configuration.notes_var, wraplength=150)
		self.notes_label.pack(padx='5m', anchor='w')

		self.lastup_var = Tk.StringVar(self.config_container)
		if hasattr(self.configuration, 'last_updated'):
			self.lastup_var.set(pretty_time(self.configuration.last_updated))
		else:
			self.lastup_var.set('Error! Corrupt configuration file')
		self.lastup_label = ttk.Label(self.config_container, textvariable=self.lastup_var)
		self.lastup_label.pack(side=Tk.BOTTOM, padx='5m', anchor='sw')
		ttk.Label(self.config_container, text="Config Last Updated:").pack(side=Tk.BOTTOM, anchor='sw')

class MainPane(M3Gui):
	def __init__(self, parent, args, config):
		self.parent = parent
		self.args = args
		self.config = config

		self.mainpane = ttk.Frame(parent,
				borderwidth=5,
				relief=Tk.RIDGE,
				height=800,
				width=600,
				)
		self.mainpane.pack(
				ipadx=self.FRAME_IPADX,
				ipady=self.FRAME_IPADY,
				padx=self.FRAME_PADX,
				pady=self.FRAME_PADY,
				fill=Tk.BOTH,
				expand=Tk.YES,
				side=Tk.LEFT,
				)

		self.on_ice_connect = []
		self.on_ice_disconnect = []

		self.async_event_queue = Queue.Queue()
		def async_event_handler():
			try:
				while True:
					self.async_event_queue.get_nowait()()
			except Queue.Empty:
				pass
			self.parent.after(50, async_event_handler)
		self.parent.after_idle(async_event_handler)

		# Bar holding ICE status / info / etc
		self.icepane = ttk.LabelFrame(self.mainpane, text="ICE")
		self.icepane.pack(fill='x', expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY,
				ipadx=self.FRAME_IPADX, ipady=self.FRAME_IPADY)

		def serial_port_changed(varName, index, mode):
			logger.debug('serial_port_changed: {}'.format(self.port_selector_var.get()))
			if self.port_selector_var.get() == 'Select serial port':
				return
			if self.port_selector_var.get() == 'Refresh List...':
				self.port_selector.after_idle(lambda :
					populate_serial_port_list(use_config=False))
				return
			if hasattr(serial_port_changed, 'last'):
				if serial_port_changed.last == self.port_selector_var.get() and\
				self.ice.is_connected():
					logger.debug('serial port unchanged; ignore')
					# Selected same item; no-op
					return
			serial_port_changed.last = self.port_selector_var.get()

			def serial_port_changed_helper():
				# Need to execute outside the scope of the StringVar set
				# callback so that the UI tracer is called. Some care in the
				# implementation of this is necessary to avoid recursion
				if hasattr(self, 'ice'):
					logger.debug('Serial Port Changed. Destroying old ICE instance')
					# No need to call this at this point
					del(self.ice.on_disconnect)
					self.ice.destroy()
					for f in self.on_ice_disconnect:
						self.parent.after_idle(f)

				# This disconnect framework is fragile at best (HACK)
				def ice_on_disconnect():
					logger.debug('ice_on_disconnect')
					self.ice.destroy()
					self.async_event_queue.put(lambda :\
							serial_port_changed(None, None, None))

				self.ice = ICE()
				self.ice.on_disconnect = ice_on_disconnect
				try:
					self.ice.connect(self.port_selector_var.get())
					self.ice_status_var.set(\
							'Connected to ICE version {}.{} at {}'.format(
								self.ice.major,
								self.ice.minor,
								self.ice.dev.portstr
								)
							)
					self.config.set('DEFAULT', 'serial_port',
							self.port_selector_var.get())
					for f in self.on_ice_connect:
						self.parent.after_idle(f)
				except serial.SerialException as e:
					logger.error(e)
					self.port_selector_var.set('Select serial port')
					self.ice_status_var.set('Not connected to ICE')

			self.port_selector.after_idle(serial_port_changed_helper)

		def populate_serial_port_list(use_config=True):
			port_list = m3_common.get_serial_candidates()
			port_list.sort()
			try:
				if use_config is False:
					raise ValueError
				last_serial = self.config.get('DEFAULT', 'serial_port')
				if not os.path.exists(last_serial):
					raise ValueError
				if last_serial not in port_list:
					port_list.insert(0, last_serial)
				self.port_selector_var.set(last_serial)
			except (ConfigParser.NoOptionError, ValueError):
				if hasattr(self, 'ice') and self.ice.is_connected():
					self.port_selector_var.set(self.ice.dev.portstr)
				else:
					self.port_selector_var.set('Select serial port')
			port_list.append("Refresh List...")
			#port_list.append("Add Serial Port...")

			# update the actual menu
			menu = self.port_selector['menu']
			menu.delete(0, 'end')
			for p in port_list:
				menu.add_command(label=p,
						command=Tk._setit(self.port_selector_var, p))

		self.port_selector_var = Tk.StringVar()
		self.port_selector_var.trace('w', serial_port_changed)
		self.port_selector = ttk.OptionMenu(self.icepane, self.port_selector_var)
		populate_serial_port_list()
		self.port_selector.pack(side=Tk.LEFT)

		self.ice_status_var = Tk.StringVar()
		self.ice_status_var.set('Not connected to ICE')
		ttk.Label(self.icepane, textvariable=self.ice_status_var
				).pack(fill='y', expand=1, anchor='e')

		# Bar with GOC configuration
		self.gocpane = ttk.LabelFrame(self.mainpane, text="GOC Configuration")
		self.gocpane.pack(fill='x', expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		def apply_goc_freq(new_freq=None):
			logger.debug('apply_goc_freq(new_freq={})'.format(new_freq))
			if new_freq is None:
				freq = self.ice.goc_get_frequency()
			else:
				freq = float(new_freq)
			self.ice.goc_set_frequency(freq)
			self.goc_freq_var.set(str(self.ice.goc_get_frequency()) + ' Hz')
			self.config.set('DEFAULT', 'goc_freq', str(freq))
			if new_freq is None:
				self.goc_freq_entry.delete(0, Tk.END)
				self.goc_freq_btn.focus_set()

		def on_ice_connect_goc_freq():
			logger.debug('on_ice_connect_goc_freq')
			try:
				apply_goc_freq(self.config.getfloat('DEFAULT', 'goc_freq'))
			except ConfigParser.NoOptionError:
				apply_goc_freq(None)

		self.gocframe1 = ttk.Frame(self.gocpane)
		self.gocframe1.pack(fill='x', expand=1)
		ttk.Label(self.gocframe1, text="Slow Frequency: ").pack(side='left')
		self.goc_freq_entry = ttk.Entry(self.gocframe1)
		add_returns(self.goc_freq_entry,
				lambda : apply_goc_freq(self.goc_freq_entry.get()))
		self.goc_freq_entry.pack(side='left')
		ttk.Label(self.gocframe1, text="Hz").pack(side='left')
		self.goc_freq_btn = ButtonWithReturns(self.gocframe1, text="Apply",
				command=lambda : apply_goc_freq(self.goc_freq_entry.get()))
		self.goc_freq_btn.pack(side='left')
		self.goc_freq_var = Tk.StringVar()
		self.goc_freq_var.set('ICE disconnected')
		self.on_ice_connect.append(on_ice_connect_goc_freq)
		self.on_ice_disconnect.append(lambda :\
				self.goc_freq_var.set('ICE disconnected'))
		ttk.Label(self.gocframe1, textvariable=self.goc_freq_var).pack(anchor='e')

		def apply_goc_pol(is_normal):
			if is_normal:
				self.ice.goc_set_onoff(False)
			else:
				self.ice.goc_set_onoff(True)
			self.config.set('DEFAULT', 'goc_pol', 'normal')
			if self.ice.goc_get_onoff() == False:
				self.goc_pol_lbl.set('Normal')
				self.goc_pol_normal.select()
			else:
				self.goc_pol_lbl.set('Inverse')
				self.goc_pol_inverse.select()

		def on_ice_connect_goc_pol(pol=None):
			try:
				pol = self.config.get('DEFAULT', 'goc_pol')
				if pol not in ('normal', 'inverse'):
					logger.error('Bad polarity setting in config file: %s', pol)
					logger.error('Ignoring bad setting and setting polarity to normal')
					pol = 'normal'
				if pol == 'normal':
					apply_goc_pol(True)
				else:
					apply_goc_pol(False)
			except ConfigParser.NoOptionError:
				apply_goc_pol(not self.ice.goc_get_onoff())

		self.gocframe2 = ttk.Frame(self.gocpane)
		self.gocframe2.pack(fill='x', expand=1)
		ttk.Label(self.gocframe2, text="Polarity: ").pack(side='left')
		self.goc_pol_var = Tk.IntVar()
		self.goc_pol_var.set(0)
		self.goc_pol_normal = Tk.Radiobutton(self.gocframe2, text="Normal",
				variable=self.goc_pol_var, value=0, command = lambda :\
						apply_goc_pol(is_normal=True))
		self.goc_pol_normal.pack(side='left')
		self.goc_pol_inverse = Tk.Radiobutton(self.gocframe2, text="Inverse",
				variable=self.goc_pol_var, value=1, command = lambda :\
						apply_goc_pol(is_normal=False))
		self.goc_pol_inverse.pack(side='left')
		self.goc_pol_lbl = Tk.StringVar()
		self.goc_pol_lbl.set('ICE disconnected')
		self.on_ice_connect.append(on_ice_connect_goc_pol)
		self.on_ice_disconnect.append(lambda :\
				self.goc_pol_lbl.set('ICE disconnected'))
		ttk.Label(self.gocframe2, textvariable=self.goc_pol_lbl).pack(anchor='e')

		# Bar holding program image / etc
		self.progpane = ttk.LabelFrame(self.mainpane, text="Program")
		self.progpane.pack(fill=Tk.X, expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		self.progframe = ttk.Frame(self.progpane)
		self.progframe.pack(fill='x', expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		def prog_changed():
			new_file = tkFileDialog.askopenfilename(
					filetypes=(
						('program', '*.bin'),
						('program', '*.hex'),
						('program', '*.txt'),
						),
					parent=self.prog_button,
					title="Select program image",
					)
			if new_file == '':
				return
			return change_file(new_file)

		def change_file(new_file, force_select=False):
			self.prog_flash_ein.configure(state=Tk.DISABLED)
			self.prog_flash_goc.configure(state=Tk.DISABLED)
			if force_select:
				self.prog_button_var.set('Select program image...')
				self.prog_info_var.set('')
				return
			try:
				self.prog = m3_common.read_binfile_static(new_file)
				if self.prog is None:
					return change_file(None, True)
				info = 'Image is {} bytes.\nLast updated at {}'.format(
						len(self.prog) / 2,
						pretty_time(os.path.getmtime(new_file)),
						)
				self.prog_info_var.set(info)
				self.prog_button_var.set(new_file)
				self.config.set('DEFAULT', 'program', new_file)
				self.prog_flash_ein.configure(state=Tk.NORMAL)
				self.prog_flash_goc.configure(state=Tk.NORMAL)
				self.prog_button.after(100, prog_file_watcher)
			except IOError:
				self.prog_button_var.set('Select program image...')
				self.prog_info_var.set('Bad file: ' + new_file)

		def prog_file_watcher():
			p = self.prog_button_var.get()
			if os.path.exists(p):
				change_file(p)
			else:
				change_file(None, True)

		self.prog_button_var = Tk.StringVar()
		self.prog_button = ttk.Button(self.progframe,
				textvariable=self.prog_button_var, command=prog_changed)
		self.prog_button.pack(side=Tk.LEFT)

		self.prog_info_var = Tk.StringVar()
		ttk.Label(self.progframe, textvariable=self.prog_info_var,
				justify=Tk.RIGHT).pack(fill='y', expand=1, anchor='e')

		self.progactionframe = ttk.Frame(self.progpane)
		self.progactionframe.pack(fill='x', expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		ttk.Label(self.progactionframe, text='Run After Programming:'
				).pack(side='left')
		self.prog_run_after_var = Tk.IntVar()
		self.prog_run_after_yes = Tk.Radiobutton(self.progactionframe,
				text="Yes", variable=self.prog_run_after_var, value=1,
				command = lambda :\
						self.config.set('DEFAULT', 'prog_run_after', str(True)))
		self.prog_run_after_yes.pack(side='left')
		self.prog_run_after_no  = Tk.Radiobutton(self.progactionframe,
				text="No",  variable=self.prog_run_after_var, value=0,
				command = lambda :\
						self.config.set('DEFAULT', 'prog_run_after', str(False)))
		self.prog_run_after_no.pack(side='left')

		try:
			runafter = self.config.getboolean('DEFAULT', 'prog_run_after')
		except ConfigParser.NoOptionError:
			runafter = True
		if runafter:
			self.prog_run_after_yes.select()
		else:
			self.prog_run_after_no.select()


		def async_call(root, fns):
			def async_fn_wrapper(fn, e):
				fn()
				e.set()

			def async_fn_done_check(root, e, cb, pb, fns):
				if e.is_set():
					cb.select()
					if pb is not None:
						pb.stop()
						pb.step(pb['maximum']-pb['value']-.1)
					async_call(root, fns)
				else:
					root.after(50, lambda : async_fn_done_check(root, e, cb, pb, fns))

			if len(fns) == 0:
				root.destroy()
				return

			cb, pb, fn = fns.pop(0)
			e = threading.Event()
			t = threading.Thread(target=async_fn_wrapper, args=(fn, e))
			t.daemon = True
			t.start()
			if pb is not None:
				pb.start()

			root.after(50, lambda : async_fn_done_check(root, e, cb, pb, fns))

		def inject_message_via_goc(message):
			goc_win = Tk.Toplevel(self.parent)
			goc_win.title('GOC Status Window')

			fns = []
			slow_freq = float(self.goc_freq_var.get().split()[0])

			c1 = Tk.Checkbutton(goc_win, #state=Tk.DISABLED,
					text="Set GOC frequency to {}".format(slow_freq))
			c1.pack(fill='x', expand=1, anchor='w')
			fns.append((c1, None, lambda f=slow_freq :\
					self.ice.goc_set_frequency(f)))

			c2 = Tk.Checkbutton(goc_win, #state=Tk.DISABLED,
					text="Wake chip via GOC")
			c2.pack(fill='x', expand=1, anchor='w')
			p2 = ttk.Progressbar(goc_win, length=300,
					maximum=((2*8)/slow_freq)/.055)
			p2.pack()
			fns.append((c2, p2, lambda :\
					self.ice.goc_send("7394".decode('hex'), False)))

			c3 = Tk.Checkbutton(goc_win, #state=Tk.DISABLED,
					text="Set GOC frequency to {}".format(8*slow_freq))
			c3.pack(fill='x', expand=1, anchor='w')
			fns.append((c3, None, lambda f=slow_freq*8:\
					self.ice.goc_set_frequency(f)))

			est_time = (len(message)*8)/(8*slow_freq)
			c4 = Tk.Checkbutton(goc_win, #state=Tk.DISABLED,
					text="Send GOC message (~{} seconds)".format(est_time))
			c4.pack(fill='x', expand=1, anchor='w')
			p4 = ttk.Progressbar(goc_win, length=300, maximum=(est_time/.055))
			p4.pack()
			fns.append((c4, p4, lambda m=message:\
					self.ice.goc_send(m, False)))

			c5 = Tk.Checkbutton(goc_win, #state=Tk.DISABLED,
					text="Send extra blink to end transaction")
			c5.pack(fill='x', expand=1, anchor='w')
			p5 = ttk.Progressbar(goc_win, length=300,
					maximum=((2*8)/(8*slow_freq))/.055)
			p5.pack()
			fns.append((c5, p5, lambda :\
					self.ice.goc_send("80".decode('hex'), False)))

			async_call(goc_win, fns)

		def load_program_via_ein():
			self.ice.ein_send(m3_common.build_injection_message(
				self.prog,
				bool(self.prog_run_after_var.get()),
				).decode('hex')
			)
			logger.info('EIN programming complete.')

		def load_program_via_goc():
			prog = m3_common.build_injection_message(
					self.prog,
					bool(self.prog_run_after_var.get()),
					).decode('hex')
			return inject_message_via_goc(prog)

		def load_program_wrapper(fn):
			prog = self.prog_button_var.get()
			base = os.path.splitext(prog)[0]
			if os.path.exists(base + '.c'):
				source = base + '.c'
			elif os.path.exists(base + '.s'):
				source = base + '.s'
			else:
				source = None

			def recompile_program(source, prog, win):
				def compile_program(base_dir, prog):
					cmd = ['make', '-C', base_dir, prog]
					logger.debug("Running: " + ' '.join(cmd))
					try:
						output = subprocess.check_output(cmd, stderr=subprocess.STDOUT)
					except subprocess.CalledProcessError, e:
						logger.warn('Build failure. Command was: ' + ' '.join(cmd))
						logger.warn('Build output:\n' + e.output[:-1])
						raise
					logger.debug(output)

				prog_name = os.path.basename(prog)
				base_dir = os.path.dirname(prog)
				prog_dir = ''
				while True:
					for m in ('makefile', 'Makefile'):
						makefile = os.path.join(base_dir, m)
						if os.path.exists(makefile):
							try:
								prog_path = os.path.join(prog_dir, prog_name)
								compile_program(base_dir, prog_path)
								logger.info(prog_name + ' built successfully.')
								win.destroy()
								return
							except subprocess.CalledProcessError:
								pass
					new_bd, new_pd = os.path.split(base_dir)
					if new_bd == base_dir:
						tkMessageBox.showerror('Build Error',
								'Compilation failed. Check output window for details.')
						break
					else:
						base_dir = new_bd
						prog_dir = os.path.join(new_pd, prog_dir)
				win.build_failed = True
				win.destroy()

			if source is not None:
				logger.debug('Program source file: ' + source)
				if os.path.getmtime(prog) < os.path.getmtime(source):
					win = Tk.Toplevel(self.parent)
					win.title('Program out of date')
					ttk.Label(win, text='Program source is older than program.'\
							' Would you like to re-compile before loading?').pack()
					ButtonWithReturns(win, text="No", command = lambda :\
							win.destroy()).pack(side='right')
					ButtonWithReturns(win, text="Yes", command = lambda :\
							recompile_program(source, prog, win)).pack(side='right')
					make_modal(win, self.parent)
					if hasattr(win, 'build_failed'):
						logger.warn('Programming aborted.')
						return

				if os.path.getmtime(prog) < os.path.getmtime(source):
					logger.warn('Program source is newer than compiled program')
					logger.warn('The following output may be incorrect as a result.')

				try:
					path,name = os.path.split(source)
					cmd = ('git', 'log', '-n', '1', '--pretty=format:%H', name)
					output = subprocess.check_output(
							cmd,
							stderr=subprocess.STDOUT,
							cwd = os.path.dirname(source),
							)
					logger.info(name + ' revision: ' + output)
				except subprocess.CalledProcessError, e:
					logger.warn('Failed to get current revision. Command was: '+\
							' '.join(cmd))
					logger.warn('Command output:\n' + e.output[:-1])
				try:
					path,name = os.path.split(source)
					cmd = ('git', 'diff', name)
					output = subprocess.check_output(
							cmd,
							stderr=subprocess.STDOUT,
							cwd = os.path.dirname(source),
							)
					if len(output):
						logger.info(name + ' has uncommitted changes:\n' + output)
					else:
						logger.debug(name + ' has no uncommitted changes.')
				except subprocess.CalledProcessError, e:
					logger.warn('Failed to get diff. Command was: '+ ' '.join(cmd))
					logger.warn('Command output:\n' + e.output[:-1])
			else:
				logger.warn('Could not find program source file')
			fn()

		self.prog_flash_ein = ButtonWithReturns(self.progactionframe,
				state=Tk.DISABLED, text='Load program via EIN',
				command = lambda : load_program_wrapper(load_program_via_ein))
		self.prog_flash_ein.pack(side='right')
		self.prog_flash_goc = ButtonWithReturns(self.progactionframe,
				state=Tk.DISABLED, text='Load program via GOC',
				command = lambda : load_program_wrapper(load_program_via_goc))
		self.prog_flash_goc.pack(side='right')

		try:
			change_file(self.config.get('DEFAULT', 'program'))
		except ConfigParser.NoOptionError:
			change_file('', force_select=True)

		# Bar with commands
		self.messagepane = ttk.LabelFrame(self.mainpane, text="Custom Messages")
		self.messagepane.pack(fill='x', expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		def validate_command():
			addr = self.message_addr.get().strip()
			data = self.message_data.get().strip()

			self.message_send_mbus.configure(state=Tk.DISABLED)
			self.message_send_goc.configure(state=Tk.DISABLED)

			if len(addr) == 0:
				self.message_contents_var.set('Empty Address')
			elif len(addr) % 2:
				self.message_contents_var.set('Addr: Bad hex value (odd length?)')
			elif len(data) == 0:
				self.message_contents_var.set('Empty Data')
			elif len(data) % 2:
				self.message_contents_var.set('Data: Bad hex value (odd length?)')
			else:
				try:
					addr.decode('hex')
				except TypeError:
					self.message_contents_var.set('Addr: Non-hex character found')
					return
				try:
					data.decode('hex')
				except TypeError:
					self.message_contents_var.set('Data: Non-hex character found')
					return
				msg = ''
				if len(addr) == 2:
					msg += 'Short Addr ' + addr
				else:
					msg += 'Long Addr ' + addr
				msg += ' + {} byte{} of data'.format(
						len(data)/2,
						('s','')[len(data) == 2],
						)
				self.message_contents_var.set(msg)
				self.message_send_mbus.configure(state=Tk.NORMAL)
				self.message_send_goc.configure(state=Tk.NORMAL)

		self.messageframe = ttk.Frame(self.messagepane)
		self.messageframe.pack(fill='x', expand=1)
		ttk.Label(self.messageframe, text='Address').pack(side='left')
		self.message_addr = ttk.Entry(self.messageframe)
		self.message_addr.pack(side='left')
		self.message_addr.bind('<Key>', lambda e :\
				self.message_addr.after_idle(validate_command))
		ttk.Label(self.messageframe, text='Data').pack(side='left')
		self.message_data = ttk.Entry(self.messageframe)
		self.message_data.pack(side='left')
		self.message_data.bind('<Key>', lambda e :\
				self.message_data.after_idle(validate_command))
		ttk.Label(self.messageframe, text='(All values hex)').pack(side='left')

		self.message_contents_var = Tk.StringVar()
		self.message_contents_var.set("Empty Address")
		ttk.Label(self.messageframe, textvariable=self.message_contents_var
				).pack(side='right')

		self.messageactionframe = ttk.Frame(self.messagepane)
		self.messageactionframe.pack(fill='x', expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		def select_preprogrammed_command(cmd):
			if cmd == 'Select Common Message...':
				return
			addr, data = map(str.strip, cmd.split('(')[1][:-1].split(','))
			self.message_addr.delete(0, Tk.END)
			self.message_addr.insert(0, addr[2:])
			self.message_data.delete(0, Tk.END)
			self.message_data.insert(0, data[2:])
			validate_command()

		self.message_defaults_var = Tk.StringVar()
		default_messages = ("Select Common Message...",
				"Enumerate          (0xF0000000, 0x24000000)",
				"SNS Config Bits    (0x40, 0x0423dfef)",
				"SNS Sample Setup   (0x40, 0x030bf0f0)",
				"SNS Sample Start   (0x40, 0x030af0f0)",
				)
		self.message_defaults = ttk.OptionMenu(self.messageactionframe,
				self.message_defaults_var, *default_messages,
				command = select_preprogrammed_command)
		self.message_defaults.pack(side='left')

		self.message_send_mbus = ButtonWithReturns(self.messageactionframe,
				text='Send message via MBus',
				state=Tk.DISABLED,
				command = lambda :\
						self.ice.mbus_send(
							self.message_addr.get().decode('hex'),
							self.message_data.get().decode('hex'),
							)
						)
		self.message_send_mbus.pack(side='right')

		def send_message_via_goc():
			msg = m3_common.build_injection_message(
					self.message_addr.get() + self.message_data.get(),
					False,
					).decode('hex')
			return inject_message_via_goc(msg)

		self.message_send_goc = ButtonWithReturns(self.messageactionframe,
				text='Send message via GOC',
				state=Tk.DISABLED,
				command = send_message_via_goc,
				)
		self.message_send_goc.pack(side='right')

		# Interface for live session
		self.actionpane = ttk.LabelFrame(self.mainpane, text='Action Pane')
		self.actionpane.pack(fill='both', expand=1,
				padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		self.terminal_out_yscrollbar = Tk.Scrollbar(self.actionpane)
		self.terminal_out_yscrollbar.pack(side=Tk.RIGHT, fill=Tk.Y)

		self.terminal_out = ReadOnlyText(self.actionpane, wrap=Tk.NONE)
		self.terminal_out.pack(fill=Tk.BOTH, expand=Tk.YES)

		self.terminal_out.config(yscrollcommand=self.terminal_out_yscrollbar.set)
		self.terminal_out_yscrollbar.config(command=self.terminal_out.yview)

		class DisplayLogger(logging.StreamHandler):
			def __init__(self, window, yscrollbar):
				super(DisplayLogger, self).__init__()
				self.window = window
				self.yscrollbar = yscrollbar

				self.window.tag_config('info', foreground='green')
				self.window.tag_config('warn', foreground='yellow')
				self.window.tag_config( 'err', foreground='red')
				self.window.tag_config( 'dbg', foreground='cyan')

				self.queue = Queue.Queue()
				self.process_events()

			def process_events(self):
				try:
					while True:
						self.queue.get_nowait()()
				except Queue.Empty:
					pass
				self.window.after(100, self.process_events)

			def emit(self, record):
				def emit_idle(record):
					try:
						level,name,msg = self.format(record).split(':', 2)

						if level == 'DEBUG':
							self.window.insert(Tk.END, ' DBG ', 'dbg')
						elif level == 'INFO':
							self.window.insert(Tk.END, 'INFO ', 'info')
						elif level == 'WARNING':
							self.window.insert(Tk.END, 'WARN ', 'warn')
						elif level == 'ERROR':
							self.window.insert(Tk.END, ' ERR ', 'err')
						elif level == 'CRITICAL':
							self.window.insert(Tk.END, 'CRIT ')

						self.window.insert(Tk.END, '{0: >8}: '.format(name[:16]))

						pos = self.yscrollbar.get()[1]
						# For w/e reason, when the scrollbar fills the window
						# it's position != 1.0, instead it's some (mercifully
						# constant) float between 0 and 1. We record the
						# original value and treat it as a position of 1.0 until
						# it's deviated to something else to compensate
						try:
							if pos == self.first_pos:
								pos = 1.0
							elif pos == 1.0:
								# Once the scrollbar is big enough, it will
								# naturally reach 1.0, so we set the 'first_pos'
								# to an impossible value that will never match
								self.first_pos = 1.1
						except AttributeError:
							self.first_pos = pos
						self.window.insert(Tk.END, msg)
						if pos == 1.0:
							# Autoscroll to the bottom automatically iff the
							# scrollbar was already at the bottom
							self.window.yview(Tk.END)
					except (KeyboardInterrupt, SystemExit):
						raise
					except:
						self.handleError(record)
				self.queue.put(lambda record=record : emit_idle(record))

		FORMAT = "%(levelname)s:%(name)s:%(message)s\n"
		self.terminal_logger_formatter = logging.Formatter(fmt=FORMAT)
		self.terminal_logger_handler = DisplayLogger(
				window=self.terminal_out,
				yscrollbar=self.terminal_out_yscrollbar,
				)
		self.terminal_logger_handler.level = m3_logging.log_level_from_environment()
		self.terminal_logger_handler.setFormatter(self.terminal_logger_formatter)
		self.terminal_logger_handler.addFilter(m3_logging.no_trace_filter)
		# TODO: This won't affect any newly created loggers. This should also
		# modify the logging configuration to add our handler as another
		# handler that is installed by default. But I don't know how to do that
		# and it doesn't matter yet, so... tomorrow Pat's problem.
		for l in logging.Logger.manager.loggerDict.values():
			l.addHandler(self.terminal_logger_handler)

		self.terminal_in = ttk.Entry(self.actionpane)
		self.terminal_in.pack(fill=Tk.X)

		# Monitor window for MBus messages
		#self.monitorpane = ttk.LabelFrame(self.mainpane, text="MBus Monitor")
		#self.monitorpane.pack(fill=Tk.X, expand=1,
		#		padx=self.FRAME_PADX, pady=self.FRAME_PADY)

		#self.mbus_monitor = ReadOnlyText(self.monitorpane)
		#self.mbus_monitor.pack(fill=Tk.BOTH, expand=Tk.YES)

def setup_file_logger(config_file, uniq):
	fname = fname_time(time.time())
	path = os.path.join(os.path.dirname(config_file), uniq, fname.split('-')[0], fname.split('-')[1])
	try:
		os.makedirs(path)
	except OSError as e:
		if e.errno != errno.EEXIST and os.path.isdir(path):
			raise
	logfile = os.path.join(path, fname)
	file_formatter = m3_logging.DefaultFormatter("%(levelname)s|%(name)s|%(lineno)s|%(message)s")
	file_handler = logging.FileHandler(logfile, delay=True)
	file_handler.level = logging.DEBUG
	file_handler.formatter = file_formatter
	for l in logging.Logger.manager.loggerDict.values():
		l.addHandler(file_handler)
	setup_file_logger.logfile = logfile

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('-c', '--config', help="Configuration file to select."\
			" Bypasses user selection prompt.")
	args = parser.parse_args()

	root = Tk.Tk()

	style = ttk.Style()
	ttk.Style().theme_use('alt')

	#print(style.layout('TLabel'))
	#print(style.element_options('Label.border'))
	#print(style.element_options('Label.padding'))
	#print(style.element_options('Label.label'))
	#style.configure('TLabel', background='blue', foreground='green', padding=5)
	style.configure('TLabel', padding=2)

	root.title("M3 ICE Interface Controller")
	root.bind("<Escape>", lambda event : root.destroy())
	root.withdraw()
	configpane = ConfigPane(root, args)
	logger.debug('configpane created')
	setup_file_logger(
			configpane.configuration.config_file,
			configpane.configuration.uniqname_var.get(),
			)
	logger.debug('file logger set up')
	# Log configuration to file
	configpane.configuration.log_configuration()
	mainpane = MainPane(root, args, configpane.configuration.config)
	logger.debug('mainpane created')
	root.geometry("1400x900")
	root.deiconify()
	logger.debug('entering mainloop')
	if platform.system().lower() == 'darwin':
		os.system('''/usr/bin/osascript -e 'tell app "Finder" to set frontmost of process "Python" to true' ''')
	logger.info('Saving session to ' + setup_file_logger.logfile)
	root.mainloop()
