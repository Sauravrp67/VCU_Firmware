#!/usr/bin/env bash
set -euo pipefail

config="${1:-release}"
elf="build/${config}/VCU_Firmware.elf"

if [[ ! -f "${elf}" ]]; then
  echo "missing ${elf}; run scripts/build.sh ${config} first" >&2
  exit 1
fi

arm-none-eabi-size "${elf}"
