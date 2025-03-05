# Bring-Up Guide

## Toolchain

Required for CLI builds:

- CMake 3.22 or newer
- Make
- GNU Arm Embedded toolchain: `arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, `arm-none-eabi-size`

Optional for flashing:

- STM32CubeProgrammer CLI: `STM32_Programmer_CLI`
- OpenOCD with ST-LINK support

## Build

```sh
./scripts/build.sh debug
./scripts/build.sh release
```

The build should produce ELF, HEX, BIN, and MAP outputs under `build/<config>/`.

## Flash

```sh
./scripts/flash.sh release
```

The default path assumes SWD through ST-LINK.

## Smoke Test

1. Power the target from a current-limited bench supply.
2. Connect ST-LINK/SWD and flash the release image.
3. Confirm the MCU resets cleanly and does not enter `HardFault_Handler`.
4. Open USART2 at 115200 baud for CLI/debug bring-up when CLI output is added.
5. Open USART1 at 115200 baud to observe dashboard text output.
6. Exercise APPS and brake analog inputs with safe bench signals before connecting vehicle sensors.
7. Verify CAN bus wiring and termination before enabling vehicle network traffic.

## Known Bring-Up Gaps

- FatFs has no physical media driver.
- IMD/BMS/BSPD inputs need final pin assignments.
- CAN message IDs and payload ownership need a documented vehicle CAN matrix.
- The firmware has not been audited as production safety software.
