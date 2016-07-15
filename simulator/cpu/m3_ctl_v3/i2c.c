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

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "i2c.h"
#include "ice_bridge.h"

#include "cpu/periph.h"
#include "cpu/core.h"

//#include "core/state_sync.h"

struct i2c_instance {
	volatile bool en;
	void (*recv_fn)(uint8_t, uint32_t, char *);
	uint8_t ones;
	uint8_t zeros;
	const char *name;
	char *host;
	uint16_t port;
	int sock;
	bool is_connected;
	bool is_sending;
	bool is_sending_resp;
	bool is_active_message;
	pthread_t pt;
	pthread_mutex_t pm;
	pthread_cond_t pc;
};

struct i2c_message {
	uint8_t address;
	uint32_t length;
	char *bytes;
};

struct i2c_acknowledge {
	char acked;
};

struct i2c_packet {
	char type;
	union {
		struct i2c_message m;
		struct i2c_acknowledge a;
	};
};

static int i2c_connect(int sock, const char *host, const uint16_t port) {
	int len;
	struct sockaddr_un address;

	address.sun_family = AF_UNIX;
	strcpy(address.sun_path, host);
	sprintf(address.sun_path+strlen(address.sun_path), ".%d", port);
	len = sizeof(address);

	if (-1 == connect(sock, (struct sockaddr*) &address, len)) {
		static int warn_once = 0;
		if (warn_once) {
			WARN("Connecting I2C Bus: %s\n", strerror(errno));
			warn_once++;
		}
		return -1;
	}

	char handshake = 'i';
	send(sock, &handshake, 1, 0);
	// XXX: Add timeout
	if (1 != recv(sock, &handshake, 1, 0)) {
		WARN("Connection dropped? %s\n", strerror(errno));
		close(sock);
		return -1;
	}
	if ('i' != handshake) {
		WARN("Bad handshake with software I2C Bus.\n");
		WARN("Expected 'i', Got '%c'\n", handshake);
		close(sock);
		return -1;
	}

	return sock;
}

static struct i2c_packet* i2c_recv_packet(int sock) {
	struct i2c_packet* p = malloc(sizeof(struct i2c_packet));

	// XXX: Timeouts
	if (1 != recv(sock, &p->type, 1, 0)) {
		WARN("Bus shutdown. Disconnected\n");
		WARN("Socket reports: %s\n", strerror(errno));
		goto i2c_recv_packet_die;
	}

	if (p->type == 0) {
		// Message
		if (1 != recv(sock, &p->m.address, 1, 0)) {
			WARN("Socket error expecting address\n");
			WARN("Socket reports: %s\n", strerror(errno));
			goto i2c_recv_packet_die;
		}
		if (4 != recv(sock, &p->m.length, 4, MSG_WAITALL)) {
			WARN("Socket error expecting length\n");
			WARN("Socket reports: %s\n", strerror(errno));
			goto i2c_recv_packet_die;
		}
		p->m.length = ntohl(p->m.length);
		p->m.bytes = malloc(p->m.length);
		if (NULL == p->m.bytes) {
			WARN("Failed to allocate memory for I2C packet\n");
			WARN("Message was of length: %d\n", p->m.length);
			WARN("Dropping packet and disconnecting.\n");
			goto i2c_recv_packet_die;
		}
		if (p->m.length != recv(sock, p->m.bytes, p->m.length, MSG_WAITALL)) {
			WARN("Socket error expecting message bytes\n");
			WARN("Socket reports: %s\n", strerror(errno));
			goto i2c_recv_packet_die_with_bytes;
		}
		DBG1("Got a message packet (dest: %02x)\n", p->m.address);
	} else if (p->type == 1) {
		// Acknowledge
		if (1 != recv(sock, &p->a.acked, 1, 0)) {
			WARN("Socket error expecting ACK/NAKE\n");
			WARN("Socket reports: %s\n", strerror(errno));
			goto i2c_recv_packet_die;
		}
		DBG1("Got an acknowledgment packet (acked: %s)\n",
				(p->a.acked) ? "true" : "false");
	} else {
		WARN("Bad message type %d. Disconnecting\n", p->type);
		goto i2c_recv_packet_die;
	}

	return p;

i2c_recv_packet_die_with_bytes:
	free(p->m.bytes);
i2c_recv_packet_die:
	close(sock);
	free(p);
	return NULL;
}

EXPORT bool i2c_send_message(struct i2c_instance* t,
		uint8_t address, uint32_t length, const char *msg) {
	pthread_mutex_lock(&t->pm);

	if ( ! t->is_connected) {
		pthread_mutex_unlock(&t->pm);
		WARN("Request to send I2C message, but simulator is not connected to a bus.\n");
		return false;
	}

#ifdef DEBUG1
	{
	flockfile(stdout); flockfile(stderr);
	DBG1("len %d to 0x%02x: ", length, address);
	unsigned i;
	for (i=0; i<length; i++) {
		printf("%x ", msg[i]);
	}
	printf("\n");
	funlockfile(stderr); funlockfile(stdout);
	}
#endif //DEBUG1

	t->is_sending = true;

	const char message_type = 0;
	uint32_t nlen = htonl(length);
#ifdef __APPLE__
#define MSG_MORE 0 // hack hack hack
#endif
	if (1 != send(t->sock, &message_type, 1, MSG_MORE))
		goto i2c_send_die;
	if (1 != send(t->sock, &address, 1, MSG_MORE))
		goto i2c_send_die;
	if (4 != send(t->sock, &nlen, 4, MSG_MORE))
		goto i2c_send_die;
	if (length != send(t->sock, msg, length, 0))
		goto i2c_send_die;
#ifdef __APPLE__
#undef MSG_MORE
#endif

	// Wait for NAK/ACK from listener thread
	pthread_cond_wait(&t->pc, &t->pm);

	bool acked = t->is_sending_resp;
	t->is_sending = false;
	pthread_mutex_unlock(&t->pm);

	return acked;

i2c_send_die:
	WARN("Unexpected error while sending I2C message.\n");
	WARN("Socket reports: %s\n", strerror(errno));
	close(t->sock);
	t->is_connected = false;
	pthread_mutex_unlock(&t->pm);
	return false;
}

static void i2c_propogate_recv(struct i2c_instance *t, struct i2c_packet *p) {
	assert(p->type == 0);
	assert(t->recv_fn != NULL);
	CORE_ERR_not_implemented("An interrupt mecahnism :(. I2C Recv");
}

static void* i2c_thread(void *v_args) {
	struct i2c_instance* t = (struct i2c_instance*) v_args;

	//                    0123456789012345 <-- max thread name
	char thread_name[] = "i2c:            ";
	strncpy(thread_name+strlen("i2c: "), t->name, 16-strlen("i2c: "));

#ifdef __APPLE__
	if (0 != pthread_setname_np(thread_name))
#else
	if (0 != prctl(PR_SET_NAME, thread_name, 0, 0, 0))
#endif
		ERR(E_UNKNOWN, "Setting thread name: %s\n", strerror(errno));

	t->sock = socket(AF_UNIX, SOCK_STREAM, 0);
	if (-1 == t->sock) {
		ERR(E_UNKNOWN, "Creating I2C device: %s\n", strerror(errno));
	}

	INFO("Bus interface '%s' created\n", thread_name);

	pthread_cond_signal(&t->pc);
	///////////////////////////////


	while (1) {
		pthread_mutex_lock(&t->pm);
		while (t->is_connected == false) {
			pthread_mutex_unlock(&t->pm);
			while (-1 == i2c_connect(t->sock, t->host, t->port)) {
				if (!t->en) goto i2c_shutdown;
				sleep(1);
			}
			pthread_mutex_lock(&t->pm);
			t->is_connected = true;
		}
		pthread_mutex_unlock(&t->pm);

		struct i2c_packet *p = i2c_recv_packet(t->sock);
		if (NULL == p) {
			pthread_mutex_lock(&t->pm);
			t->is_connected = false;
			pthread_mutex_unlock(&t->pm);
			continue;
		}

		bool matched = false;
		pthread_mutex_lock(&t->pm);
		if (p->type == 0) {
			// Incoming message (async event)
			if (t->is_sending) {
				DBG1("Lost Arbitration, switching to recv\n");
				t->is_sending = false;
				pthread_cond_signal(&t->pc);
			}
			t->is_active_message = true;

			uint8_t ones_match  = (p->m.address & t->ones);
			uint8_t zeros_match = (~p->m.address) & (~t->zeros);
			if (ones_match && zeros_match) {
				matched = true;

				// This message is for us, ACK it
				const char* ack_pkt = "11";
				send(t->sock, ack_pkt, 2, 0);

				// And propogate up the stack
				i2c_propogate_recv(t, p);
			}
		} else if (p->type == 1) {
			// Incoming ACK/NAK
			if (t->is_sending) {
				t->is_sending_resp = p->a.acked;
				pthread_cond_signal(&t->pc);
			} else if (t->is_active_message) {
				t->is_active_message = false;
			} else {
				WARN("Spurious ACK. Dropped.\n");
			}
		} else {
			assert(false && "i2c_recv_packet returned invalid p?");
		}
		pthread_mutex_unlock(&t->pm);

		if (!matched) {
			if (p->type == 0)
				free(p->m.bytes);
			free(p);
		}
	}

i2c_shutdown:
	assert(!t->en);
	INFO("Bus interface '%s' shut down\n", thread_name);
	free(t->host);
	free(t);
	pthread_exit(NULL);
}

static pthread_t start_i2c(void *v_args) {
	struct i2c_instance* t = (struct i2c_instance*) v_args;

	// Spawn i2c thread, waits until spawned to return
	pthread_mutex_lock(&t->pm);
	pthread_create(&t->pt, NULL, i2c_thread, t);
	pthread_cond_wait(&t->pc, &t->pm);
	pthread_mutex_unlock(&t->pm);
	return t->pt;
}

// This function *MUST* be called from a peripheral's constructor
EXPORT struct i2c_instance* create_i2c_instance(const char *periph_name,
		void (*async_recv_message)(uint8_t, uint32_t, char *),
		uint8_t ones_mask, uint8_t zeros_mask,
		const char *host, const uint16_t port) {
	struct i2c_instance *t = malloc(sizeof(struct i2c_instance));
	assert((t != NULL) && "Alloc i2c_instance");
	t->name = periph_name;
	t->recv_fn = async_recv_message;
	t->ones = ones_mask;
	t->zeros = zeros_mask;
	t->host = strdup(host);
	t->port = port;

	t->is_connected = false;
	t->is_sending = false;
	t->is_active_message = false;

	{
		int ret;
#ifdef DEBUG1
		pthread_mutexattr_t attr;
		if (0 != (ret = pthread_mutexattr_init(&attr)) )
			ERR(E_UNKNOWN, "pthread_mutexattr_init: %s", strerror(ret));
		if (0 != (ret = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK)) )
			ERR(E_UNKNOWN, "pthread_mutexattr_settype: %s", strerror(ret));
		if (0 != (ret = pthread_mutex_init(&t->pm, &attr)) )
			ERR(E_UNKNOWN, "pthread_mutex_init: %s", strerror(ret));
		if (0 != (ret = pthread_mutexattr_destroy(&attr)) )
			ERR(E_UNKNOWN, "pthread_mutexattr_destory: %s", strerror(ret));
#else
		if (0 != (ret = pthread_mutex_init(&t->pm, NULL)) )
			ERR(E_UNKNOWN, "pthread_mutex_init: %s", strerror(ret));
#endif
		if (0 != (ret = pthread_cond_init(&t->pc, NULL)) )
			ERR(E_UNKNOWN, "pthread_cond_init: %s", strerror(ret));
	}

	struct periph_time_travel tt = PERIPH_TIME_TRAVEL_NONE;
	register_periph_thread(start_i2c, "m3_ctl: ice", tt, &(t->en), 0, t);
	return t;
}
