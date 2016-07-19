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

#include "gdb.h"

#include "state_sync.h"
#include "simulator.h"

#include "cpu/registers.h"
#include "cpu/core.h"

#include <sys/socket.h>
#include <netinet/in.h>

#define REG_STR_LEN (16 * 8 + 1) // 16 regs * 8 ascii chars each + NULL
#define GDB_MSG_MAX (2*REG_STR_LEN) // Minimum is REG_STR_LEN

static int sock = 0;
static char escape_chars[] = "}$#*";
//static char empty_resp[] = "$#00";

EXPORT void gdb_init(int port) {
	assert((sock == 0) && "Multiple calls to gdb_init");

	int serv_sock;
	serv_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (serv_sock == -1) {
		perror("opening gdb socket");
		exit(errno);
	}

	struct sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = htonl(INADDR_ANY);
	server.sin_port = htons(port);

	if (-1 == bind(serv_sock, (struct sockaddr*) &server, sizeof server)) {
		perror("binding gdb socket");
		exit(errno);
	}

	socklen_t length = sizeof server;
	if (-1 == getsockname(serv_sock, (struct sockaddr*) &server, &length)) {
		perror("getting gdb port");
		exit(errno);
	}

	listen(serv_sock, 1);

	INFO("Waiting for gdb connection on port %d\n",
			ntohs(server.sin_port));

	sock = accept(serv_sock, NULL, 0);
	if (sock == -1) {
		perror("accepting gdb connection");
		exit(errno);
	}

	// Grab the opening '+'
	{
		char c;
		ssize_t ret = recv(sock, &c, 1, 0);
		if (ret != 1) {
			ERR(E_UNKNOWN, "Unexpected error during gdb connection:\n%s",
					strerror(errno));
		}
		if (c != '+') {
			WARN("Unexpected character during initial gdb connection\n");
			ERR(E_UNKNOWN, "Expected '+', got '%c'", c);
		}
	}

	// Get the initial gdb support message
	const char *msg;
	long len;
	msg = gdb_get_message(&len);

	DBG2("Got msg >>>%s<<<, len %ld, strlen %zd\n", msg, len, strlen(msg));

	if (0 != strncmp("qSupported", msg, strlen("qSupported"))) {
		WARN("No support from gdb host for qSupported message\n");
		ERR(E_NOT_IMPLEMENTED, "Your version of gdb is too old\n");
	}

	// Respond with out message
	char *resp;
#ifdef HAVE_REPLAY
	if (-1 == asprintf(&resp, "\
qSupported:PacketSize=%x;ReverseContinue+;ReverseStep+", GDB_MSG_MAX - 1))
#else
	if (-1 == asprintf(&resp, "\
qSupported:PacketSize=%x", GDB_MSG_MAX - 1))
#endif
		ERR(E_UNKNOWN, "Error allocating response string\n");

	gdb_send_message(resp);
	free(resp);

	INFO("Connection initialized successfully\n");
}

EXPORT char* gdb_get_message(long *ext_len) {
	static char buf[GDB_MSG_MAX];
	memset(buf, 0, GDB_MSG_MAX);

	int read = 0;
	long len = 0;

	do {
		if (read >= GDB_MSG_MAX) {
			WARN("Max message length (%d) exceeded (read %d)",
					GDB_MSG_MAX, read);
			WARN("Msg thus far >>>%s<<<\n", buf);
			ERR(E_UNKNOWN, "Why did gdb ignore PacketSize??\n");
		}

		ssize_t ret;
		ret = recv(sock, buf+read, GDB_MSG_MAX-read, 0);

		if (ret < 0) {
			if (ret == EINTR) {
				ERR(E_NOT_IMPLEMENTED, "TODO: GDB handle EINTR case\n");
			} else
			if (ret == ENOMEM) {
				// try again
				;
			} else {
				perror("receving from gdb");
				exit(errno);
			}
		} else
		if (ret == 0) {
			ERR(E_UNKNOWN, "Connection closed unexpectedly\n");
		} else {
			read += ret;
		}

		char *end = memchr(buf, '#', GDB_MSG_MAX);
		len = end ? (end - buf + 1) : 0;
	} while (len == 0);

	assert((len+2) < GDB_MSG_MAX);

	if (read > (len + 2)) {
		WARN("Read beyond one message boundary\n");
		WARN("read %d len %ld (len + 2 %ld)\n", read, len, len+2);
		WARN(">>>%s<<<\n", buf);
		WARN("This should not be possible, as gdb commands are single messages\n");
		ERR(E_UNPREDICTABLE, "Lost protocol sync?\n");
	} else
	if (read < (len + 2)) {
		ssize_t ret;
		ret = recv(sock, buf+read, (len+2)-read, MSG_WAITALL);
		if (ret != (len+2)-read)
			ERR(E_UNKNOWN, "Communication error with gdb: %s\n", strerror(errno));
	}

	unsigned char csum = 0;
	char *cur = buf + 1;
	while (*cur != '#')
		csum += *cur++;

	if (csum != strtol(buf + len, NULL, 16)) {
		WARN("Checksum mismatch!\n");
		WARN("Expected: %2x Got: %s\n", csum, buf+len);
		WARN("buf: >>>%s<<<\n", buf);
		ERR(E_UNPREDICTABLE, "Bad Checksum\n");
	} else {
		// Send ACK
		send(sock, "+", 1, 0);
	}

	DBG2("Got message >>>%s<<<, len %ld, my_csum %2x\n", buf, len+2, csum);

	if (NULL != ext_len)
		*ext_len = len - 2; // omit $ and #

	buf[len - 1] = '\0'; // overwrite #

	return buf + 1; // point ahead of $
}

static void _gdb_send(const char c) {
#ifdef DEBUG2
	static int csum = 0;
	if (c == '$') {
		printf(PP_STRING" I: ");
		printf("Sending: >$");
		csum = 0;
	}
	else if (c == '#') {
		printf("#");
		csum = 2;
	}
	else if (csum > 1) {
		printf("%c", c);
		csum--;
	}
	else if (csum == 1) {
		printf("%c<\n", c);
		csum--;
	} else {
		printf("%c", c);
	}
#endif

	ssize_t ret = send(sock, &c, 1, 0);
	if (ret != 1) {
		perror(PP_STRING" W");
		ERR(E_UNKNOWN, "Failed to send message to gdb\n");
	}
}

static void gdb_send(const char *msg, long len) {
#ifdef DEBUG2
	flockfile(stdout);
#endif
	while (len) {
		const char *c = escape_chars;
		while (*c != '\0') {
			if (*c == *msg) {
				_gdb_send('}');
				_gdb_send(*msg ^ 0x20);
				break;
			}
			c++;
		}
		if (*c == '\0') {
			_gdb_send(*msg);
		}
		msg++;
		len--;
	}
#ifdef DEBUG2
	funlockfile(stdout);
#endif
}

EXPORT void gdb_send_message(const char *msg) {
	unsigned char csum = 0;
	const char *cur = msg;
	while (*cur != '\0')
		csum += *cur++;

	char csum_str[3];
	sprintf(csum_str, "%02x", csum);

	_gdb_send('$');
	gdb_send(msg, strlen(msg));
	_gdb_send('#');
	_gdb_send(csum_str[0]);
	_gdb_send(csum_str[1]);

	char c;
	ssize_t ret;
	ret = recv(sock, &c, 1, 0);
	if (ret != 1)
		ERR(E_UNKNOWN, "Communication with gdb failed: %s", strerror(errno));

	if (c == '+')
		return;
	else if (c == '-')
		return gdb_send_message(msg);
	else
		ERR(E_UNKNOWN, "Expected + or -, got %c\n", c);
}

/* We expect gdb to issue some form of command to indicate how
   long we should run. This function (or its delegates) should
   set variables such that the simulator stops as requested, and
   should return false when prepared for the simulator to run freely */
static bool _wait_for_gdb(void) {
	// In general, the empty response is allowed for unknown messages. For now,
	// we try to explicitly identify the messages we don't recognize, and bail
	// out on completely new ones.
	DBG2("Waiting for a message from gdb...\n");
	long cmd_len;
	char *cmd = gdb_get_message(&cmd_len);

	switch (cmd[0]) {
		case '?':
			// Report why the target halted
			// Right now SIGTRAP is the only option
			gdb_send_message("S05");
			break;

		case 'b':
		{
#ifdef HAVE_REPLAY
			if (cmd[1] == 's') {
				if (cycle > 0) {
					if (simulator_state_seek(cycle - 1)) {
						gdb_send_message("E00");
						return true;
					} else {
						gdb_send_message("S05");
						return true;
					}
				}else {
					WARN("Request to seek before beginning\n");
					gdb_send_message("E00");
					return true;
				}
			} else
			if (cmd[1] == 'c') {
				if (cycle) {
					if (simulator_state_seek(0)) {
						gdb_send_message("E00");
						return true;
					} else {
						gdb_send_message("S05");
						return true;
					}
				} else {
					WARN("Request to seek to current cycle ignored\n");
					gdb_send_message("S05");
					return true;
				}
			} else {
				goto unknown_gdb;
			}
#else
			WARN("Simulator was compiled without HAVE_REPLAY\n");
			WARN("Time-travel is not supported\n");
			goto unknown_gdb;
#endif
		}

		case 'c':
		{
			if (cmd_len == 1) {
				// Basic "c" continue message
				return false;
			} else {
				WARN("Request to continue at specific address currently unsupported, ignoring\n");
				goto unknown_gdb;
			}
		}

		case 'g':
		{
			char buf[REG_STR_LEN];

			snprintf(buf, REG_STR_LEN,
					"%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x%08x", 
					htonl(CORE_reg_read(0)),
					htonl(CORE_reg_read(1)),
					htonl(CORE_reg_read(2)),
					htonl(CORE_reg_read(3)),
					htonl(CORE_reg_read(4)),
					htonl(CORE_reg_read(5)),
					htonl(CORE_reg_read(6)),
					htonl(CORE_reg_read(7)),
					htonl(CORE_reg_read(8)),
					htonl(CORE_reg_read(9)),
					htonl(CORE_reg_read(10)),
					htonl(CORE_reg_read(11)),
					htonl(CORE_reg_read(12)),
					htonl(CORE_reg_read(13)),
					htonl(CORE_reg_read(14)),
					htonl(CORE_reg_read(15))
				);

			gdb_send_message(buf);
			break;
		}

		case 'G':
		{
			// Write all registers
			int i;
			cmd++; // advance past G
			for (i=0; i<16; i++) {
				char buf[9] = {0};
				memcpy(buf, cmd, 8);
				cmd += 8;
				CORE_reg_write(i, ntohl(strtol(buf, NULL, 16)));
				DBG2("Set r%02d=%08x\n",
						i, ntohl(strtol(buf, NULL, 16)));
			}
			gdb_send_message("OK");
			break;
		}

		case 'H':
		{
			// H operations target specific threads, but we have
			// no notion of threads, so we bail with an unsupported
			// response for all of them
			gdb_send_message("");
			break;
		}

		case 'k':
		{
			// kill
			INFO("Killed by remote debugger, dying\n");
			sim_terminate(true);
		}

		case 'm':
		{
			// m addr,length

			// XXX: Make efficient / pretty
			const char *address = strtok(cmd+1, ",");
			const char *length = strtok(NULL, ",");
			assert(NULL != length);
			uint32_t addr = (uint32_t) strtoul(address, NULL, 16);
			unsigned long len = strtoul(length, NULL, 16);
			long resp_len = len * 2 + 1;

			DBG2("addr %d (%s) len %ld (%s)\n", addr, address, len, length);
			{
				char buf[resp_len];
				char *head = buf;
				while (len) {
					uint8_t val;
					if (gdb_read_byte(addr, &val)) {
						sprintf(head, "%02x", val);
					} else {
						// I thought you could write X's
						// to indicate read failure of a
						// given byte, but gdb wasn't
						// happy with that, so we'll
						// fail the whole request
						gdb_send_message("");
						return true;
					}
					head += 2;
					addr++;
					len--;
				}

				gdb_send_message(buf);
			}
			break;
		}

		case 'M':
		{
			// M addr,length:bytes

			const char *address = strtok(cmd+1, ",");
			const char *length = strtok(NULL, ":");
			const char *bytes = strtok(NULL, "");
			assert(NULL != length);
			assert(NULL != bytes);

			uint32_t addr = (uint32_t) strtol(address, NULL, 16);
			unsigned long len = strtoul(length, NULL, 16);

			while (len) {
				char buf[3] = {0};
				buf[0] = bytes[0];
				buf[1] = bytes[1];
				uint8_t val = strtol(buf, NULL, 16);
				gdb_write_byte(addr, val);
				DBG2("0x%x=0x%02x\n", addr, val);
				addr++;
				bytes += 2;
				len--;
			}

			gdb_send_message("OK");
			break;
		}

		case 'p':
		{
			uint32_t val;

			// p reg
			if (cmd[2] != '\0') {
				if (0 == strcmp(cmd, "p19")) {
					// Inferred from gdb's messages, 0x19
					// is looking for the apsr, but I'll be
					// dammned if I could find any proof
					union apsr_t apsr = CORE_apsr_read();
					val = apsr.storage;
				} else {
					WARN("Request for illegal register: 0x%s\n",
							cmd + 1);
					// I cannot find documentation on the
					// gdb error codes, nor can I find any
					// actual usage in gdb's remote.c, so...
					gdb_send_message("E00");
					return true;
				}
			} else {
				uint8_t reg = strtol(cmd+1, NULL, 16);
				val = CORE_reg_read(reg);
			}

			char buf[9];
			sprintf(buf, "%08x", val);
			gdb_send_message(buf);
			break;
		}

		case 'P':
		{
			// P reg = val
			const char *regstr = strtok(cmd+1, "=");
			const char *value = strtok(NULL, "");
			assert(strlen(regstr) == 1);

			uint8_t reg = strtol(regstr, NULL, 16);
			uint32_t val = ntohl(strtol(value, NULL, 16));
			CORE_reg_write(reg, val);

			gdb_send_message("OK");
			break;
		}

		case 'q':
		{
			if (0 == strcmp("qC", cmd)) {
				// Asking for info on current thread, we don't
				// support multithread, so an empty response says
				// the 'default' thread is running, which is okay
				gdb_send_message("");
			} else if (0 == strcmp("qfThreadInfo", cmd)) {
				// Asking for info on all threads. Nope -> empty.
				gdb_send_message("");
			} else if (0 == strncmp("qL", cmd, 2)) {
				// qL... messages are the legacy form of qfThreadInfo
				gdb_send_message("");
			} else if (0 == strcmp("qAttached", cmd)) {
				// Wants to know whether attached to a process
				// or spawned one, but we're bare metal and do
				// not support this, so empty
				gdb_send_message("");
			} else if (0 == strcmp("qTStatus", cmd)) {
				// Wants to know the status of any running traces
				// https://sourceware.org/gdb/onlinedocs/gdb/Tracepoint-Packets.html#Tracepoint-Packets
				// gdb_send_message("tnotrun:0");
				// -> Bogus trace status reply from target: tnotrun:0
				// Odd. We'll go with the empty / unsupported reply then
				gdb_send_message("");
			} else {
				goto unknown_gdb;
			}
			break;
		}

		case 's':
		{
			if (0 == strcmp("s", cmd)) {
				dumpatcycle = cycle + 1;
				return false;
			} else {
				goto unknown_gdb;
			}
		}

		/* Okay, never mind this for now, this interface is just too
		   poorly documented to be worth it.
		case 'v':
		{
			if (0 == strcmp("vCont?", cmd)) {
				// Probing for supported vCont commands
				// Which is a lie, since gdb will only succeed if
				// you report that you support all of them. Largely
				// we do however (signals make no sense in this
				// context), so let's lie a bit
				gdb_send_message("vCont;c;C;s;S;t");
			} else
			if (0 == strcmp("vCont;c", cmd)) {
				// "Continue" command
				return false;
			} else
			if (0 == strcmp("vCont;s", cmd)) {
				dumpatcycle = cycle+1;
			} else {
				goto unknown_gdb;
			}
			break;
		}
		*/
		case 'v':
		{
			// Just because we don't support it doesn't mean we
			// don't recognize it
			gdb_send_message("");
			return true;
		}

		case 'X':
		{
			// The undocumented, 'more efficient' binary transfer
			// protocol, should replicate 'M' at half the character
			// count

			// Xaddr,length:XX...

			const char *address = strtok(cmd+1, ",");
			const char *length = strtok(NULL, ":");

			uint32_t addr = (uint32_t) strtoul(address, NULL, 16);
			unsigned long len = strtoul(length, NULL, 16);

			//                 X          addr       ,          length    :
			unsigned msg_len = 1 + strlen(address) + 1 + strlen(length) + 1;
			DBG2("X, address %s addr %d length %s len %lu msg_len %d cmd_len %ld\n",
					address, addr, length, len, msg_len, cmd_len);

			unsigned char* data = (unsigned char*) cmd+msg_len;

			while (len) {
				if (*data == '}') {
					data++;
					*data ^= 0x20;
				}
				gdb_write_byte(addr, *data);
				DBG2("0x%08x = %02x\n", addr, *data);
				addr++;
				len--;
				data++;
			}
			gdb_send_message("OK");

			break;
		}

		default:
unknown_gdb:
			WARN("Unknown gdb command: %s\n", cmd);
			gdb_send_message("");
	}

	return true;
}

void wait_for_gdb(void) {
	
	state_enter_debugging();
	while (_wait_for_gdb());
	state_exit_debugging();
}

void stop_and_wait_for_gdb(void) {
	DBG1("breaking to wait for gdb\n");
	gdb_send_message("S05");
	wait_for_gdb();
	DBG1("done waiting for gdb\n");
	fflush(stdout);
}
