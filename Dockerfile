FROM centos:7 AS build_stage

MAINTAINER Yummyliu <sdwhlym@126.com>

# all the apt-gets in one command & delete the cache after installing

# Install build dependencies
RUN yum update \
    && yum install -y \
       build-essential libevent-dev ca-certificates curl git

EXPOSE 5432

RUN groupadd -r pgbouncer && useradd -r -g pgbouncer pgbouncer

# ENV PGBOUNCER_URL http://pgbouncer.github.io/downloads/files/${PGBOUNCER_VERSION}/pgbouncer-${PGBOUNCER_VERSION}.tar.gz
# ENV PGBOUNCER_VERSION 1.8.1
ENV PGBOUNCER_URL https://github.com/yummyliu/pgbouncers.git

# Get PgBouncer source code
RUN git clone --depth=1 ${PGBOUNCER_URL} \
  && chown root:root pgbouncers

# Configure, make, and install
RUN cd pgbouncers \
  && ./configure --prefix=/usr/local --with-libevent=libevent-prefix \
  && make \
  && make install

ADD pgbouncer.ini /var/app/pgbouncer/pgbouncer.ini
ADD auth_file.ini /var/app/pgbouncer/auth_file.ini

RUN chown pgbouncer:pgbouncer /var/app/pgbouncer/ -R \
     && chmod a+w /var/log -R \
    && chmod a+w /var/run -R

CMD pgbouncer /var/app/pgbouncer/pgbouncer.ini
