PACKAGE_NAME = pgbouncer
PACKAGE_TARNAME = pgbouncer
PACKAGE_VERSION = 1.8.1
PACKAGE_STRING = pgbouncer 1.8.1
PACKAGE_URL = 
PACKAGE_BUGREPORT = 
PORTNAME = unix
EXEEXT = 
HAVE_CC_DEPFLAG = yes
CC = gcc -std=gnu99
CPP = gcc -std=gnu99 -E
CPPFLAGS = 
CFLAGS = -g -O2
DEFS = -DHAVE_CONFIG_H
WFLAGS =  -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -Wmissing-prototypes -Wpointer-arith -Wendif-labels -Wdeclaration-after-statement -Wold-style-definition -Wstrict-prototypes -Wundef -Wformat=2 -Wuninitialized
CXX = @CXX@
CXXFLAGS = @CXXFLAGS@
LD = @LD@
LDFLAGS = -g  -Wl,--as-needed
LIBS = -levent 
AR = ar
ARFLAGS = rcu
RANLIB = ranlib
LIBTOOL = @LIBTOOL@
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_SCRIPT = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644
MKDIR_P = /usr/bin/mkdir -p
SED = /usr/bin/sed
AWK = gawk
GREP = /usr/bin/grep
EGREP = /usr/bin/grep -E
STRIP = strip
prefix = /home/liuyangming/build
exec_prefix = ${prefix}
bindir = ${exec_prefix}/bin
includedir = ${prefix}/include
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datarootdir = ${prefix}/share
datadir = ${datarootdir}
sysconfdir = ${prefix}/etc
docdir = ${datarootdir}/doc/${PACKAGE_TARNAME}
mandir = ${datarootdir}/man
libdir = ${exec_prefix}/lib
localedir = ${datarootdir}/locale
pkgdatadir = ${datarootdir}/pgbouncer
pkgconfigdir = ${libdir}/pkgconfig
abs_top_srcdir ?= /home/liuyangming/pgbouncers
abs_top_builddir ?= /home/liuyangming/pgbouncers
nosub_top_srcdir ?= .
nosub_top_builddir ?= .

CARES_CFLAGS =  
CARES_LIBS = -lcares  

TLS_CPPFLAGS = 
TLS_LDFLAGS = 
TLS_LIBS = -lssl -lcrypto

RST2MAN = rst2man
DLLWRAP = 
DLLTOOL = 
WINDRES = 

enable_debug = yes
have_libevent = yes
