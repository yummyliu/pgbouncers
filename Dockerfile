FROM alpine:3.7
MAINTAINER Yummyliu <sdwhlym@126.com>

WORKDIR /pgbouncers
ADD . /pgbouncers

# Inspiration from https://github.com/gmr/alpine-pgbouncer/blob/master/Dockerfile
#RUN groupadd -r pgbouncer && useradd -r -g pgbouncer pgbouncer

RUN \
  # Download
  apk --update add autoconf autoconf-doc automake c-ares c-ares-dev curl gcc libc-dev libevent libevent-dev libtool make man libressl-dev pkgconfig &&\
  cd /pgbouncers && \
  ./configure --prefix=/usr && \
  make && \

  # Manual install
  cp pgbouncer /usr/bin && \
  mkdir -p /etc/pgbouncer /var/log/pgbouncer /var/run/pgbouncer && \
  # entrypoint installs the configuation, allow to write as postgres user
  cp etc/pgbouncer.ini /etc/pgbouncer/pgbouncer.ini.example && \
  cp etc/userlist.txt /etc/pgbouncer/userlist.txt.example && \
  chown -R postgres /var/run/pgbouncer /etc/pgbouncer && \
  # Cleanup
  cd /tmp && \
  rm -rf /tmp/pgbouncer*  && \
  apk del --purge autoconf autoconf-doc automake c-ares-dev curl gcc libc-dev libevent-dev libtool make man libressl-dev pkgconfig

USER postgres
EXPOSE 5432
CMD ["/usr/bin/pgbouncer", "/etc/pgbouncer/pgbouncer.ini"]
