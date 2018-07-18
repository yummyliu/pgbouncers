FROM alpine:3.7
MAINTAINER Yummyliu <sdwhlym@126.com>

WORKDIR /pgbouncers
ADD . /pgbouncers

RUN apk --update add autoconf autoconf-doc automake c-ares c-ares-dev curl gcc libc-dev libevent libevent-dev libtool make man libressl-dev pkgconfig && \
	# build
    cd /pgbouncers && ./autogen.sh && \
    ./configure --prefix=/pgbouncers/build --with-libevent=/usr/lib && make && \
    cp  ./pgbouncer /bin && \
    # cleanup
    cd /tmp && \
    rm -rf /tmp/pgbouncer*  && \
    apk del --purge autoconf autoconf-doc automake c-ares-dev curl gcc libc-dev libevent-dev libtool make man libressl-dev pkgconfig

EXPOSE      6432
ADD ./entrypoint.sh /pgbouncers
ENTRYPOINT  ["/pgbouncers/entrypoint.sh"]
