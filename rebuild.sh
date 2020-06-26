#!/usr/bin/env bash

set -eu

function main() {
#  dev="$1"
#  part="$2"
#  if [ "$dev" = "" ]; then
#    echo "arg1: require device name (e.g. sdb)"
#    exit 1
#  fi
#  if [ "$part" = "" ]; then
#    echo "arg2: require partition name (e.g. sdb1)"
#    exit 1
#  fi
  echo "clean"
#  make clean
  rm -f boot/uefi-loader/loadvmm.efi
  rm -f boot/loader/bootloader boot/loader/bootloaderusb

#  echo "build..."
#  make
#  make -C boot/loader
#  make -C boot/uefi-loader

#  echo "fortmatting USB device..."
#  sudo dd if=/dev/zero of=/dev/"$dev" bs=512 count=1
#  sudo parted /dev/"$dev" mklabel msdos
#  sudo parted /dev/"$dev" "mkpart p fat32 262144s -0"
#  sudo mkfs.vfat -c /dev/"$part"
#  echo "finish format."

#  echo "install uefi to USB"
#  mkdir /tmp/mnt
#  sudo mount /dev/"$part" /tmp/mnt
#  sudo mkdir -p /tmp/mnt/EFI/BOOT
#  sudo cp ./boot/uefi-loader/loadvmm.efi /tmp/mnt/EFI/BOOT/BOOTX64.EFI
#  sudo cp ./bitvisor.elf /tmp/mnt/EFI/BOOT/BITVISOR.ELF
#  sudo umount /tmp/mnt
#  rmdir /tmp/mnt
#  echo "finish install."

}

main
