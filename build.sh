#!/usr/bin/env bash

set -eu

# docker-compose run --rm ubuntu1804 sh -c "make && make -C boot/loader && make -C boot/uefi-loader && make -C tools/dbgsh"

docker-compose exec ubuntu1804 sh -c "make && make -C boot/loader && make -C boot/uefi-loader && make -C tools/dbgsh"