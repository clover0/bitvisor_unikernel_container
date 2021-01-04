#!/usr/bin/env bash

set -eu

echo "build includeos and app"
cd ~/programs/IncludeOS
docker-compose exec builder /bin/bash -c \
 "cd build && source activate.sh && make && source deactivate.sh \
 && \
 cd ../app1/build && source activate.sh && cmake --build . && source deactivate.sh"
cd app1/build

echo "install to bv"
../install_to_bv.sh

cd ~/programs/my_bitvisor
./build.sh

echo "finish"
