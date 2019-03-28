/*
 * libusual - Utility library for C
 *
 * Copyright (c) 2007-2009 Marko Kreen, Skype Technologies OÜ
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */ 

/*
 * Wrappers around regular I/O functions (send/recv/read/write)
 * that survive EINTR and also can log problems.
 */

#include <usual/safeio.h>

#include <usual/socket.h>
#include <usual/logging.h>
#include <usual/string.h>
#include <usual/time.h>

const char v2sig[12] = "\x0D\x0A\x0D\x0A\x00\x0D\x0A\x51\x55\x49\x54\x0A";

int safe_read(int fd, void *buf, int len)
{
	int res;
loop:
	res = read(fd, buf, len);
	if (res < 0 && errno == EINTR)
		goto loop;
	return res;
}

int safe_write(int fd, const void *buf, int len)
{
	int res;
loop:
	res = write(fd, buf, len);
	if (res < 0 && errno == EINTR)
		goto loop;
	return res;
}

int safe_recv(int fd, void *buf, int len, int flags)
{
	int res;
	char ebuf[128];
loop:
	res = recv(fd, buf, len, flags);
	if (res < 0 && errno == EINTR)
		goto loop;
	if (res < 0)
		log_noise("safe_recv(%d, %d) = %s", fd, len,
			  strerror_r(errno, ebuf, sizeof(ebuf)));
	else if (cf_verbose > 2)
		log_noise("safe_recv(%d, %d) = %d", fd, len, res);
	return res;
}

int safe_send(int fd, const void *buf, int len, int flags)
{
	int res;
	char ebuf[128];
loop:
	res = send(fd, buf, len, flags);
	if (res < 0 && errno == EINTR)
		goto loop;
	if (res < 0)
		log_noise("safe_send(%d, %d) = %s", fd, len,
			  strerror_r(errno, ebuf, sizeof(ebuf)));
	else if (cf_verbose > 2)
		log_noise("safe_send(%d, %d) = %d", fd, len, res);
	return res;
}

int safe_close(int fd)
{
	int res;

#ifndef WIN32
	/*
	 * POSIX says close() can return EINTR but fd state is "undefined"
	 * later.  Seems Linux and BSDs close the fd anyway and EINTR is
	 * simply informative.  Thus retry is dangerous.
	 */
	res = close(fd);
#else
	/*
	 * Seems on windows it can returns proper EINTR but only when
	 * WSACancelBlockingCall() is called.  As we don't do it,
	 * ignore EINTR on win32 too.
	 */
	res = closesocket(fd);
#endif
	if (res < 0) {
		char ebuf[128];
		log_warning("safe_close(%d) = %s", fd,
			    strerror_r(errno, ebuf, sizeof(ebuf)));
	} else if (cf_verbose > 2) {
		log_noise("safe_close(%d) = %d", fd, res);
	}

	/* ignore EINTR */
	if (res < 0 && errno == EINTR)
		return 0;

	return res;
}

int safe_recvmsg(int fd, struct msghdr *msg, int flags)
{
	int res;
	char ebuf[128];
loop:
	res = recvmsg(fd, msg, flags);
	if (res < 0 && errno == EINTR)
		goto loop;
	if (res < 0)
		log_warning("safe_recvmsg(%d, msg, %d) = %s", fd, flags,
			    strerror_r(errno, ebuf, sizeof(ebuf)));
	else if (cf_verbose > 2)
		log_noise("safe_recvmsg(%d, msg, %d) = %d", fd, flags, res);
	return res;
}

int safe_sendmsg(int fd, const struct msghdr *msg, int flags)
{
	int res;
	int msgerr_count = 0;
	char ebuf[128];
loop:
	res = sendmsg(fd, msg, flags);
	if (res < 0 && errno == EINTR)
		goto loop;

	if (res < 0) {
		log_warning("safe_sendmsg(%d, msg[%d,%d], %d) = %s", fd,
			    (int)msg->msg_iov[0].iov_len,
			    (int)msg->msg_controllen,
			    flags, strerror_r(errno, ebuf, sizeof(ebuf)));

		/* with ancillary data on blocking socket OSX returns
		 * EMSGSIZE instead of blocking.  try to solve it by waiting */
		if (errno == EMSGSIZE && msgerr_count < 20) {
			struct timeval tv = {1, 0};
			log_warning("trying to sleep a bit");
			select(0, NULL, NULL, NULL, &tv);
			msgerr_count++;
			goto loop;
		}
	} else if (cf_verbose > 2)
		log_noise("safe_sendmsg(%d, msg, %d) = %d", fd, flags, res);
	return res;
}

int safe_connect(int fd, const struct sockaddr *sa, socklen_t sa_len)
{
	int res;
	char buf[128];
	char ebuf[128];
loop:
	res = connect(fd, sa, sa_len);
	if (res < 0 && errno == EINTR)
		goto loop;
	if (res < 0 && (errno != EINPROGRESS || cf_verbose > 2))
		log_noise("connect(%d, %s) = %s", fd,
			  sa2str(sa, buf, sizeof(buf)),
			  strerror_r(errno, ebuf, sizeof(ebuf)));
	else if (cf_verbose > 2)
		log_noise("connect(%d, %s) = %d", fd, sa2str(sa, buf, sizeof(buf)), res);
	return res;
}

int safe_accept(int fd, struct sockaddr *sa, socklen_t *sa_len_p)
{
	int res;
	char buf[128];
	char ebuf[128];
loop:
	res = accept(fd, sa, sa_len_p);
	if (res < 0 && errno == EINTR)
		goto loop;
	if (res < 0)
		log_noise("safe_accept(%d) = %s", fd,
			  strerror_r(errno, ebuf, sizeof(ebuf)));
	else if (cf_verbose > 2)
		log_noise("safe_accept(%d) = %d (%s)", fd, res, sa2str(sa, buf, sizeof(buf)));
	return res;
}

/* returns
 * =0 if needs to poll
 * <0 upon error
 * >0 if it did the job
 */
int safe_accept_proxy(int fd, struct sockaddr *from, socklen_t *from_len)
{
    Header hdr;
    int size=0, ret=-1;

	char ebuf[128];
	char buf[128];
    int client_fd;
loop:
    client_fd = accept(fd, from, from_len);
    if (client_fd < 0 && errno == EINTR) {
        goto loop;
    }
    if (client_fd < 0) {
        log_noise("safe_accept_proxy(%d) = %s", fd, strerror_r(errno, ebuf, sizeof(ebuf)));
        return client_fd;
    }

    if (cf_verbose > 2)
        log_noise("safe_accept_proxy(%d) = %d (%s)", fd, client_fd, sa2str(from, buf, sizeof(buf)));

    do {
        ret = recv(client_fd, &hdr, sizeof(hdr), MSG_PEEK);
    } while (ret == -1 && errno == EINTR);

    if (ret == -1)
        return (errno == EAGAIN) ? 0 : -1;

    if (ret >= 16 && memcmp(&hdr.v2, v2sig, 12) == 0 &&
            (hdr.v2.ver_cmd & 0xF0) == 0x20) {
        size = 16 + ntohs(hdr.v2.len);
        if (ret < size)
            return -1; /* truncated or too large header */

        switch (hdr.v2.ver_cmd & 0xF) {
            case 0x01: /* PROXY command */
                switch (hdr.v2.fam) {
                    case 0x11:  /* TCPv4 */
                        ((struct sockaddr_in *)from)->sin_family = AF_INET;
                        ((struct sockaddr_in *)from)->sin_addr.s_addr = hdr.v2.addr.ip4.src_addr;
                        ((struct sockaddr_in *)from)->sin_port = hdr.v2.addr.ip4.src_port;

                        log_info("proxy v2 ipv4 ip (%s)", sa2str(from, buf, sizeof(buf)));
                        goto done;
                    case 0x21:  /* TCPv6 */
                        ((struct sockaddr_in6 *)from)->sin6_family = AF_INET6;
                        memcpy(&((struct sockaddr_in6 *)from)->sin6_addr,
                                hdr.v2.addr.ip6.src_addr, 16);
                        ((struct sockaddr_in6 *)from)->sin6_port =
                            hdr.v2.addr.ip6.src_port;
                        goto done;
                }
                break;
            case 0x00: /* LOCAL command */
                log_noise("v2 local\n");
                break;
            default:
                log_noise("default\n");
                return -1; /* not a supported command */
        }
    }
    else if (ret >= 8 && memcmp(hdr.v1.line, "PROXY", 5) == 0) {
        char *end = (char*)memchr(hdr.v1.line, '\r', ret - 1);
        if (!end || end[1] != '\n')
            return -1; /* partial or invalid header */
        *end = '\0'; /* terminate the string to ease parsing */
        size = end + 2 - hdr.v1.line; /* skip header + CRLF */
        /* parse the V1 header using favorite address parsers like inet_pton.
         * return -1 upon error, or simply fall through to accept.
         */
    }
    else {/* Wrong protocol */
        return -1;
    }

done:
	if (cf_verbose > 2) {
		log_info("remained size %d", size);
	}
    do { /* we need to consume the appropriate amount of data from the socket */
        ret = recv(client_fd, &hdr, size, 0);
    } while (ret == -1 && errno == EINTR);
    return (ret >= 0) ? client_fd : -1;
}
