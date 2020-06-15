#!/usr/bin/env bash

set -eu

function main() {
  dev="$1"
  part="$2"
  if [ "$dev" = "" ]; then
    echo "arg1: require device name (e.g. sdb)"
    exit 1
  fi
  if [ "$part" = "" ]; then
    echo "arg2: require partition name (e.g. sdb1)"
    exit 1
  fi

  echo "fortmatting USB device..."
  sudo dd if=/dev/zero of=/dev/"$dev" bs=512 count=1
  sudo parted /dev/"$dev" mklabel msdos
  sudo parted /dev/"$dev" "mkpart p fat32 262144s -0"
  sudo mkfs.vfat -c /dev/"$part"
  echo "finish format."
}

main "$1" "$2"
