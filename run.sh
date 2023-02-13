#!/bin/bash


source cs450-setup

make

clear

qemu-system-i386 -nographic -kernel kernel.bin -s

chmod u+x run.sh
