FROM alpine:3.7
MAINTAINER Yummyliu <sdwhlym@126.com>

WORKDIR /pgbouncers
ADD . /pgbouncers

# Inspiration from https://github.com/gmr/alpine-pgbouncer/blob/master/Dockerfile

RUN apk --update add autoconf autoconf-doc automake c-ares c-ares-dev curl gcc libc-dev libevent libevent-dev libtool make man libressl-dev pkgconfig && \
    cd /pgbouncers && ./configure --prefix=/usr && make && \
    cp pgbouncer /usr/bin && \
	mkdir -p /etc/pgbouncer /var/log/pgbouncer /var/run/pgbouncer && \
	cp etc/pgbouncer.ini /etc/pgbouncer/ && \
	cp etc/userlist.txt /etc/pgbouncer/ && \
	adduser -D -S pgbouncer && \
	chown pgbouncer /var/run/pgbouncer && \
	cd /tmp && \
	rm -rf /tmp/pgbouncer*  && \
	sed -i 's/logfile = \/var\/log\/pgbouncer\/pgbouncer.log/; logfile = \/var\/log\/pgbouncer\/pgbouncer.log/' /etc/pgbouncer/pgbouncer.ini && \
    # Cleanup
    cd /tmp && \
    rm -rf /tmp/pgbouncer*  && \
    apk del --purge autoconf autoconf-doc automake c-ares-dev curl gcc libc-dev libevent-dev libtool make man libressl-dev pkgconfig

USER pgbouncer

EXPOSE      6432
ENTRYPOINT  ["/pgbouncers/entrypoint.sh"]
