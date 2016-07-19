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

#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "uart.h"
#include "memmap.h"

#include "cpu/periph.h"
#include "cpu/core.h"

#include "core/state_async.h"

#define INVALID_CLIENT (UINT_MAX-1)

#define THREAD_NAME "poll_uart_thread"

pthread_t poll_uart_pthread;
volatile bool poll_uart_enabled = false;


static void *poll_uart_thread(void *);
#if (defined DEBUG1) && (!defined __APPLE__)
static pthread_mutex_t poll_uart_mutex = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
#else
static pthread_mutex_t poll_uart_mutex = PTHREAD_MUTEX_INITIALIZER;
#endif
static pthread_cond_t  poll_uart_cond  = PTHREAD_COND_INITIALIZER;
static uint32_t poll_uart_client = INVALID_CLIENT;

static pthread_rwlock_t poll_uart_rwlock = PTHREAD_RWLOCK_INITIALIZER;
static uint32_t UART_SR(uint32_t *addr) {
	uint32_t ret;
	pthread_rwlock_rdlock(&poll_uart_rwlock);
	ret = SR_A(addr);
	pthread_rwlock_unlock(&poll_uart_rwlock);
	return ret;
}
static void UART_SW(uint32_t *addr, uint32_t val) {
	pthread_rwlock_wrlock(&poll_uart_rwlock);
	SW_A(addr, val);
	pthread_rwlock_unlock(&poll_uart_rwlock);
}

// circular buffer
// head is location to read valid data from
// tail is location to write data to
// characters are lost if not read fast enough
// head == tail --> buffer is full (not that it matters)
// head == NULL --> buffer if empty

// these should be treated as char's, but are stored as uint32_t
// to unify state tracking code
static uint32_t poll_uart_buffer[POLL_UART_BUFSIZE];
static uint32_t *poll_uart_head = NULL;
static uint32_t *poll_uart_tail = poll_uart_buffer;

static const struct timespec poll_uart_baud_sleep =\
		{0, (NSECS_PER_SEC/POLL_UART_BAUD)*8};	// *8 bytes vs bits


////////////////////////////////////////////////////////////////////////////////
// UART THREAD(S)
////////////////////////////////////////////////////////////////////////////////

static void *poll_uart_thread(void *unused __attribute__ ((unused))) {
	uint16_t port = POLL_UART_PORT;

	int sock;
	struct sockaddr_in server;

#ifdef __APPLE__
	if (0 != pthread_setname_np(THREAD_NAME))
#else
	if (0 != prctl(PR_SET_NAME, THREAD_NAME, 0, 0, 0))
#endif
		ERR(E_UNKNOWN, "Setting thread name: %s\n", strerror(errno));

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (-1 == sock) {
		ERR(E_UNKNOWN, "Creating UART device: %s\n", strerror(errno));
	}

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);
	int on = 1;
	setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	if (-1 == bind(sock, (struct sockaddr *) &server, sizeof(server))) {
		ERR(E_UNKNOWN, "Creating UART device: %s\n", strerror(errno));
	}

	if (-1 == listen(sock, 1)) {
		ERR(E_UNKNOWN, "Creating UART device: %s\n", strerror(errno));
	}

	INFO("UART listening on port %d (use `nc -4 localhost %d` to communicate)\n",
			port, port);

	pthread_cond_signal(&poll_uart_cond);

	while (1) {
		int client;
		while (1) {
			fd_set set;
			struct timeval timeout;

			FD_ZERO(&set);
			FD_SET(sock, &set);

			timeout.tv_sec = 0;
			timeout.tv_usec = 100000;

			if (select(FD_SETSIZE, &set, NULL, NULL, &timeout)) {
				client = accept(sock, NULL, 0);
				break;
			} else {
				if (!poll_uart_enabled) {
					INFO("Polling UART device shut down\n");
					pthread_exit(NULL);
				}
			}
		}

		if (-1 == client) {
			ERR(E_UNKNOWN, "UART device failure: %s\n", strerror(errno));
		} else {
			struct sockaddr_in sa;
			socklen_t l = sizeof(sa);

			if (getsockname(client, (struct sockaddr*) &sa, &l)) {
				ERR(E_UNKNOWN, "%d UART: %s\n", __LINE__, strerror(errno));
			}
			assert(l == sizeof(sa));

			char buf[INET_ADDRSTRLEN];
			if (NULL == inet_ntop(AF_INET, &sa.sin_addr, buf, INET_ADDRSTRLEN)) {
				ERR(E_UNKNOWN, "%d UART: %s\n", __LINE__, strerror(errno));
			}

			INFO("UART connected. Client at %s\n", buf);
		}

		static const char *welcome = "\
>>MSG<< You are now connected to the UART polling device\n\
>>MSG<< Lines prefixed with '>>MSG<<' are sent from this\n\
>>MSG<< UART <--> network bridge, not the connected device\n\
>>MSG<< This device has a "VAL2STR(POLL_UART_BUFSIZE)" byte buffer\n\
>>MSG<< This device operates at "VAL2STR(POLL_UART_BAUD)" baud\n\
>>MSG<< To send a message, simply type and press the return key\n\
>>MSG<< All characters, up to and including the \\n will be sent\n";

		if (-1 == send(client, welcome, strlen(welcome), 0)) {
			ERR(E_UNKNOWN, "%d UART: %s\n", __LINE__, strerror(errno));
		}

		UART_SW(&poll_uart_client, client);

		static uint8_t c;
		static ssize_t ret;
		while (1) {
			// n.b. If the baud rate is set to a speed s.t. polling
			// becomes CPU intensive (not likely..), this could be
			// replaced with select + self-pipe
			nanosleep(&poll_uart_baud_sleep, NULL);

			if (!poll_uart_enabled) {
				UART_SW(&poll_uart_client, INVALID_CLIENT);
				static const char *goodbye = "\
\n\
>>MSG<< An extra newline has been printed before this line\n\
>>MSG<< The polling UART device has shut down. Good bye.\n";
				send(client, goodbye, strlen(goodbye), 0);
				close(client);
				INFO("Polling UART disconnected from client\n");
				INFO("Polling UART device shut down\n");
				pthread_exit(NULL);
			}

			ret = recv(client, &c, 1, MSG_DONTWAIT);

			if (ret != 1) {
				// Common case: poll
				if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
					continue;
				}

				break;
			}

			pthread_rwlock_wrlock(&poll_uart_rwlock);

			uint32_t* head = SRP_A(&poll_uart_head);
			uint32_t* tail = SRP_A(&poll_uart_tail);

			DBG1("recv start\thead: %td, tail: %td\n",
					(head)?head - poll_uart_buffer:-1,
					tail - poll_uart_buffer);

			SW_A(tail, c);
			if (NULL == head) {
				head = tail;
				SWP_A(&poll_uart_head, head);
			}
			tail++;
			if (tail == (poll_uart_buffer + POLL_UART_BUFSIZE))
				tail = poll_uart_buffer;
			SWP_A(&poll_uart_tail, tail);

			DBG1("recv end\thead: %td, tail: %td\t[%td=%c]\n",
					(head)?head - poll_uart_buffer:-1,
					tail - poll_uart_buffer,
					tail-poll_uart_buffer-1, *(tail-1));

			pthread_rwlock_unlock(&poll_uart_rwlock);
		}

		if (ret == 0) {
			INFO("UART client has closed connection"\
				"(no more data in but you can still send)\n");
		} else {
			WARN("Lost connection to UART client (%s)\n", strerror(errno));
			UART_SW(&poll_uart_client, INVALID_CLIENT);
		}
	}
}

static uint8_t poll_uart_status_read(void) {
	uint8_t ret = 0;

	pthread_rwlock_rdlock(&poll_uart_rwlock);
	ret |= (SRP_A(&poll_uart_head) != NULL) << POLL_UART_RXBIT; // data avail?
	ret |= (SR_A(&poll_uart_client) == INVALID_CLIENT) << POLL_UART_TXBIT; // tx busy?
	pthread_rwlock_unlock(&poll_uart_rwlock);

	// For lock contention
	nanosleep(&poll_uart_baud_sleep, NULL);

	return ret;
}

static void poll_uart_status_write(uint8_t val) {
	if (val & (1 << POLL_UART_RSTBIT)) {
		pthread_rwlock_wrlock(&poll_uart_rwlock);
		SWP_A(&poll_uart_head, NULL);
		SWP_A(&poll_uart_tail, poll_uart_buffer);
		pthread_rwlock_unlock(&poll_uart_rwlock);
	}
}

static uint8_t poll_uart_rxdata_read(void) {
	uint8_t ret;

	pthread_rwlock_wrlock(&poll_uart_rwlock);
	if (NULL == SRP_A(&poll_uart_head)) {
		DBG1("Poll UART RX attempt when RX Pending was false\n");
		ret = SR_A(&poll_uart_buffer[3]); // eh... rand? 3, why not?
	} else {
		uint32_t* head = SRP_A(&poll_uart_head);
		uint32_t* tail = SRP_A(&poll_uart_tail);

		ret = *head;

		head++;
		if (head == (poll_uart_buffer + POLL_UART_BUFSIZE))
			head = poll_uart_buffer;
		if (head == tail)
			head = NULL;

		DBG1("rxdata end\thead: %td, tail: %td\t[%td=%c]\n",
				(head)?head - poll_uart_buffer:-1,
				tail - poll_uart_buffer,
				(head)?head-poll_uart_buffer:-1,(head)?*head:'\0');

		SWP_A(&poll_uart_head, head);
	}
	pthread_rwlock_unlock(&poll_uart_rwlock);

	return ret;
}

static void poll_uart_txdata_write(uint8_t val) {
	DBG2("UART write byte: %c %x\n", val, val);

	static ssize_t ret;

	uint32_t client = UART_SR(&poll_uart_client);
	if (INVALID_CLIENT == client) {
		DBG1("Poll UART TX ignored as client is busy\n");
		// no connected client (TX is busy...) so drop
	}
	else if (-1 ==  ( ret = send(client, &val, 1, 0))  ) {
		WARN("%d UART: %s\n", __LINE__, strerror(errno));
		UART_SW(&poll_uart_client, INVALID_CLIENT);
	}

	DBG2("UART byte sent %c\n", val);
}

//////////////////////////////////////////////////////////////////

// Wrappers to match function signature
static bool uart_read_status(
		uint32_t addr __attribute__ ((unused)),
		uint8_t *val,
		bool debugger __attribute__ ((unused))
		) {
	*val = poll_uart_status_read();
	return true;
}

static void uart_write_status(
		uint32_t addr __attribute__ ((unused)),
		uint8_t val,
		bool debugger __attribute__ ((unused))
		) {
	poll_uart_status_write(val);
}

static bool uart_read_data(
		uint32_t addr __attribute__ ((unused)),
		uint8_t *val,
		bool debugger __attribute__ ((unused))
		) {
	*val = poll_uart_rxdata_read();
	return true;
}

static void uart_write_data(
		uint32_t addr __attribute__ ((unused)),
		uint8_t val,
		bool debugger __attribute__ ((unused))
		) {
	poll_uart_txdata_write(val);
}

static pthread_t start_poll_uart(void *unused __attribute__ ((unused))) {
	// Spawn uart thread, waits until spawned to return
	pthread_mutex_lock(&poll_uart_mutex);
	pthread_create(&poll_uart_pthread, NULL, poll_uart_thread, NULL);
	pthread_cond_wait(&poll_uart_cond, &poll_uart_mutex);
	pthread_mutex_unlock(&poll_uart_mutex);
	return poll_uart_pthread;
}

__attribute__ ((constructor))
void register_memmap_uart(void) {
	union memmap_fn mem_fn;

	mem_fn.R_fn8 = uart_read_status;
	register_memmap("Poll UART", false, 1, mem_fn,
			POLL_UART_STATUS, POLL_UART_STATUS+1);
	mem_fn.W_fn8 = uart_write_status;
	register_memmap("Poll UART", true, 1, mem_fn,
			POLL_UART_STATUS, POLL_UART_STATUS+1);

	mem_fn.R_fn8 = uart_read_data;
	register_memmap("Poll UART", false, 1, mem_fn,
			POLL_UART_RXDATA, POLL_UART_RXDATA+1);

	mem_fn.W_fn8 = uart_write_data;
	register_memmap("Poll UART", true, 1, mem_fn,
			POLL_UART_TXDATA, POLL_UART_TXDATA+1);

	struct periph_time_travel tt = PERIPH_TIME_TRAVEL_NONE;
	register_periph_thread(start_poll_uart, THREAD_NAME,
			tt, &poll_uart_enabled, 0, NULL);
}
