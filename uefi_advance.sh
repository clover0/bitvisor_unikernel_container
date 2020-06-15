#!/usr/bin/env bash

set -eu

function main() {

  mkdir /tmp/mnt
  sudo mount /dev/sdb1 /tmp/mnt
  sudo mkdir -p /tmp/mnt/EFI/BOOT
  sudo cp ./boot/uefi-loader/loadvmm.efi /tmp/mnt/EFI/BOOT/BOOTX64.EFI
  sudo sh copyelf.sh ./bitvisor.elf /tmp/mnt/EFI/BOOT/BITVISOR.ELF

  sudo umount /tmp/mnt
  rmdir /tmp/mnt
  sync
  sudo eject /dev/sdb
  #  lba=$(sudo hdparm --fibmap /tmp/mnt/EFI/BOOT/BITVISOR.ELF)
  #  echo "LBA: ${lba}"
}

main
