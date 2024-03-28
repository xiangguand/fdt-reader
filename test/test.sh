#!/usr/bin/env bash

echo "Workspace: $(pwd)"

dut=./build/fdt_reader

echo "Buiild input device tree"
dtc -I dts -O dtb -o patterns/dt00.dtb patterns/dt00.dts
dtc -I dts -O dtb -o patterns/dt01.dtb patterns/dt01.dts

$dut
if [ $? -eq 0 ]; then
  exit 1
fi

$dut patterns/dt00.dtb
if [ $? -ne 0 ]; then
  exit 1
fi

$dut patterns/dt01.dtb
if [ $? -ne 0 ]; then
  exit 1
fi

$dut patterns/error_magic.dtb
if [ $? -eq 0 ]; then
  exit 1
fi

exit $?

