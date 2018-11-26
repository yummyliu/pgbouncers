# PgBouncers

Lightweight connection pooler for PostgreSQL.

Homepage
    https://pgbouncer.github.io

Sources, bugtracking
    https://github.com/pgbouncer/pgbouncer

## Building

PgBouncer depends on few things to get compiled:

* `GNU Make`_ 3.81+
* libevent_ 2.0
* (optional) OpenSSL_ 1.0.1 for TLS support.
* (optional) `c-ares`_ as alternative to libevent's evdns.

.. _GNU Make: https://www.gnu.org/software/make/
.. _libevent: http://libevent.org/
.. _OpenSSL: https://www.openssl.org/
.. _`c-ares`: http://c-ares.haxx.se/

When dependencies are installed just run::

    $ ./configure --prefix=/usr/local --with-libevent=libevent-prefix
    $ make
    $ make install

If you are building from git, or are building for Windows, please see
separate build instructions below.

## DNS lookup support

Starting from PgBouncer 1.4, it does hostname lookups at connect
time instead just once at config load time.  This requires proper
async DNS implementation.  Following list shows supported backends
and their probing order:

+----------------------------+----------+-----------+------------+----------------+---------------------------------------+
| backend                    | parallel | EDNS0 (1) | /etc/hosts | SOA lookup (2) | note                                  |
+============================+==========+===========+============+================+=======================================+
| c-ares                     | yes      | yes       | yes        | yes            | ipv6+CNAME buggy in <=1.10            |
+----------------------------+----------+-----------+------------+----------------+---------------------------------------+
| udns                       | yes      | yes       | no         | yes            | ipv4-only                             |
+----------------------------+----------+-----------+------------+----------------+---------------------------------------+
| evdns, libevent 2.x        | yes      | no        | yes        | no             | does not check /etc/hosts updates     |
+----------------------------+----------+-----------+------------+----------------+---------------------------------------+
| getaddrinfo_a, glibc 2.9+  | yes      | yes (3)   | yes        | no             | N/A on non-linux                      |
+----------------------------+----------+-----------+------------+----------------+---------------------------------------+
| getaddrinfo, libc          | no       | yes (3)   | yes        | no             | N/A on win32, requires pthreads       |
+----------------------------+----------+-----------+------------+----------------+---------------------------------------+
| evdns, libevent 1.x        | yes      | no        | no         | no             | buggy                                 |
+----------------------------+----------+-----------+------------+----------------+---------------------------------------+

1. EDNS0 is required to have more than 8 addresses behind one hostname.
2. SOA lookup is needed to re-check hostnames on zone serial change
3. To enable EDNS0, add `options edns0` to /etc/resolv.conf

`./configure` also has flags `--enable-evdns` and `--disable-evdns` which
turn off automatic probing and force use of either `evdns` or `getaddrinfo_a()`.

## PAM authorization

To enable PAM authorization `./configure` has a flag `--with-pam` (default value is no). When compiled with
PAM support new global authorization type `pam` appears which can be used to validate users through PAM.

## Building from GIT

Building PgBouncer from GIT requires that you fetch libusual
submodule and generate the header and config files before
you can run configure::

	$ git clone https://github.com/pgbouncer/pgbouncer.git
	$ cd pgbouncer
	$ git submodule init
	$ git submodule update
	$ ./autogen.sh
	$ ./configure ...
	$ make
	$ make install

Additional packages required: autoconf, automake, libevent-dev, libtool,
autoconf-archive, python-docutils, and pkg-config.

## Building for WIN32

At the moment only build env tested is MINGW32 / MSYS.  Cygwin
and Visual $ANYTHING are untested.  Libevent 2.x is required
for DNS hostname lookup.

Then do the usual::

	$ ./configure ...
	$ make

If cross-compiling from Unix::

	$ ./configure --host=i586-mingw32msvc ...

## Running on WIN32

Running from command-line goes as usual, except -d (daemonize),
-R (reboot) and -u (switch user) switches will not work.

To run pgbouncer as a Windows service, you need to configure
`service_name` parameter to set name for service.  Then::

	$ pgbouncer -regservice config.ini

To uninstall service::

	$ pgbouncer -unregservice config.ini

To use Windows Event Log, set "syslog = 1" in config file.
But before you need to register pgbevent.dll::

	$ regsvr32 pgbevent.dll

To unregister it, do::

        $ regsvr32 /u pgbevent.dll

# Docker Usage
This is yet another docker image with pgbouncer, based on alpine.

Code Example
You can configure it by Environment variables:

$ docker run -d \
 --name=pgbouncer \
 -e DB_HOST=postgresql.example.com \
 -e DB_USER=admin \
 -e DB_PASSWORD=mypassword \
 brainsam/pgbouncer:latest
Or You can mount config file into docker container:

$ docker run -d \
 --name pgbouncer \
 -v pgbouncer-config-file:/etc/pgbouncer/pgbouncer.ini \
 brainsam/pgbouncer:latest
Installation
$ docker pull brainsam/pgbouncer:latest
Configuration
All configuration parameters of pgbouncer are available both by --env (use the same keys in upper case) and by mounting pgbouncer.ini into container.

Troubleshooting
docker logs <your-pgbouncer-container-name>

# debug
docker run -d -p 6432:6432 --name pgbouncers -e DB_HOST=172.17.0.2 -e DB_USER=postgres -e DB_PASSWORD=123 pgbouncers:latest

# Road Map

1. message tool between dispatcher & worker

+ signal handler ;
+ message channel

2. worker listen on random port

3. dispatcher listen on configurable port, accepet TCP message and route to the 
