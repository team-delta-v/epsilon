#!/bin/sh
set -e

make clean
make EPSILON_VERSION=dv-git-$(git rev-parse --short HEAD)
arm-none-eabi-objcopy -O binary build/device/epsilon.elf epsilon.bin
