# VCU Firmware

Firmware for the STM32F103RB-based Vehicle Control Unit used on a Formula Bharat 2025 electric-vehicle track project. The VCU coordinates driver inputs, brake plausibility checks, ready-to-drive state, dashboard telemetry, and CAN messaging on top of STM32 HAL and FreeRTOS.

This repository is presented as a firmware engineering artifact, not as a certified safety product. Review the code, hardware wiring, and rules compliance before using it on a vehicle.

## Target

- MCU: STM32F103RBTx, Cortex-M3, 128 KB flash, 20 KB SRAM
- Board assumption: NUCLEO-F103RB / custom VCU wiring derived from `VCU_Version1.ioc`
- STM32Cube firmware package: STM32Cube FW_F1 V1.8.6
- Middleware: FreeRTOS CMSIS-RTOS v2 wrapper, FatFs
- Interfaces: ADC, CAN, GPIO, I2C1, SPI2, USART1, USART2, USART3
- System clock: 8 MHz generated from HSI/PLL configuration in CubeMX

## Repository Layout

- `Core/`: application code, CubeMX-generated startup/peripheral initialization, tasks, and board support.
- `Drivers/`: vendored CMSIS and STM32F1 HAL sources generated with the project.
- `Middlewares/`: vendored FreeRTOS and FatFs sources.
- `FATFS/`: CubeMX FatFs app/target glue. The user disk backend is intentionally marked not ready.
- `cmake/`, `CMakeLists.txt`, `CMakePresets.json`: reproducible command-line build path.
- `scripts/`: build, flash, and size helpers.
- `docs/`: hardware, architecture, and bring-up notes.

## Build

Install the GNU Arm Embedded toolchain so `arm-none-eabi-gcc`, `arm-none-eabi-objcopy`, and `arm-none-eabi-size` are on `PATH`.

```sh
./scripts/build.sh debug
./scripts/build.sh release
```

Build outputs are written under `build/debug/` or `build/release/`:

- `VCU_Firmware.elf`: ELF image
- `VCU_Firmware.hex`: Intel HEX flash image
- `VCU_Firmware.bin`: raw binary image
- `VCU_Firmware.map`: linker map

Report memory usage:

```sh
./scripts/size.sh release
```

## Flash

With an ST-LINK connected over SWD:

```sh
./scripts/flash.sh release
```

The script tries `STM32_Programmer_CLI` first, then falls back to OpenOCD with `interface/stlink.cfg` and `target/stm32f1x.cfg`.

## CubeIDE

The original STM32CubeIDE project files are kept so the project can still be imported into CubeIDE for pin configuration and interactive debug:

- `.project`
- `.cproject`
- `.mxproject`
- `VCU_Version1.ioc`

Use CMake as the reproducible build path for command-line and CI builds.

## Current Limitations

- FatFs is configured, but no SD-card or block-device backend is implemented. Disk reads/writes return `RES_NOTRDY`.
- IMD, BMS, and BSPD fault inputs are not mapped in the current `.ioc`; the error task leaves those flags false until hardware pins are defined.
- USART3 is configured in the CubeMX project, but the current board support routes CLI to USART2 and dashboard text to USART1.
- This pass keeps control logic minimal and focuses on buildability, traceability, and professional presentation.
