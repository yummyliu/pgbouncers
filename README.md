# PgBouncers
fork from https://github.com/pgbouncer/pgbouncer, and add haproxy support, in the future maybe turn to multiprocess dbpool。

```
PROXY_STRING + single space + INET_PROTOCOL + single space + CLIENT_IP + single space + PROXY_IP + single space + CLIENT_PORT + single space + PROXY_PORT + "\r\n"
```

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

## Building for OSX

./configure --with-openssl=/usr/local/Cellar/openssl/1.0.2q/

# Docker Usage
This is yet another docker image with pgbouncer, based on alpine.

Code Example
You can configure it by Environment variables:

```
$ docker run -d \
 --name=pgbouncer \
 -e DB_HOST=postgresql.example.com \
 -e DB_USER=admin \
 -e DB_PASSWORD=mypassword \
 brainsam/pgbouncer:latest
```

Or You can mount config file into docker container:

```
$ docker run -d \
 --name pgbouncer \
 -v pgbouncer-config-file:/etc/pgbouncer/pgbouncer.ini \
 brainsam/pgbouncer:latest
 ```

Installation
$ docker pull brainsam/pgbouncer:latest
Configuration
All configuration parameters of pgbouncer are available both by --env (use the same keys in upper case) and by mounting pgbouncer.ini into container.

Troubleshooting
docker logs <your-pgbouncer-container-name>

# Testing
docker run -d -p 6432:6432 --name pgbouncers -e DB_HOST=172.17.0.2 -e DB_USER=postgres -e DB_PASSWORD=123 pgbouncers:latest


