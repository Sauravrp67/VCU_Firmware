#!/usr/bin/env bash
set -euo pipefail

config="${1:-release}"
image="build/${config}/VCU_Firmware.hex"

if [[ ! -f "${image}" ]]; then
  echo "missing ${image}; run scripts/build.sh ${config} first" >&2
  exit 1
fi

if command -v STM32_Programmer_CLI >/dev/null 2>&1; then
  STM32_Programmer_CLI -c port=SWD -w "${image}" -v -rst
elif command -v openocd >/dev/null 2>&1; then
  openocd -f interface/stlink.cfg -f target/stm32f1x.cfg \
    -c "program ${image} verify reset exit"
else
  echo "no supported flashing tool found" >&2
  echo "install STM32CubeProgrammer CLI or OpenOCD with ST-LINK support" >&2
  exit 1
fi
