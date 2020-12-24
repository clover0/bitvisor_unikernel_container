#!/usr/bin/env bash

set -eu

echo "build includeos and app"
cd ~/programs/IncludeOS
docker-compose exec builder /bin/bash -c "cd build && make && cd ../app1/build && cmake --build ."
cd app1/build

echo "install to bv"
../install_to_bv.sh

cd ~/programs/my_bitvisor
./build.sh

echo "finish"
