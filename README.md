# arria10_dk_fpga_bridge_baremetal
Example project for baremetal application accessing FPGA devices using the lw_h2f bridge on the Arria10 devkit board

## Pre-requisites

* Quartus 20.1
* ARM Development Studio
* arm-eabi- toolchain
* ARM tools: Linking script

## Init submodules

> git submodule update --init --recursive

## Building bootloader

````
cd u-boot-socfpga
git checkout socfpga_v2020.04
make ARCH=arm CROSS_COMPILE=arm-eabi- socfpga_arria10_defconfig
make ARCH=arm CROSS_COMPILE=arm-eabi-
````
## Creating FPGA Programming file

````
cd ghrd_socfpga
# Make sure quartus can be called (binary in PATH and QUARTUS_ROOTDIR envvar is set)
make generate_from_tcl
make rbf
````

## Programming FPGA

````
quartus_pgm -c 1 -m jtag -o "p;ghrd_socfpga/a10_soc_devkit_ghrd_std/output_files/ghrd_10as066n2.sof"
````

## Build configuration

## Debug configuration

### Debug Script

## Known problems

### Bridge reset signal

The *lw_h2f* bridge's reset signal must be disabled for the FPGA devices to be accessible. This can be done by writing to register at address 0xXXXX

TODO: Find a way to do so in the SPL's config

### Disable watchdogs

The SPL sets some watchdogs that reset the board (and thus re-enables the bridge's reset signal). One of which seems to be wdt_1 as it can be seen to be activated at program start (register at address 0xXXXX). However, another, 30s timer seems to be enable in the config but it looks like it cannot be disabled in menuconfig (terminal flashes but box stays set) 