[![Tests](https://github.com/xiangguand/fdt-reader/actions/workflows/test.yaml/badge.svg)](https://github.com/xiangguand/fdt-reader/actions/workflows/test.yaml)

<!-- ABOUT THE PROJECT -->
## About The Project
This project is to dump the .dtb file and show tree-like printout.  
The DTB format can refer to the doc "devicetree-specification-v0.4.pdf".


<!-- GETTING STARTED -->
## Getting Started
You can use `dtc` command to compile *.dts file to *.dtb file.
```bash
./fdt_header dt00.dtb
```
Then, use my program to dump the 
```bash
./fdt_header dt00.dtb
```
  
  
Example printout of my program 
```plaintext
File patterns/dt01.dtb size is 1098 bytes
All values below are in HEX representation.
magic: d00dfeed
totalsize: 44a
off_dt_struct: 38
off_dt_strings: 420
off_mem_rsvmap: 28
version: 11
last_comp_version: 10
boot_cpuid_phys: 0
size_dt_strings: 2a
size_dt_struct: 3e8
fdt header size is 28
===== Tree the node start =====
Root node: -
>#address-cells: 
>#size-cells: 
>compatible: acme,coyotes-revenge
--cpus
-->#address-cells: 
-->#size-cells: 
----cpu@0
---->compatible: arm,cortex-a9
---->reg: 
----cpu@1
---->compatible: arm,cortex-a9
---->reg: 
--serial@101F0000
-->compatible: arm,pl011
-->reg: 
--serial@101F2000
-->compatible: arm,pl011
-->reg:  
--gpio@101F3000
-->compatible: arm,pl061
-->reg: 0@
--interrupt-controller@10140000
-->compatible: arm,pl190
-->reg: 
--spi@10115000
-->compatible: arm,pl022
-->reg: P
--external-bus
-->#address-cells: 
-->#size-cells: 
----ethernet@0,0
---->compatible: smc,smc91c111
---->reg: 
----i2c@1,0
---->compatible: acme,a1234-i2c-bus
---->#address-cells: 
---->#size-cells: 
---->reg: 
------rtc@58
------>compatible: maxim,ds1338
------>reg: 
----flash@2,0
---->compatible: samsung,k8f1315ebmcfi-flash
---->reg: 
===== Tree the node end =====
```


### Installation
*Setup cmake*
```bash
cmake -S . -B build -G"Ninja"
```
  
*Build the program*
```bash
ninja -C build
```
  
Now the output program `fdt-reader` locates in build folder


<!-- USAGE EXAMPLES -->
## Usage
*Example:* 
```bash
./fdt_header dt00.dtb
```


<!-- CONTACT -->
## Contact
If you have any issues, please feel welcome to report them or contact me via email refers as follows:  
xiangguand@gmail.com



