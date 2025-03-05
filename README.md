# VCU Firmware

[![ci](https://github.com/Sauravrp67/VCU_Firmware/actions/workflows/ci.yml/badge.svg)](https://github.com/Sauravrp67/VCU_Firmware/actions/workflows/ci.yml)

Firmware for an STM32F103RB-based **Vehicle Control Unit (VCU)** for a Formula
Bharat 2025 / FSAE-EV electric race car. The VCU acquires the pedal sensors,
enforces the FSAE-EV plausibility and shutdown rules, runs the Ready-To-Drive
state machine, commands torque to the inverter over CAN, and drives dashboard
telemetry — on top of the STM32 HAL and FreeRTOS.

> This repository is an engineering artifact, **not** a certified safety product.
> The CAN message catalog is a placeholder and several behaviors require
> race-engineer review and hardware-in-the-loop validation before any on-car use
> (see [`docs/discrepancies.md`](docs/discrepancies.md)).

## Target

- **MCU:** STM32F103RBTx, Cortex-M3, 128 KB flash, 20 KB SRAM
- **RTOS:** FreeRTOS (CMSIS-RTOS v2), static allocation, 1 kHz tick
- **Board:** NUCLEO-F103RB / custom VCU wiring — pin map in [`docs/pinmap.md`](docs/pinmap.md), source of truth `Core/bsp/board_config.h`
- **Interfaces:** ADC1/ADC2, CAN1, GPIO, I2C1, SPI2 (SD/FatFs), USART1 (dashboard), USART2 (CLI), IWDG
- **Toolchain:** `arm-none-eabi-gcc`, CMake, OpenOCD / ST-LINK
- **STM32Cube FW_F1** package vendored under `Drivers/` and `Middlewares/`

## Architecture

The firmware is **layered with dependencies pointing downward**, and the
safety-critical logic is **decoupled from hardware so it is unit-testable on a
host**:

```
Core/
  app/       FreeRTOS tasks + wiring (safety_monitor, canbus, rtd, cli, dashboard)
  control/   plausibility, torque map, RTD/precharge state machine   -- hardware-free, host-tested
  safety/    fault manager + enum, brake-throttle latch, CAN watchdog -- hardware-free, host-tested
  proto/     CAN message catalog / codec                              -- hardware-free, host-tested
  drivers/   thin HAL-touching drivers (adc/pot, bse, can, uart, sd, speed)
  bsp/       board_config.h (single pin source of truth), clock, startup
  rtos/      FreeRTOSConfig.h
test/        host unit tests for control/ safety/ proto/
```

- **`control/`, `safety/`, and `proto/` include no STM32/CMSIS/HAL headers** —
  they take plain scalars and return decisions, which is what makes them testable
  without hardware.
- A single **`safety_monitor`** task (highest priority, 5 ms) owns pedal
  acquisition, all plausibility checks, the fault manager, the shutdown-circuit
  output, and the IWDG refresh — see [`docs/tasks.md`](docs/tasks.md).
- One **fault enum + fault manager** is the only thing that decides the system
  response (zero-torque / SDC) — see [`docs/faults.md`](docs/faults.md).

## Build

Put the GNU Arm Embedded toolchain on `PATH` (`arm-none-eabi-gcc`,
`-objcopy`, `-size`), then:

```sh
./scripts/build.sh debug      # or: cmake --preset debug && cmake --build --preset debug
./scripts/build.sh release
./scripts/size.sh release     # memory-usage report
```

Outputs land in `build/<config>/`: `VCU_Firmware.{elf,hex,bin,map}`. The build is
`-Wall -Wextra -Werror` and links well within the 20 KB RAM / 128 KB flash budget.

## Test

The hardware-free layers are covered by host unit tests built with the **native**
compiler (no ARM toolchain needed):

```sh
cmake -S test -B build/test
cmake --build build/test
ctest --test-dir build/test --output-on-failure
```

Tests cover the FSAE-EV safety invariants: APPS 10 % / 100 ms plausibility incl.
open-circuit and idle-gated recovery, the brake-throttle 25 % / 5 % latch,
CAN-timeout -> zero-torque, torque clamp / no-reverse, RTD entry gating + 1-3 s
buzzer, and the pre-charge >= 90 % gate.

CI ([`.github/workflows/ci.yml`](.github/workflows/ci.yml)) runs three jobs on
every push/PR: the **firmware build** (debug + release, `-Werror`, uploads the
`.hex`/`.bin`), the **host tests** (with a gcovr line-coverage gate — currently
~98 % of `control/`+`safety/`+`proto/`, failing under 90 %), and **lint**
(`clang-format` + `clang-tidy`). Format and lint can be run locally:

```sh
./scripts/format.sh check     # or: ./scripts/format.sh fix
clang-tidy Core/control/*.c Core/safety/*.c Core/proto/*.c -- -I Core -std=c11
```

## Flash

With an ST-LINK over SWD:

```sh
./scripts/flash.sh release
```

The script tries `STM32_Programmer_CLI`, then falls back to OpenOCD
(`interface/stlink.cfg` + `target/stm32f1x.cfg`).

## Documentation

- [`docs/pinmap.md`](docs/pinmap.md) — pin map (from `board_config.h`)
- [`docs/tasks.md`](docs/tasks.md) — task table, priorities, interrupts
- [`docs/faults.md`](docs/faults.md) — fault classes and responses
- [`docs/can_catalog.md`](docs/can_catalog.md) — CAN message catalog (placeholder)
- [`docs/discrepancies.md`](docs/discrepancies.md) — every spec/hardware discrepancy and its resolution
- [`docs/architecture.md`](docs/architecture.md), [`docs/hardware.md`](docs/hardware.md), [`docs/bringup.md`](docs/bringup.md)

## Status / known limitations

- **CAN catalog is placeholder.** Message IDs and torque scaling in
  `proto/can_catalog.h` must be replaced with the real inverter/AMS spec before
  flashing. The **CAN-timeout watchdog is wired but disarmed** (`CAN_WATCHDOG_ARMED 0`)
  until the real heartbeat ID is set.
- **Pre-charge gate** decodes a placeholder AMS DC-bus voltage and does not yet
  gate RTD entry.
- **BSPD is hardware**, by rule (§5.3) — the firmware deliberately does not
  implement it; PA7 current-sense is reserved for monitoring/telemetry.
- **FatFs** has no SD/block-device backend yet (disk ops return `RES_NOTRDY`).
- Several deliberate safety behavior changes are logged for race-engineer review
  in `docs/discrepancies.md`.

## CubeMX / CubeIDE

`VCU_Version1.ioc` is kept as the pin-configuration source and can be reopened in
CubeMX. CMake is the build of record; the Eclipse/CubeIDE project state
(`.cproject`/`.project`/`.settings`) is intentionally not tracked.

## License

See [`LICENSE`](LICENSE).
