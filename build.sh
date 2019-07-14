#!/bin/sh
set -e

make clean
make
arm-none-eabi-objcopy -O binary build/device/epsilon.elf epsilon.bin
