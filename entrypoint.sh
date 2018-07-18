#!/bin/bash
set -e

PGB_LOG=/var/log/pgbouncer/
PGB_CONFIG=/etc/pgbouncer/pgbouncer.ini
PGB_USER=pgbouncer

mkdir -p ${PGB_LOG}
chmod -R 755 ${PGB_LOG}
chown -R ${PGB_USER}:${PGB_USER} ${PGB_LOG}

echo "Starting pgbouncer..."
exec pgbouncer -q -u ${PGB_USER} $PGB_CONFIG
