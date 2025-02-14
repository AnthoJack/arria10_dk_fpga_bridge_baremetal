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

``` bash
cd u-boot-socfpga
git checkout socfpga_v2020.04
make ARCH=arm CROSS_COMPILE=arm-eabi- socfpga_arria10_defconfig
make ARCH=arm CROSS_COMPILE=arm-eabi-
```

## Programming FPGA

Before running any program on the card, the SoC should be programmed. We will use the Golden Standard Reference Design for this 

### Creating FPGA Programming file

``` bash
cd ghrd_socfpga
# Make sure quartus can be called (binary in PATH and QUARTUS_ROOTDIR envvar is set)
make generate_from_tcl
make rbf
```
### Programming FPGA using JTAG

``` bash
quartus_pgm -c 1 -m jtag -o "p;ghrd_socfpga/a10_soc_devkit_ghrd_std/output_files/ghrd_10as066n2.sof"
```

## Build configuration

Launch ARM-DS and create a new **C Project** (ours will be called arria10_dk_fpga) and in the **Toolchains** list select **GCC 6.2.0[arm-altera-eabi]** and then click on **Finish**

Right-click on you project and select **Properties**. In the window that opens, in the left menu, go to **C/C++ Build > Settings**. In the **Tool Settings** tab, go to **GCC C Linker 6.2.0[arm-altera-eabi] > Image**. In **Linker script** enter (or search using the **Browse** button) the linker script for the Arria10. It should be in **<IntelFPGA_install_folder>/University_Program/Monitor_Program/arm_tools/baremetal/arm-altera-eabi/lib/arria10-dk-ram-hosted.ld**. Save the settings and you should now be able to build using the **Hammer icon**

The repository provides the **arria10_dk_fpga** folder which contains an ARM-DS project that already contains the build and debug configurations but this documentation still explains how to remake them from scratch to show what was done in case it must be changed

## Debug configuration

In the **Run** menu, click on **Debug Configurations...**. In the window that opens, right-click on **Generic ARM C/C++ Application** and choose **New configuration**

Open your new Configuration. You can rename it in the field at the top to whatever you want. Well call ours "arria10_devkit". Then go into the **Connection** tab

In the **Select targer** box, Find and select the **Intel SoC FPGA > Arria 10 SoC > Bare Metal Debug > Debug Cortex-A9_0** platform and in the **Target Connection** dropdown menu, choose **USB-Blaster**

If you already have the Arria10 devkit turned on and JTAG connected to the computer, you can go to the **Connections** box and click on the **Browse** button next to the **Bare Metal Debug Connection** field and select you JTAG connection (Usually **USB-BlasterII**)

Go into the **Files** tab and in the **Target Configuration** box, select the **previously built .axf file** in **Application on host to download** either typing the path manually or using the **File System..** or **Workspace...** buttons. Make sure that **Load Symbol** is ticked

Finally, go to the **Debugger** tab and in the **Run control** box, select **Debug from symbol** and make sure that the field next to it contains **main**. Also tick the **Run target initialization debugger script** (Make sure you tick **target** and not **debug**) and select the **debug-spl.ds** debugger script in the field beneath. The file can be found at the root of the repository

With that, you should have a functionning debug configuration. If the Arria10 devkit is turned on, connected to the computer and programmed with the GSRD and the baremetal application built, you can click **Debug** or save the configuration and in **Debug Control** click on your new configuration and **Connect to Target** (Button on top or right-click)

### U-boot Debug Config

If U-boot is the application you wish to debug, you can do so by creating another debug configuration (we call ourselve's arria10_devkit_u-boot) from the previously configured one (right-click on the configuration and **Duplicate**). Then, in the **Files** tab, remove the path to the built program file. In the **Debugger** tab. Select **Connect only** and replace the target initialization script with **debug-u-boot** script (also found in the repository). You should now be able to launch and debug u-boot

## Resources

The steps in this repository have been tested with promising results but aren't guaranteed to work. Should they fail to give a functioning environment, the following resources provide more in-depth steps to help 

* [Arria 10 Register Address Map](https://www.intel.com/content/www/us/en/programmable/hps/arria-10/hps.html)
* [Using ARM DS with arria10](https://www.rocketboards.org/foswiki/Documentation/SoCEDS): Includes instructions on how to install ARM-DS and how to install ARM-DS (and SOCEDS but this tool is deprecated) and how to configure it to launch and debug baremetal programs on Arria10
* [Building U-boot for cyclone V and Arria 10](https://www.rocketboards.org/foswiki/Documentation/BuildingBootloaderCycloneVAndArria10): Steps to install the toolchain and build the bootloader. Some supplementary steps are described for "handoff" whose usefulness is yet to be found as it doesn't seem to make our project behave better or worse in our tests without doing it
* [Arria 10 SoC GSRD](https://www.rocketboards.org/foswiki/Documentation/Arria10SoCGSRD): Documentation on the Golden Standard Reference Design for the devkit

## Known problems

### Bridge reset signal

The *lw_h2f* bridge's reset signal must be disabled for the FPGA devices to be accessible. This can be done by using the mask *0x2* on the register at address 0x0xffd0502c

Going forward, it would be interesting to find a way to do so in the SPL's config so the program doesn't need to do it itself

### Disable watchdogs

The SPL sets some watchdogs that reset the board (and thus re-enables the bridge's reset signal). One of which seems to be wdt_1 as it can be seen to be activated at program start (register at address 0xffd00300). However, another, 30s timer seems to be enabled by the SPL but it cannot be disabled in menuconfig (terminal flashes but box stays set). Other combinations of config options related to the watchdog have been attempted to no avail