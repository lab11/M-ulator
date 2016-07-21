/* Mulator - An extensible {ARM} {e,si}mulator
 * Copyright 2011-2012  Pat Pannuto <pat.pannuto@gmail.com>
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

#include <sys/select.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <ctype.h>

#include MEMMAP_HEADER
#include "gpio.h"

#include "cpu/periph.h"
#include "cpu/core.h"

#include "core/state_async.h"

////////////////////////////////////////////////////////////////////////
// Only include this peripheral if requested in the platform memmap.h //
#ifdef GENERIC_GPIO_BASE

#if GENERIC_GPIO_ALIGNMENT == 4
#define R_fn R_fn32
#define W_fn W_fn32
#elif GENERIC_GPIO_ALIGNMENT == 2
#define R_fn R_fn16
#define W_fn W_fn16
#warn Peripheral architecture does not currently support halfword access
#warn (though it could, without too much effort if necessary)
#error Invalid GENERIC_GPIO_ALIGNMENT
#elif GENERIC_GPIO_ALIGNMENT == 1
#define R_fn R_fn8
#define W_fn W_fn8
#else
#error Invalid GENERIC_GPIO_ALIGNMENT
#endif

#define THREAD_NAME "GPIO (Generic)"

int generic_gpio_termination_fd;
pthread_t generic_gpio_pthread;
#if (defined DEBUG1) && (!defined __APPLE__)
static pthread_mutex_t generic_gpio_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
#else
static pthread_mutex_t generic_gpio_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
static pthread_cond_t  generic_gpio_cond  = PTHREAD_COND_INITIALIZER;

// Temporary: TODO generalize this across simulator
#define DIR_SEP '/'
const char *device_dir_prefix(void) {
	return "/tmp";
}
const char *gpio_device_dir_prefix(void) {
	static char buf[32];
	snprintf(buf, 32, "%s%c%s", device_dir_prefix(), DIR_SEP, GENERIC_GPIO_DIRNAME);
	return buf;
}

static enum gpio_direction {
	GPIO_DIR_TRISTATE,
	GPIO_DIR_INPUT,
	GPIO_DIR_OUTPUT,
	GPIO_DIR_DISABLED,
} gpio_dirs[GENERIC_GPIO_COUNT];

static enum gpio_value {
	GPIO_VAL_FLOATING,
	GPIO_VAL_LOW,
	GPIO_VAL_HIGH,
} gpio_vals[GENERIC_GPIO_COUNT];

static uint8_t gpio_confs[GENERIC_GPIO_COUNT];

static char gpio_val_to_char(const enum gpio_value val) {
	if (val == GPIO_VAL_FLOATING) return 'x';
	if (val == GPIO_VAL_LOW) return '0';
	if (val == GPIO_VAL_HIGH) return '1';
	ERR(E_RUNTIME, "Impossible GPIO val (chr %c int %d)\n", val, val);
}

static enum gpio_value gpio_char_to_val(const char c) {
	if (c == 'x') return GPIO_VAL_FLOATING;
	if (c == '0') return GPIO_VAL_LOW;
	if (c == '1') return GPIO_VAL_HIGH;
	ERR(E_RUNTIME, "Illegal GPIO val (chr %c int %d)\n", c, c);
}

static int  gpio_in_fds        [GENERIC_GPIO_COUNT];
static int  gpio_out_file_fds  [GENERIC_GPIO_COUNT];
//static int  gpio_out_socket_fds[GENERIC_GPIO_COUNT];

////

static void _update_gpio_val(int gpio, enum gpio_value val) {
	gpio_vals[gpio] = val;

	if (ftruncate(gpio_out_file_fds[gpio], 0) < 0)
		ERR(E_UNKNOWN, "Truncating gpio file: %s\n", strerror(errno));

	char val_as_char = gpio_val_to_char(val);

	if (write(gpio_out_file_fds[gpio], &val_as_char, 1) != 1)
		ERR(E_UNKNOWN, "Writing new gpio value: %s\n", strerror(errno));

	// TODO: Socket writing
}

static void update_gpio_val(int gpio, enum gpio_value val) {
	pthread_mutex_lock(&generic_gpio_mutex);
	_update_gpio_val(gpio, val);
	pthread_mutex_unlock(&generic_gpio_mutex);
}

static void create_gpio_in_file(int gpio) {
	const char *path = gpio_device_dir_prefix();
	char *gpio_path;

	if (asprintf(&gpio_path, "%s%cgpio%02d.in", path, DIR_SEP, gpio) < 0)
		ERR(E_UNKNOWN, "Generating path for generic gpio in\n");

	if (mkfifo(gpio_path, 0622) < 0)
		ERR(E_UNKNOWN, "In gpio fifo: %s\n", strerror(errno));

	// Open RDWR so that read doesn't report ready to read EOF when
	// there are no readers.
	gpio_in_fds[gpio] = open(gpio_path, O_RDWR);
	if (gpio_in_fds[gpio] < 0)
		ERR(E_UNKNOWN, "In gpio open: %s\n", strerror(errno));

	if (fchmod(gpio_in_fds[gpio], 0222) < 0)
		ERR(E_UNKNOWN, "In gpio reduce permissions: %s\n", strerror(errno));

	free(gpio_path);
}

static void remove_gpio_in_file(int gpio) {
	const char *path = gpio_device_dir_prefix();
	char *gpio_path;

	if (asprintf(&gpio_path, "%s%cgpio%02d.in", path, DIR_SEP, gpio) < 0)
		ERR(E_UNKNOWN, "Generating path for generic gpio in\n");

	if (unlink(gpio_path) < 0)
		WARN("Removing generic gpio input file: %s\n", strerror(errno));

	free(gpio_path);
}

static void create_gpio_out_files(int gpio) {
	const char *path = gpio_device_dir_prefix();
	char *gpio_path;

	//// Regular file output

	if (asprintf(&gpio_path, "%s%cgpio%02d.out.file", path, DIR_SEP, gpio) < 0)
		ERR(E_UNKNOWN, "Generating path for generic gpio out file\n");

	gpio_out_file_fds[gpio] = open(gpio_path, O_WRONLY|O_CREAT|O_EXCL|O_APPEND|O_SYNC, 0444);
	if (gpio_out_file_fds[gpio] < 0)
		ERR(E_UNKNOWN, "Opening gpio out file: %s\n", strerror(errno));

	free(gpio_path);

	//// Socket file output

	/*
	if (asprintf(&gpio_path, "%s%cgpio%02d.out.socket", path, DIR_SEP, gpio) < 0)
		ERR(E_UNKNOWN, "Generating path for generic gpio out file\n");

	struct sockaddr_un sa;
	memset(&sa, 0, sizeof(sa));
	sa.sun_family = AF_UNIX;
	strcpy(sa.sun_path, gpio_path);

	gpio_out_socket_fds[gpio] = socket(AF_UNIX, SOCK_DGRAM, 0);
	if (gpio_out_socket_fds[gpio] < 0)
		ERR(E_UNKNOWN, "Creating gpio out socket: %s\n", strerror(errno));

	if (bind(gpio_out_socket_fds[gpio], (struct sockaddr*) &sa, sizeof(sa)) < 0)
		ERR(E_UNKNOWN, "Bind to gpio out socket: %s\n", strerror(errno));

	free(gpio_path);
	*/

	_update_gpio_val(gpio, gpio_vals[gpio]);
}

static void remove_gpio_out_files(int gpio) {
	const char *path = gpio_device_dir_prefix();
	char *gpio_path;

	//// Regular file output

	if (asprintf(&gpio_path, "%s%cgpio%02d.out.file", path, DIR_SEP, gpio) < 0)
		ERR(E_UNKNOWN, "Generating path for generic gpio out file\n");
	if (unlink(gpio_path) < 0)
		WARN("Removing generic gpio output files: %s\n", strerror(errno));

	//// Socket file output

	/*
	if (asprintf(&gpio_path, "%s%cgpio%02d.out.socket", path, DIR_SEP, gpio) < 0)
		ERR(E_UNKNOWN, "Generating path for generic gpio out file\n");
	if (unlink(gpio_path) < 0)
		WARN("Removing generic gpio output files: %s\n", strerror(errno));
	*/

	free(gpio_path);
}

static void _update_gpio_dir(int gpio, enum gpio_direction dir) {
	bool have_i = false;
	bool have_o = false;
	bool need_i = false;
	bool need_o = false;

	if (gpio_dirs[gpio] == GPIO_DIR_TRISTATE) {
		have_i = true;
		have_o = true;
	} else if (gpio_dirs[gpio] == GPIO_DIR_INPUT) {
		have_i = true;
	} else if (gpio_dirs[gpio] == GPIO_DIR_OUTPUT) {
		have_o = true;
	}

	if (dir == GPIO_DIR_TRISTATE) {
		need_i = true;
		need_o = true;
	} else if (gpio_dirs[gpio] == GPIO_DIR_INPUT) {
		need_i = true;
	} else if (gpio_dirs[gpio] == GPIO_DIR_OUTPUT) {
		need_o = true;
	}

	if (need_i && !have_i)
		create_gpio_in_file(gpio);
	if (!need_i && have_i)
		remove_gpio_in_file(gpio);
	if (need_o && !have_o)
		create_gpio_out_files(gpio);
	if (!need_o && have_o)
		remove_gpio_out_files(gpio);

	gpio_dirs[gpio] = dir;
}

static void update_gpio_dir(int gpio, enum gpio_direction dir) {
	pthread_mutex_lock(&generic_gpio_mutex);
	_update_gpio_dir(gpio, dir);
	pthread_mutex_unlock(&generic_gpio_mutex);
}

static bool gpio_read(uint32_t addr, gpio_align_t *val,
		bool debugger __attribute__ ((unused)) ) {
	bool ret;
	int idx;

	idx = (addr - GENERIC_GPIO_BASE) / GENERIC_GPIO_ALIGNMENT;
	assert(idx < GENERIC_GPIO_COUNT); // Should be limited by memmap

	pthread_mutex_lock(&generic_gpio_mutex);
	ret = gpio_vals[idx];
	pthread_mutex_unlock(&generic_gpio_mutex);

	*val = ret;
	return true;
}

static void gpio_write(uint32_t addr, gpio_align_t val,
		bool debugger __attribute__ ((unused)) ) {
	int idx;

	idx = (addr - GENERIC_GPIO_BASE) / GENERIC_GPIO_ALIGNMENT;
	assert(idx < GENERIC_GPIO_COUNT); // Should be limited by memmap

	update_gpio_val(idx, val);
}

static void gpio_written_async(int gpio, int fd) {
	bool should_interrupt = false;
	enum gpio_value old_val, new_val;
	char val;

	if (read(fd, &val, 1) < 0)
		ERR(E_UNKNOWN, "Reading async gpio write: %s\n", strerror(errno));

	// Ignore whitespace (e.g. echo 1 will write a \n w/out -n)
	if (isspace(val))
		return;

	pthread_mutex_lock(&generic_gpio_mutex);
	old_val = gpio_vals[gpio];
	_update_gpio_val(gpio, gpio_char_to_val(val));
	new_val = gpio_vals[gpio];

	if ((new_val == GPIO_VAL_LOW) &&
			(gpio_confs[gpio] & GENERIC_GPIO_CONF_INT_LVL0_EN_MASK))
		should_interrupt = true;
	if ((new_val == GPIO_VAL_HIGH) &&
			(gpio_confs[gpio] & GENERIC_GPIO_CONF_INT_LVL1_EN_MASK))
		should_interrupt = true;
	if ((new_val == GPIO_VAL_LOW) && (old_val != GPIO_VAL_LOW) &&
			(gpio_confs[gpio] & GENERIC_GPIO_CONF_INT_EDG0_EN_MASK))
		should_interrupt = true;
	if ((new_val == GPIO_VAL_HIGH) && (old_val != GPIO_VAL_HIGH) &&
			(gpio_confs[gpio] & GENERIC_GPIO_CONF_INT_EDG1_EN_MASK))
		should_interrupt = true;
	pthread_mutex_unlock(&generic_gpio_mutex);

	if (should_interrupt) {
#ifdef GENERIC_GPIO_COALESCE_INTERRUPTS
		state_assert_interrupt_async(GENERIC_GPIO_INTERRUPT_BASE);
#else
		state_assert_interrupt_async(GENERIC_GPIO_INTERRUPT_BASE + gpio);
#endif
	}
}

static bool gpio_conf_read(uint32_t addr, gpio_align_t *val,
		bool debugger __attribute__ ((unused)) ) {
	int idx;

	idx = (addr - GENERIC_GPIO_CONF_BASE) / GENERIC_GPIO_ALIGNMENT;
	assert(idx < GENERIC_GPIO_COUNT); // Should be limited by memmap

	pthread_mutex_lock(&generic_gpio_mutex);
	*val = gpio_confs[idx];
	pthread_mutex_unlock(&generic_gpio_mutex);

	return true;
}

static void gpio_conf_write(uint32_t addr, gpio_align_t new_conf,
		bool debugger __attribute__ ((unused)) ) {
	int idx;

	idx = (addr - GENERIC_GPIO_CONF_BASE) / GENERIC_GPIO_ALIGNMENT;
	assert(idx < GENERIC_GPIO_COUNT); // Should be limited by memmap

	pthread_mutex_lock(&generic_gpio_mutex);
	uint8_t orig_conf = gpio_confs[idx];

	if ((orig_conf & GENERIC_GPIO_CONF_OUTPUT_EN_MASK) != (new_conf & GENERIC_GPIO_CONF_OUTPUT_EN_MASK))
		_update_gpio_dir(idx, (new_conf & GENERIC_GPIO_CONF_OUTPUT_EN_MASK) ? GPIO_DIR_OUTPUT : GPIO_DIR_INPUT);
	// GENERIC_GPIO_CONF_INT*_EN_MASK only checked on interrupt trigger
	if ((orig_conf & GENERIC_GPIO_CONF_PULLUP_EN_MASK) != (new_conf & GENERIC_GPIO_CONF_PULLUP_EN_MASK))
		ERR(E_NOT_IMPLEMENTED, "GPIO Pullup\n");
	if ((orig_conf & GENERIC_GPIO_CONF_PULLDOWN_EN_MASK) != (new_conf & GENERIC_GPIO_CONF_PULLDOWN_EN_MASK))
		ERR(E_NOT_IMPLEMENTED, "GPIO Pulldown\n");

	gpio_confs[idx] = new_conf;
	DBG1("gpio %d configuration updated to 0x%02x\n", idx, new_conf);
	pthread_mutex_unlock(&generic_gpio_mutex);
}

static void* generic_gpio_thread(void *unused __attribute__ ((unused))) {
	const char *path = gpio_device_dir_prefix();

	if (mkdir(path, 0777) < 0)
		ERR(E_UNKNOWN, "Creating generic gpio device node dir: %s\n", strerror(errno));

	int i;
	for (i = 0; i < GENERIC_GPIO_COUNT; i++) {
		gpio_dirs[i] = GPIO_DIR_DISABLED;
		gpio_vals[i] = GPIO_VAL_FLOATING;
		update_gpio_dir(i, GPIO_DIR_TRISTATE);
	}

	// Setup done.
	INFO("Initialized %d gpios in %s\n", GENERIC_GPIO_COUNT, path);
	pthread_cond_signal(&generic_gpio_cond);

	fd_set fd_r;
	fd_set fd_w;
	int max_fd = 0;

	FD_ZERO(&fd_r);
	FD_ZERO(&fd_w);
	for (i = 0; i < GENERIC_GPIO_COUNT; i++) {
		FD_SET(gpio_in_fds[i], &fd_r);
		max_fd = MAX(max_fd, gpio_in_fds[i]);
		//FD_SET(gpio_out_socket_fds[i], &fd_w);
		//max_fd = MAX(max_fd, gpio_out_socket_fds[i]);
	}
	FD_SET(generic_gpio_termination_fd, &fd_r);
	max_fd = MAX(max_fd, generic_gpio_termination_fd);

	while (true) {
		fd_set fd_rs = fd_r;
		fd_set fd_ws = fd_w;

		if (select(max_fd, &fd_rs, &fd_ws, NULL, NULL) < 0)
			ERR(E_UNKNOWN, "Watching gpio fds: %s\n", strerror(errno));

		if (unlikely(FD_ISSET(generic_gpio_termination_fd, &fd_rs)))
			break;

		for (i = 0; i < GENERIC_GPIO_COUNT; i++) {
			if (FD_ISSET(gpio_in_fds[i], &fd_rs))
				gpio_written_async(i, gpio_in_fds[i]);
		}
	}

	// Shutdown
	for (i = 0; i < GENERIC_GPIO_COUNT; i++) {
		update_gpio_dir(i, GPIO_DIR_DISABLED);
	}
	if (rmdir(path) < 0)
		ERR(E_UNKNOWN, "Unlinking generic gpio device node dir: %s\n", strerror(errno));

	INFO("Generic gpio interface shut down successfully.\n");
	return NULL;
}

////

static pthread_t start_generic_gpio(void *unused __attribute__ ((unused))) {
	pthread_mutex_lock(&generic_gpio_mutex);
	pthread_create(&generic_gpio_pthread, NULL, generic_gpio_thread, NULL);
	pthread_cond_wait(&generic_gpio_cond, &generic_gpio_mutex);
	pthread_mutex_unlock(&generic_gpio_mutex);
	return generic_gpio_pthread;
}

__attribute__ ((constructor))
static void register_memmap_gpio(void) {
	union memmap_fn mem_fn;

	mem_fn. R_fn = gpio_read;
	register_memmap("GPIO", false, GENERIC_GPIO_ALIGNMENT,
			mem_fn, GENERIC_GPIO_BASE, GENERIC_GPIO_TOP);
	mem_fn. W_fn = gpio_write;
	register_memmap("GPIO", true, GENERIC_GPIO_ALIGNMENT,
			mem_fn, GENERIC_GPIO_BASE, GENERIC_GPIO_TOP);

	mem_fn. R_fn = gpio_conf_read;
	register_memmap("GPIO Conf", false, GENERIC_GPIO_ALIGNMENT,
			mem_fn, GENERIC_GPIO_CONF_BASE, GENERIC_GPIO_CONF_TOP);

	mem_fn. W_fn = gpio_conf_write;
	register_memmap("GPIO Conf", true, GENERIC_GPIO_ALIGNMENT,
			mem_fn, GENERIC_GPIO_CONF_BASE, GENERIC_GPIO_CONF_TOP);

	int pipe_fds[2];
	if (pipe(pipe_fds) < 0)
		ERR(E_UNKNOWN, "Creating generic gpio termination pipe: %s\n",
				strerror(errno));
	generic_gpio_termination_fd = pipe_fds[0];

	struct periph_time_travel tt = PERIPH_TIME_TRAVEL_NONE;
	register_periph_thread(start_generic_gpio, THREAD_NAME,
			tt, NULL, pipe_fds[1], NULL);
}

#endif
// Only include this peripheral if requested in the platform memmap.h //
////////////////////////////////////////////////////////////////////////

