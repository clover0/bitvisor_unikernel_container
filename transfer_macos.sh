#!/usr/bin/env bash
#
# transfer to USB for macOS
#

set -eu

function main() {
  dev="$1"
  part="$2"
  #
  # show USB device
  #    diskutil list
  # 
  if [ "$dev" = "" ]; then
    echo "arg1: require device name (e.g. disk2)"
    exit 1
  fi
  if [ "$part" = "" ]; then
    echo "arg2: require partition name (e.g. disk2s1)"
    exit 1
  fi
#  echo "clean"
#  make clean
#  rm -f boot/uefi-loader/loadvmm.efi
#  rm -f boot/loader/bootloader boot/loader/bootloaderusb

#  echo "build..."
#  make
#  make -C boot/loader
#  make -C boot/uefi-loader


  echo "install uefi to USB"
  mkdir -p /tmp/mnt
  diskutil unmount /dev/"$part"
  sudo diskutil mount -mountPoint /tmp/mnt /dev/"$part"
  sudo mkdir -p /tmp/mnt/EFI/BOOT /tmp/mnt/tools
  cp ./boot/uefi-loader/loadvmm.efi /tmp/mnt/EFI/BOOT/BOOTX64.EFI
  cp ./bitvisor.elf /tmp/mnt/EFI/BOOT/BITVISOR.ELF
  cp ./test.c /tmp/mnt/tools
  sync -f /tmp/mnt/EFI/BOOT/BOOTX64.EFI
  diskutil unmount /tmp/mnt 
  rmdir /tmp/mnt
  echo "finish install."

}

main "$1" "$2"
