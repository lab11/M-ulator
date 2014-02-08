#!/usr/bin/env python2
# vim: sts=4 ts=4 sw=4 noet:

import sys, os, time
from datetime import datetime
import glob
import ConfigParser
from Queue import Queue
import argparse
import Tkinter as Tk
import ttk
import tkFileDialog, tkMessageBox
from idlelib.WidgetRedirector import WidgetRedirector

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
	parent.wait_window()

def add_returns(widget, callback):
	widget.bind("<Return>", event_lambda(callback))
	widget.bind("<KP_Enter>", event_lambda(callback))

def add_escape(widget, callback):
	widget.bind("<Escape>", event_lambda(callback))

class ButtonWithReturns(ttk.Button):
	# n.b.: ttk.Button is an old-style class
	def __init__(self, *args, **kwargs):
		if 'style' not in kwargs:
			pass
			#kwargs['style'] = 'Dfl.TButton'
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
		self.ws_var.set("foo")
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
			for u in users:
				self.listbox.insert(Tk.END, os.path.splitext(os.path.basename(u))[0])
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
					self.use_selected()
		except AttributeError:
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
			print(new_dir)
			tkMessageBox.showerror("Illegal Directory", "Please select a "\
			"directory that actually exists. (As an aside: How did you "\
			"manage to select one that doesn't exist?)")
			return self.change_directory()

	def new_user(self):
		def create_new_user():
			uniq = entry.get()
			if len(uniq):
				new.destroy()
				self.uniqname_var.set(uniq)
				self.config_file = os.path.join(self.config_dir, uniq + '.ini')
				self.config = ConfigParser.SafeConfigParser()
				self.edit_configuration()
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
		entry.bind("<Return>", event_lambda(create_new_user))
		entry.bind("<KP_Enter>", event_lambda(create_new_user))
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
		self.uniqname_var.set(self.listbox.get(self.listbox.curselection()[0]))
		self.config_file = os.path.join(self.config_dir, self.uniqname_var.get() + '.ini')
		self.parse_config(force_edit=force_edit)

	def parse_config(self, force_edit=False):
		self.config = ConfigParser.SafeConfigParser()
		self.config.read(self.config_file)
		if force_edit:
			self.edit_configuration()
		else:
			try:
				self.last_updated = self.config.getint('DEFAULT', 'last-updated')
				cur_time = int(time.time())
				if cur_time - self.last_updated > M3Gui.ONE_DAY:
					tkMessageBox.showinfo("Stale config file",
							"Configuration file has not been"\
							" updated in over 24 hours. Please verify that"\
							" all information is still correct.")
					self.edit_configuration()
				self.ws_var.set(self.config.get('DEFAULT', 'workstation'))
				print('in conf', self.ws_var)
				print(self.ws_var.get())
				self.cs_var.set(self.config.get('DEFAULT', 'chips'))
				self.notes_var.set(self.config.get('DEFAULT', 'notes'))
			except ConfigParser.NoOptionError:
				tkMessageBox.showwarning("Bad config file",
						"Configuration file corrupt."\
						" Please update your configuration.")
				self.edit_configuration()
		try:
			self.top.destroy()
		except AttributeError:
			pass

	def edit_configuration(self):
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
			self.config.write(open(self.config_file, 'w'))
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

		label = ttk.Label(edit,
				text="Editing configuration for " + self.uniqname_var.get())
		label.grid(row=0, columnspan=2)

		ws_label = ttk.Label(edit, text="Workstation")
		ws_label.grid(row=1, column=0, sticky='e')

		ws_var_new = Tk.StringVar(edit)

		# XXX Config
		ws_list = ['Workstation 1', 'Workstation 2', 'Workstation 3']
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

			select = ButtonWithReturns(selector, text="Use selected",
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
				command=lambda : self.configuration.edit_configuration(),
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


		def pretty_time(unix_time):
			return datetime.fromtimestamp(unix_time).strftime("%Y-%m-%d %H:%M:%S")
		self.lastup_var = Tk.StringVar(self.config_container)
		self.lastup_var.set(pretty_time(self.configuration.last_updated))
		self.lastup_label = ttk.Label(self.config_container, textvariable=self.lastup_var)
		self.lastup_label.pack(side=Tk.BOTTOM, padx='5m', anchor='sw')
		ttk.Label(self.config_container, text="Config Last Updated:").pack(side=Tk.BOTTOM, anchor='sw')

class MainPane(M3Gui):
	def __init__(self, parent):
		self.parent = parent

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

		self.modepane = ttk.Frame(self.mainpane)
		self.modepane.pack(fill=Tk.X)

		ttk.Label(self.mainpane, text="Action Pane").pack(padx='5m', anchor='w')
		self.actionpane = ttk.Frame(self.mainpane)
		self.actionpane.pack(fill=Tk.X)

		ttk.Label(self.mainpane, text="MBus Monitor").pack(padx='5m', anchor='w')
		self.monitorpane = ttk.Frame(self.mainpane)
		self.monitorpane.pack(fill=Tk.X)

		self.mbus_monitor = ReadOnlyText(self.monitorpane)
		self.mbus_monitor.pack(fill=Tk.BOTH, expand=Tk.YES)

if __name__ == '__main__':
	parser = argparse.ArgumentParser()
	parser.add_argument('-c', '--config', help="Configuration file to select."\
			" Bypasses user selection prompt.")
	args = parser.parse_args()

	root = Tk.Tk()

	ttk.Style().configure("TButton",
			backgroound='red',
			fg='blue',
			padding=(0, 5, 0, 5),
			)

	root.title("Root Window")
	root.bind("<Escape>", lambda event : root.destroy())
	root.withdraw()
	config = ConfigPane(root, args)
	mainpane = MainPane(root)
	root.geometry("800x800")
	root.deiconify()
	print("mainloop running...")
	root.mainloop()
