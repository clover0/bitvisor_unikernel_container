#!/usr/bin/env bash

set -eu

#
# build IncludeOS and bitvisor
#

BASEDIR=~/programs

echo "build includeos and app"
cd $BASEDIR/IncludeOS
docker-compose exec builder /bin/bash -c \
 "cd build && source activate.sh && make && source deactivate.sh \
 && \
 cd ../app1/build && source activate.sh && cmake --build . && source deactivate.sh"

echo "copy includeos bin to bitvisor"
cd $BASEDIR/my_bitvisor
rm -f process/includeos.bin
rm -f process/includeos.bin.s

cd $BASEDIR/IncludeOS/app1/build
../install_to_bv.sh

cd $BASEDIR/my_bitvisor
./build.sh

echo "finish"
