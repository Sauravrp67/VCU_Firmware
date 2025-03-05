#!/usr/bin/env bash
# Format (mode "fix", default) or check (mode "check") the hand-written firmware
# sources with clang-format. Vendored HAL/FreeRTOS/FatFs and the CubeMX-generated
# files (main.c, *_hal_msp.c, *_it.c, system_*, FreeRTOSConfig.h, main.h, …) are
# intentionally excluded so they keep ST's formatting.
set -euo pipefail
cd "$(dirname "$0")/.."

mode="${1:-fix}"

mapfile -t files < <(git ls-files \
  'Core/control/*.c' 'Core/control/*.h' \
  'Core/safety/*.c'  'Core/safety/*.h' \
  'Core/proto/*.c'   'Core/proto/*.h' \
  'Core/bsp/*.h' \
  'Core/Src/app/*.c' 'Core/Inc/app/*.h' \
  'Core/Src/drivers/*.c' 'Core/Inc/drivers/*.h' \
  'Core/Src/app.c' 'Core/Inc/app.h' \
  'Core/Src/board.c' 'Core/Inc/board.h' \
  'test/*.c' 'test/*.h')

case "$mode" in
  check) clang-format --dry-run --Werror "${files[@]}" ;;
  fix)   clang-format -i "${files[@]}"; echo "formatted ${#files[@]} files" ;;
  *)     echo "usage: $0 [fix|check]" >&2; exit 2 ;;
esac
