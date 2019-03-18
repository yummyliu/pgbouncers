/*
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

/** @file
 *
 * EINTR-safe I/O functions.
 */
#ifndef _USUAL_SAFEIO_H_
#define _USUAL_SAFEIO_H_

#include <usual/socket.h>
#define BUFFER_SIZE 1024
#define on_error(...) { fprintf(stderr, __VA_ARGS__); fflush(stderr); exit(1); }

/* Proxy protocol
 */
typedef union {
    struct {
        char line[108];
    } v1;
    struct {
        uint8_t sig[12];
        uint8_t ver_cmd;
        uint8_t fam;
        uint16_t len;
        union {
            struct {  /* for TCP/UDP over IPv4, len = 12 */
                uint32_t src_addr;
                uint32_t dst_addr;
                uint16_t src_port;
                uint16_t dst_port;
            } ip4;
            struct {  /* for TCP/UDP over IPv6, len = 36 */
                uint8_t  src_addr[16];
                uint8_t  dst_addr[16];
                uint16_t src_port;
                uint16_t dst_port;
            } ip6;
            struct {  /* for AF_UNIX sockets, len = 216 */
                uint8_t src_addr[108];
                uint8_t dst_addr[108];
            } unx;
        } addr;
    } v2;
} Header;

extern const char v2sig[12];

/** read */
int safe_read(int fd, void *buf, int len)                       _MUSTCHECK;
/** write */
int safe_write(int fd, const void *buf, int len)                _MUSTCHECK;
/** recv */
int safe_recv(int fd, void *buf, int len, int flags)            _MUSTCHECK;
/** send */
int safe_send(int fd, const void *buf, int len, int flags)      _MUSTCHECK;
/** close */
int safe_close(int fd);
/** recvmsg */
int safe_recvmsg(int fd, struct msghdr *msg, int flags)         _MUSTCHECK;
/** sendmsg */
int safe_sendmsg(int fd, const struct msghdr *msg, int flags)   _MUSTCHECK;
/** connect */
int safe_connect(int fd, const struct sockaddr *sa, socklen_t sa_len)   _MUSTCHECK;
/** accept */
int safe_accept(int fd, struct sockaddr *sa, socklen_t *sa_len) _MUSTCHECK;
/** accept with proxy */
int safe_accept_proxy(int fd, struct sockaddr *from, socklen_t *from_len) _MUSTCHECK;
#endif
