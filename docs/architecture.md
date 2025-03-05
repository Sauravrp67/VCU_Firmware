# Firmware Architecture

## Layers

- CubeMX/HAL layer: `Core/Src/main.c`, MSP, interrupt handlers, startup, linker script, HAL/CMSIS drivers.
- Board support: `board_t` aggregates MCU handles and logical devices such as APPS, brake pressure, CAN, CLI, and dashboard.
- Device drivers: thin wrappers for CAN, CLI UART, dashboard UART, ADC-backed potentiometers, and pressure sensor conversion.
- Application state: `app_data_t` owns shared VCU state and FreeRTOS task handles.
- Tasks: APPS, BSE, BPPC, RTD, error aggregation, CAN, CLI, and dashboard loops.

## Startup Flow

1. `main()` initializes HAL, clocks, GPIO, ADC, CAN, FatFs, I2C, SPI, and UART peripherals.
2. `osKernelInitialize()` starts RTOS object creation.
3. CubeMX creates the idle/default task.
4. `app_create()` initializes board support and creates application tasks.
5. `osKernelStart()` transfers execution to FreeRTOS.

## Task Responsibilities

| Task | Responsibility |
| --- | --- |
| APPS | Reads accelerator sensor percentages and flags plausibility faults. |
| BSE | Reads brake pressure and drives brake light state. |
| BPPC | Applies brake/throttle plausibility behavior. |
| RTD | Handles ready-to-drive transition and RTD output. |
| Error | Aggregates hard/soft faults and drives firmware/shutdown output. |
| CAN | Sends APPS-triggered CAN packets using the HAL CAN driver. |
| CLI | Placeholder task; CLI parser/commands are not yet implemented. |
| Dashboard | Periodically emits selected app state over UART. |

## Generated-Code Boundary

Keep CubeMX-regenerated code inside `USER CODE` sections. Put durable application logic in `Core/app/`, `Core/bsp/`, `Core/control/`, `Core/safety/`, `Core/proto/`, and `Core/drivers/`.

The CMake build intentionally compiles the same sources as the CubeIDE project so unresolved stale symbols fail during normal CLI builds.
