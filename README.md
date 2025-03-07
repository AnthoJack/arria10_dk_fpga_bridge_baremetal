# arria10_dk_fpga_bridge_baremetal
Example project for baremetal application accessing FPGA devices using the lw_h2f bridge on the Arria10 devkit board

## Pre-requisites

* Quartus (20.1)
* ARM Development Studio (2020.1)
* arm-eabi- toolchain (Linaro 7.5.0)
* (optionally) ARM tools: Arria10 Linking script (Provided with IntelFPGA installation)

## Init submodules

Initialise the submodules to retrieve the Bootloader (u-boot-socfpga) and Golden Hardware Reference Design

> git submodule update --init --recursive

## Building bootloader

``` bash
cd u-boot-socfpga
git checkout socfpga_v2020.04
make ARCH=arm CROSS_COMPILE=arm-eabi- socfpga_arria10_defconfig
make ARCH=arm CROSS_COMPILE=arm-eabi-
```

## Programming FPGA

Before running any program on the card, the SoC should be programmed. We will use the Golden Hardware Reference Design for this. You may also use your own design if you have one but be aware that this may require changes to the later configurations and codes

### Creating FPGA Programming file

Synthesize and generate the GHRD bitstream

``` bash
cd ghrd_socfpga
# Make sure quartus can be called (binary in PATH and QUARTUS_ROOTDIR envvar is set)
make generate_from_tcl
make rbf
```
### Programming FPGA using JTAG

This step may be done from inside Quartus but the command line is provided for automation purposes

``` bash
quartus_pgm -c 1 -m jtag -o "p;ghrd_socfpga/a10_soc_devkit_ghrd_std/output_files/ghrd_10as066n2.sof"
```

## Example project and HWLib

Altera's [Socfpga Hardware Library (intel-socfpga_hwlib)](https://github.com/altera-opensource/intel-socfpga-hwlib) is included as a submodule. This repository provides a Hardware Abstraction Layer as well as linker scripts and code examples for the Arria10 and Cyclone V devkits

### Create project from example

Creating a project from an example allows minimal setup to be done while ending up with a project that supports the HWLib. Modifications can then be made to replace or add features 

* Launch ARM-DS and click on **New Project**
* In the window that opens, click on **C/C++ > Makefile Project  with Existing Code**, then **Next>**
* Click on **Browse...** and open the **intel-socfpga-hwlib/examples/A10/Altera-SoCFPGA-HelloWorld-Baremetal-GNU** folder
* Untick **C++**, select **GCC 6.2.0 [arm-altera-eabi]** and click **Finish**
* You should now be able to build using the **Hammer icon**

The **Makefile** inside the project allows you to configure the build. Take a look inside it

The **ghrd_fpga** folder in this repository contains a tiny library with helper functions to access the switches and leds of the A10's FPGA using the GHRD design and can be used as a base to develop drivers for the devices on the FPGA. To include it to the project:

* Right-click on the project and click **New > Folder**
* Click **Advanced>>** and select the **Link to alternate location (Linked Folder)** then use **Browse...** to select the **ghrd_fpga** folder before clicking **Finish**

## Debug configuration

* In the **Run** menu, click on **Debug Configurations...**. In the window that opens, right-click on **Generic ARM C/C++ Application** and choose **New configuration**
* Open your new Configuration. You can rename it in the field at the top to whatever you want. Well call ours "arria10_devkit". Then go into the **Connection** tab
* In the **Select targer** box, Find and select the **Intel SoC FPGA > Arria 10 SoC > Bare Metal Debug > Debug Cortex-A9_0** platform and in the **Target Connection** dropdown menu, choose **USB-Blaster**
* If you already have the Arria10 devkit turned on and JTAG connected to the computer, you can go to the **Connections** box and click on the **Browse** button next to the **Bare Metal Debug Connection** field and select you JTAG connection (Usually **USB-BlasterII**)
* Go into the **Files** tab and in the **Target Configuration** box, select the **previously built .axf file** in **Application on host to download** either typing the path manually or using the **File System..** or **Workspace...** buttons. Make sure that **Load Symbol** is ticked
* Finally, go to the **Debugger** tab and in the **Run control** box, select **Debug from symbol** and make sure that the field next to it contains **main**. Also tick the **Run target initialization debugger script** (Make sure you tick **target** and not **debug**) and select the **debug-spl.ds** debugger script in the field beneath. The file can be found at the root of the repository

With that, you should have a functionning debug configuration. If the Arria10 devkit is turned on, connected to the computer and programmed with the GHRD and the baremetal application built, you can now launch and debug the example by clicking **Debug**. Otherwise save the configuration and once you have a compiling C program, go to **Debug Control**, click on your new configuration and **Connect to Target** (Button on top or right-click). The program will be deployed onto the card and paused once it enters the *main* function

### U-boot Debug Config

If U-boot is the application you wish to debug, you can do so by creating another debug configuration (we call ours *arria10_devkit_u-boot*) from the previously configured one (right-click on the configuration and **Duplicate**). Then, in the **Files** tab, remove the path to the built program file. In the **Debugger** tab. Select **Connect only** and replace the target initialization script with **debug-u-boot** script (also found in the repository). You should now be able to launch and debug u-boot

## Resources

The steps in this repository have been tested but aren't guaranteed to work with new updates. Should they fail to give a functioning environment, the following resources provide more in-depth steps to help try to make it work again

* [Arria 10 Register Address Map](https://www.intel.com/content/www/us/en/programmable/hps/arria-10/hps.html)
* [Using ARM DS with arria10](https://www.rocketboards.org/foswiki/Documentation/SoCEDS): Includes instructions on how to install ARM-DS and how to install ARM-DS (and SOCEDS but this tool is deprecated) and how to configure it to launch and debug baremetal programs on Arria10
* [Building U-boot for cyclone V and Arria 10](https://www.rocketboards.org/foswiki/Documentation/BuildingBootloaderCycloneVAndArria10): Steps to install the toolchain and build the bootloader. Some supplementary steps are described for "handoff" whose usefulness is yet to be found as it doesn't seem to make our project behave better or worse in our tests without doing it
* [Arria 10 SoC GSRD](https://www.rocketboards.org/foswiki/Documentation/Arria10SoCGSRD): Documentation on the Golden Standard Reference Design for the devkit
* **<IntelFPGA_install_folder>/University_Program/Monitor_Program/arm_tools/baremetal/arm-altera-eabi/lib/arria10-dk-ram-hosted.ld** is another linker script that showed promising results and might be a little bit easier to edit in order to create a linker script for a custom platform but it requires the IntelFPGA tools to be installed

## Notes

### Bridge reset signal

The *lw_h2f* bridge's reset signal must be disabled for the FPGA devices to be accessible. This can be done by using the mask *0x2* on the register at address *0x0xffd0502c* or using the HWLib's *alt_bridge_init(ALT_BRIDGE_LWH2F)* (include *soc_a10/alt_bridge_manager.h*)

It may be interesting to find a way to do so in the SPL's config so the program doesn't need to do it itself

### Disable watchdogs

The SPL sets some watchdogs that reset the board (and thus re-enables the bridge's reset signal) after some time. Those cannot be disabled using their registers once activated and disabling them in menuconfig is impossible (Dependant on SOCFPGA parameter, terminal flashes but box stays set) without deeper modifications in Kconfig files. However, the HWLib provides functions that use the peripheral reset to disable it