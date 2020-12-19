#!/usr/bin/env bash

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

  echo "copy EFI to USB-VOLUME"
  cp ./boot/uefi-loader/loadvmm.efi /Volumes/NO\ NAME/EFI/BOOT/BOOTX64.EFI
  cp ./bitvisor.elf /Volumes/NO\ NAME/EFI/BOOT/BITVISOR.ELF
  
  # cp ./* /Volumes/NO\ NAME/tools
  sync -f /Volumes/NO\ NAME/EFI/BOOT/BOOTX64.EFI
  echo "finish copy."

}

main "$1" "$2"
