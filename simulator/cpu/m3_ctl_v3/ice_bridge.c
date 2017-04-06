/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2013  Pat Pannuto <pat.pannuto@gmail.com>
 *
 * This file is part of Mulator.
 *
 * Mulator is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Mulator is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Mulator.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef __APPLE__
#include <Python/Python.h>
#else
#include <Python.h>
#endif

#include "ice_bridge.h"

#include "cpu/periph.h"

struct ice_instance {
	int term_fd;

	const char* host;
	uint16_t baud;

	PyThreadState* main_pythread_state;
	PyObject* py_ice;

	pthread_t sim_main_thread;
	PyThreadState* sim_main_thread_pystate;

	pthread_t pt;
	pthread_mutex_t pm;
	pthread_cond_t pc;
};

static inline void enter_python(struct ice_instance* ice) {
	if (ice->sim_main_thread == 0) {
		ice->sim_main_thread = pthread_self();
		PyEval_AcquireLock();
		ice->sim_main_thread_pystate = PyThreadState_New(ice->main_pythread_state->interp);
		PyEval_ReleaseLock();
	}
	assert(pthread_equal(ice->sim_main_thread, pthread_self()));
	PyEval_AcquireLock();
	PyThreadState_Swap(ice->sim_main_thread_pystate);
}

static inline void exit_python(void) {
	/* Nominally, the execution thread should eventually free its python
	 * context. That's really on an issue for shutdown though, so currently
	 * we don't bother. This is the relevant code:
		// grab the lock
		PyEval_AcquireLock();
		// swap my thread state out of the interpreter
		PyThreadState_Swap(NULL);
		// clear out any cruft from thread state object
		PyThreadState_Clear(myThreadState);
		// delete my thread state object
		PyThreadState_Delete(myThreadState);
		// release the lock
		PyEval_ReleaseLock();
	*/
	PyThreadState_Swap(NULL);
	PyEval_ReleaseLock();
}

static PyObject* get_py_ice_function(struct ice_instance* ice, const char *f) {
	PyObject *func;
	func = PyObject_GetAttrString(ice->py_ice, f);
	if (func && PyCallable_Check(func)) {
		return func;
	} else {
		if (PyErr_Occurred()) PyErr_Print();
		return NULL;
	}
}

EXPORT void ice_gpio_out(struct ice_instance* ice, uint8_t idx, bool val) {
	static PyObject *py_gpio_set_level = NULL;

	enter_python(ice);

	if (py_gpio_set_level == NULL) {
		py_gpio_set_level = get_py_ice_function(ice, "gpio_set_level");
		if (py_gpio_set_level == NULL) {
			ERR(E_NOT_IMPLEMENTED, "Soft GPIO emulation?\n");
		}
	}

	//      |  gpio_set_level(self, gpio_idx, level)
	PyObject *args;
	args = Py_BuildValue("(bb)", idx, val);
	if (args == NULL) {
		if (PyErr_Occurred()) PyErr_Print();
		ERR(E_UNKNOWN, "Failed to build args tuple\n");
	}

	PyObject *ret;
	ret = PyObject_CallObject(py_gpio_set_level, args);
	Py_DECREF(args);
	if (ret == NULL) {
		if (PyErr_Occurred()) PyErr_Print();
		ERR(E_UNKNOWN, "GPIO set level failed\n");
	}

	exit_python();
}

EXPORT void ice_gpio_dir(struct ice_instance* ice, uint8_t idx, bool val) {
	static PyObject *py_gpio_set_direction = NULL;

	enter_python(ice);

	if (py_gpio_set_direction == NULL) {
		py_gpio_set_direction = get_py_ice_function(ice, "gpio_set_direction");
		if (py_gpio_set_direction == NULL) {
			ERR(E_NOT_IMPLEMENTED, "Soft GPIO emulation?\n");
		}
	}

	//      |  gpio_set_direction(self, gpio_idx, direction)
	PyObject *args;
	args = Py_BuildValue("(bb)", idx, val);
	if (args == NULL) {
		if (PyErr_Occurred()) PyErr_Print();
		ERR(E_UNKNOWN, "Failed to build args tuple\n");
	}

	PyObject *ret;
	ret = PyObject_CallObject(py_gpio_set_direction, args);
	Py_DECREF(args);
	if (ret == NULL) {
		if (PyErr_Occurred()) PyErr_Print();
		ERR(E_UNKNOWN, "GPIO set direction failed\n");
	}

	exit_python();
}

EXPORT void ice_gpio_int(struct ice_instance* ice, uint8_t idx, bool val) {
	ERR(E_NOT_IMPLEMENTED, "ice_gpio_int %p %d %d\n", ice, idx, val);
}

EXPORT unsigned ice_i2c_send(struct ice_instance* ice,
		uint8_t addr, char *data, int len) {
	static PyObject *py_i2c_send = NULL;

	enter_python(ice);

	if (py_i2c_send == NULL) {
		py_i2c_send = get_py_ice_function(ice, "i2c_send");
		if (py_i2c_send == NULL) {
			ERR(E_NOT_IMPLEMENTED, "Soft I2C emulation?\n");
		}
	}

	//     |  i2c_send(self, addr, data)
	PyObject *args;
	args = Py_BuildValue("(bs#)", addr, data, len);
	if (args == NULL) {
		if (PyErr_Occurred()) PyErr_Print();
		ERR(E_UNKNOWN, "Failed to build args tuple\n");
	}

	PyObject *ret;
	ret = PyObject_CallObject(py_i2c_send, args);
	Py_DECREF(args);
	if (ret == NULL) {
		if (PyErr_Occurred()) PyErr_Print();
		ERR(E_UNKNOWN, "Send I2C message failed\n");
	}

	unsigned ret_len;
	if (PyArg_Parse(ret, "I", &ret_len) != true) {
		if (PyErr_Occurred()) PyErr_Print();
		ERR(E_UNKNOWN, "Parsing i2c_send return\n");
	}

	exit_python();

	return ret_len;
}

static void create_ice_bridge(struct ice_instance* ice) {
	Py_InitializeEx(0);

	PyEval_InitThreads();
	ice->main_pythread_state = PyThreadState_Get();

	PyRun_SimpleString("import sys, os");
	PyRun_SimpleString("sys.path.append(os.getcwd().split('M-ulator')[0] + 'M-ulator/platforms/m3/programming/')");

	PyObject *py_ice_module;
	{
		PyObject *py_ice_str;
		py_ice_str = PyString_FromString("ice");
		assert(py_ice_str != NULL);
		py_ice_module = PyImport_Import(py_ice_str);
		Py_DECREF(py_ice_str);

		if (py_ice_module == NULL) {
			if (PyErr_Occurred()) PyErr_Print();
			ERR(E_UNKNOWN, "Could not load 'ice' python module\n");
		}
	}

	{
		PyObject *py_ice_constructor;
		py_ice_constructor = PyObject_GetAttrString(py_ice_module, "ICE");
		if (py_ice_constructor && PyCallable_Check(py_ice_constructor)) {
			PyObject *empty;
			empty = Py_BuildValue("()");
			if (empty == NULL) {
				if (PyErr_Occurred()) PyErr_Print();
				ERR(E_UNKNOWN, "Err building empty object?\n");
			}
			ice->py_ice = PyObject_CallObject(py_ice_constructor, empty);
			Py_DECREF(empty);
			if (ice->py_ice == NULL) {
				if (PyErr_Occurred()) PyErr_Print();
				ERR(E_UNKNOWN, "Failed to create ice object\n");
			}
		} else {
			if (PyErr_Occurred()) PyErr_Print();
			ERR(E_UNKNOWN, "Could not find 'ICE' function\n");
		}
	}

	{
		PyObject *connect;
		connect = PyObject_GetAttrString(ice->py_ice, "connect");
		if (connect && PyCallable_Check(connect)) {
			PyObject *args;
			if (ice->baud == 0) {
				// use default baud rate
				args = Py_BuildValue("(s)", ice->host);
			} else {
				args = Py_BuildValue("(si)", ice->host, ice->baud);
			}
			if (args == NULL) {
				if (PyErr_Occurred()) PyErr_Print();
				ERR(E_UNKNOWN, "Failed to build args tuple\n");
			}

			INFO("Searching for ICE peripheral at `%s' ...\n", ice->host);
			PyObject *ret;
			ret = PyObject_CallObject(connect, args);
			Py_DECREF(args);
			if (ret == NULL) {
				if (PyErr_Occurred()) PyErr_Print();
				WARN("\n");
				WARN("TODO: Option mechanism for setting the path to the serial port where\n");
				WARN("      ICE is located. This is currently hardcoded to /tmp/m3_ice_com1.\n");
				WARN("      Setting up a simlink (e.g. ln -s /dev/ttyUSB0 /tmp/m3_ice_com1) is\n");
				WARN("      an effective workaround for now.\n");
				WARN("\n");
				WARN("This module requires support from an external hardware board (ICE).\n");
				WARN("This external board facilitates all of the memory-mapped peripherals\n");
				WARN("(e.g. GPIOs, bus interfaces). If a hardware board is unavailable or\n");
				WARN("unnecessary, a software ICE emulator is available at\n");
				WARN("    M-ulator/platforms/m3_ctl/programming/fake_ice.py\n");
				WARN("This emulator will provide the necessary support for M3 peripherals\n");
				WARN("and will print useful information on what the core is doing (e.g.\n");
				WARN("when a GPIO is written).\n");
				WARN("\n");
				ERR(E_UNKNOWN, "ICE connect failed\n");
			}
		} else {
			if (PyErr_Occurred()) PyErr_Print();
			ERR(E_UNKNOWN, "ICE object has no 'connect' method?\n");
		}
	}

	INFO("Ice Bridge initialized\n");
}

static void destroy_ice_bridge(void) {
	Py_Finalize();
	INFO("Ice Bridge destroyed\n");
}

static void *ice_thread(void *v_args) {
	struct ice_instance *ice = (struct ice_instance*) v_args;
	create_ice_bridge(ice);
	pthread_cond_signal(&ice->pc);

	Py_BEGIN_ALLOW_THREADS;
	char buf;
	if (read(ice->term_fd, &buf, 1) < 0)
		WARN("Error on ice_thread shutdown monitor: %s\n",
				strerror(errno));
	Py_END_ALLOW_THREADS;

	destroy_ice_bridge();
	pthread_exit(NULL);
}

static pthread_t start_ice(void *v_args) {
	struct ice_instance *ice = (struct ice_instance*) v_args;

	// Need to wait until python is up and running to safely return
	pthread_mutex_lock(&ice->pm);
	pthread_create(&ice->pt, NULL, ice_thread, ice);
	pthread_cond_wait(&ice->pc, &ice->pm);
	pthread_mutex_unlock(&ice->pm);

	return ice->pt;
}

/*** CONSTRUCTOR ***/
struct ice_instance* create_ice_instance(const char *host, int baud) {
	struct ice_instance* ice = malloc(sizeof(struct ice_instance));
	assert((ice != NULL) && "Allocating ice instance storage");

	int pipe_fds[2];
	if (pipe(pipe_fds) < 0)
		ERR(E_UNKNOWN, "Creating ICE termination pipe: %s\n",
				strerror(errno));
	ice->term_fd = pipe_fds[0];

	ice->host = host;
	ice->baud = baud;

	ice->sim_main_thread = 0;

	{
		int ret;
#ifdef DEBUG1
		pthread_mutexattr_t attr;
		if (0 != (ret = pthread_mutexattr_init(&attr)) )
			ERR(E_UNKNOWN, "pthread_mutexattr_init: %s", strerror(ret));
		if (0 != (ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK)) )
			ERR(E_UNKNOWN, "pthread_mutexattr_settype: %s", strerror(ret));
		if (0 != (ret = pthread_mutex_init(&ice->pm, &attr)) )
			ERR(E_UNKNOWN, "pthread_mutex_init: %s", strerror(ret));
		if (0 != (ret = pthread_mutexattr_destroy(&attr)) )
			ERR(E_UNKNOWN, "pthread_mutexattr_destroy: %s", strerror(ret));
#else
		if (0 != (ret = pthread_mutex_init(&ice->pm, NULL)) )
			ERR(E_UNKNOWN, "pthread_mutex_init: %s", strerror(ret));
#endif
		if (0 != (ret = pthread_cond_init(&ice->pc, NULL)) )
			ERR(E_UNKNOWN, "pthread_cond_init: %s", strerror(ret));
	}

	struct periph_time_travel tt = PERIPH_TIME_TRAVEL_NONE;
	register_periph_thread(start_ice, "m3_ctl: ice_bridge", tt, NULL, pipe_fds[1], ice);
	return ice;
}

