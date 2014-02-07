#!/usr/bin/env python2
# vim: sts=4 ts=4 sw=4 noet:

import sys, os, time
import glob
import Tkinter as Tk
import tkFileDialog, tkMessageBox
import ConfigParser

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

def quit_all():
	global root
	root.destroy()

def make_modal(window):
	global root
	window.transient(root)
	window.grab_set()
	root.wait_window()

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

class ConfigPane(M3Gui):
	class Configuration(object):
		def __init__(self, parent):
			self.parent = parent
			self.top = Tk.Toplevel(parent)
			self.top.title('Select User')

			self.listbox = Tk.Listbox(self.top)
			self.listbox.bind('<<ListboxSelect>>', self.listbox_selection)
			self.listbox.bind('<Button-1>', self.listbox_click)
			self.listbox.bind('<Return>', self.listbox_return)
			self.listbox.pack()

			self.status_label_text = Tk.StringVar()
			self.status_label = Tk.Label(self.top, textvariable=self.status_label_text)

			self.cwd = os.getcwd()

			self.quit_btn = Tk.Button(self.top,
					text="Quit",
					command=quit_all,
					)
			self.quit_btn.pack(side=Tk.LEFT)

			self.change_directory_btn = Tk.Button(self.top,
					text="Change Directory",
					command=self.change_directory,
					)
			self.change_directory_btn.pack(side=Tk.LEFT)

			self.new_user_btn = Tk.Button(self.top,
					text="New User",
					command=self.new_user,
					)
			self.new_user_btn.pack(side=Tk.LEFT)

			self.select_user_btn = Tk.Button(self.top,
					text="Use Selected",
					command=self.use_selected,
					state=Tk.DISABLED,
					)
			self.select_user_btn.pack()

			self.populate_users_list()

			make_modal(self.top)

		def populate_users_list(self):
			users = glob.glob(os.path.join(self.cwd, '*.ini'))
			users.sort()
			self.select_user_btn.configure(state=Tk.DISABLED)
			self.listbox.delete(0, Tk.END)
			if len(users) == 0:
				self.status_label_text.set("No config files found. Create a "\
				"new one or change the search directory.")
			else:
				self.status_label_text.set("Select user or create new user.")
				for u in users:
					self.listbox.insert(Tk.END, os.path.splitext(os.path.basename(u))[0])

		def listbox_selection(self, event):
			if len(self.listbox.curselection()):
				self.select_user_btn.configure(state=Tk.NORMAL)
			else:
				self.select_user_btn.configure(state=Tk.DISABLED)

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
					initialdir=self.cwd,
					mustexist=True,
					parent=self.top,
					#title="Configs directory",
					)
			if new_dir == '':
				return
			elif os.path.exists(new_dir):
				self.cwd = new_dir
				self.populate_users_list()
				return
			else:
				print(new_dir)
				tkMessageBox.showerror("Illegal Directory", "Please select a "\
				"directory that actually exists. (As an aside: How did you "\
				"manage to select one that doesn't exist?)")
				return self.change_directory()

		def new_user(self):
			def destroy_new_user():
				new.destroy()

			def create_new_user():
				uniq = entry.get()
				if len(uniq):
					new.destroy()
					self.uniqname = uniq
					self.config = ConfigParser.SafeConfigParser()
					self.edit_configuration()
				else:
					tkMessageBox.showerror("Blank uniqname",
							"Please enter a uniqname.")

			new = Tk.Toplevel(self.top)
			new.title('Create New User')

			row0 = Tk.Frame(new)
			row0.pack()

			label = Tk.Label(row0, text="uniqname")
			label.pack(side=Tk.LEFT)

			entry = Tk.Entry(row0)
			entry.pack(fill=Tk.X)

			row1 = Tk.Frame(new, bg='red')
			row1.pack(fill=Tk.X)

			create = Tk.Button(row1, text="Create", command=create_new_user)
			create.bind("<Return>", event_lambda(create_new_user))
			create.pack(side=Tk.RIGHT, anchor='e')

			cancel = Tk.Button(row1, text="Cancel", command=destroy_new_user)
			cancel.bind("<Return>", event_lambda(destroy_new_user))
			cancel.pack(side=Tk.RIGHT, anchor='e')

			make_modal(new)

		def use_selected(self):
			self.uniqname = self.listbox.get(self.listbox.curselection()[0])
			self.config_file = os.path.join(self.cwd, self.uniqname)
			self.config = ConfigParser.SafeConfigParser()
			self.config.read(self.config_file)
			try:
				last_update = self.config.get('DEFAULT', 'last-updated')
				cur_time = int(time.time())
				if cur_time - last_update > M3Gui.ONE_DAY:
					tkMessageBox.showinfo("Stale config file",
							"Configuration file has not been"\
							" updated in over 24 hours. Please verify that"\
							" all information is still correct.")
					self.edit_configuration()
			except ConfigParser.NoOptionError:
				tkMessageBox.showwarning("Bad config file",
						"Configuration file corrupt."\
						" Please update your configuration.")
				self.edit_configuration()
			# XXX: here
			#self.top.destroy()

		def edit_configuration(self):
			default_notes = '< Write some notes about what you are doing this session >'

			def edit_save_conf():
				ws = ws_option.get()
				cs = cs_var.get()
				notes = notes_text.get(1.0, Tk.END).strip()
				if ws[0] == '<':
					tkMessageBox.showerror("No Workstation Selected",
							"Please select a workstation")
					return
				if cs[0] == '<':
					tkMessageBox.showerror("No Chips / Stack Selected",
							"Please select the chips / stacks you are currently"\
							"using for testing.")
					return
				if len(notes) < 10 or notes.strip() == default_notes:
					tkMessageBox.showerror("No testing notes added",
							"Please add some notes on what you are currently"\
							"working on. Add some detail -- it may be important"\
							"for you to be able to look this up in the future")
					return
				self.config.set('DEFAULT', 'workstation', ws)
				self.config.set('DEFAULT', 'chips', cs)
				self.config.set('DEFAULT', 'notes', notes)
				self.config.set('DEFAULT', 'last-updated', int(time.time()))
				self.top.destroy()

			def notes_clear_default_text(event):
				text = notes_text.get(1.0, Tk.END).strip()
				if text == default_notes:
					notes_text.delete(1.0, Tk.END)

			edit = Tk.Toplevel(self.top)

			label = Tk.Label(edit, text="Editing configuration for " + self.uniqname)
			label.grid(row=0, columnspan=2)

			ws_label = Tk.Label(edit, text="Workstation")
			ws_label.grid(row=1, column=0)

			ws_var = Tk.StringVar(edit)
			try:
				ws_var.set(self.config.get('DEFAULT', 'workstation'))
			except ConfigParser.NoOptionError:
				ws_var.set('< Select Workstation >')

			# XXX Config
			ws_list = ['Workstation 1', 'Workstation 2', 'Workstation 3']
			ws_option = Tk.OptionMenu(edit, ws_var, *ws_list)
			ws_option.grid(row=1, column=1)

			cs_label = Tk.Label(edit, text="Chips / Stacks")
			cs_label.grid(row=2, column=0)

			cs_var = Tk.StringVar(edit)
			try:
				cs_var.set(self.config.get('DEFAULT', 'chips'))
			except ConfigParser.NoOptionError:
				cs_var.set('< No Chips / Stacks Selected >')

			cs_active_label = Tk.Label(edit, textvariable=cs_var)
			cs_active_label.grid(row=2, column=1)

			cs_btn = Tk.Button(edit, text="Select Chips / Stacks")
			cs_btn.grid(row=3, columnspan=2, sticky='we')

			notes_label = Tk.Label(edit, text="Notes:")
			notes_label.grid(row=4, columnspan=2, sticky='w')

			notes_text = Tk.Text(edit)
			try:
				notes_text.insert(Tk.INSERT, self.config.get('DEFAULT', 'notes'))
			except ConfigParser.NoOptionError:
				notes_text.insert(Tk.INSERT, default_notes)
			notes_text.bind('<FocusIn>', notes_clear_default_text)
			notes_text.grid(row=5, columnspan=2, sticky='we')

			save_conf_btn = Tk.Button(edit, text="Save Configuration",
					command=edit_save_conf)
			save_conf_btn.grid(row=6, columnspan=2, stick='e')

	class CurrentUser(M3Gui):
		def __init__(self, parent):
			self.parent = parent

			self.user_label = Tk.Label(parent, text="User Unknown")
			self.user_label.pack(fill=Tk.X)

	class SoftwareVersion(M3Gui):
		def __init__(self, parent):
			self.parent = parent

			self.current_label = Tk.Label(parent,
					text="Software Version Unknown")
			self.current_label.pack(fill=Tk.X)
			self.start_label = Tk.Label(parent, text="Start Date: <Unknown>")
			self.start_label.pack(fill=Tk.X)
			self.end_label = Tk.Label(parent, text="End Date: <Unknown>")
			self.end_label.pack(fill=Tk.X)
			self.software_version_btn = Tk.Button(parent, text="Choose software version")
			self.software_version_btn.pack(fill=Tk.X)

	def __init__(self, parent):
		self.parent = parent

		self.configuration = self.Configuration(parent)

		self.config_container = Tk.Frame(parent,
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
				fill=Tk.BOTH,
				expand=Tk.YES,
				side=Tk.LEFT,
				)

		self.current_user = self.CurrentUser(self.config_container)
		self.software_version = self.SoftwareVersion(self.config_container)

		self.select_chips = Tk.Button(self.config_container)
		self.select_chips['text'] = 'Select Chips'
		self.select_chips['background'] = 'green'
		self.select_chips.configure(
				width=self.BUTTON_WIDTH,
				padx=self.BUTTON_PADX,
				pady=self.BUTTON_PADY,
				)
		self.select_chips.pack(fill=Tk.X)

		self.button_color = Tk.Button(self.config_container,
				text="Color",
				command = lambda
				arg1='name', arg2='command' :
				self.button_color_command(arg1, arg2)
				)
		self.button_color.pack(fill=Tk.X)
		self.button_color.bind("<Return>", lambda
				event, arg1='name', arg2='event' :
				self.button_color_event(event, arg1, arg2)
				)
		self.button_color.bind("<Button-1>", lambda
				event, arg1='name', arg2='direct event' :
				self.button_color_command(arg1, arg2)
				)
		self.button_color.bind("<Button-2>",
				event_lambda(self.button_color_command, 'name', 'lambda event'))

	def button_color_event(self, event, a1, a2):
		report_event(event)
		return self.button_color_command(a1, a2)

	def button_color_command(self, a1, a2):
		print(a1, a2)
		if self.button_color['background'] == 'green':
			self.button_color['background'] = 'red'
		else:
			self.button_color['background'] = 'green'

class MainPane(M3Gui):
	def __init__(self, parent):
		self.parent = parent

		self.mainpane = Tk.Frame(parent,
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

root = Tk.Tk()
config = ConfigPane(root)
mainpane = MainPane(root)
root.geometry("800x800")
root.mainloop()
