# Firmware Architecture

## Layers

- CubeMX/HAL layer: `Core/Src/main.c`, MSP, interrupt handlers, startup, linker script, HAL/CMSIS drivers.
- Board support: `board_t` aggregates MCU handles and logical devices such as APPS, brake pressure, CAN, CLI, and dashboard.
- Device drivers: thin wrappers for CAN, CLI UART, dashboard UART, ADC-backed potentiometers, and pressure sensor conversion.
- Application state: `app_data_t` owns shared VCU state and FreeRTOS task handles.
- Tasks: safety monitoring, RTD sequencing, CAN, CLI, and dashboard loops.

## Startup Flow

1. `main()` initializes HAL, clocks, GPIO, ADC, CAN, FatFs, I2C, SPI, and UART peripherals.
2. `osKernelInitialize()` starts RTOS object creation.
3. CubeMX creates the idle/default task.
4. `app_create()` initializes board support and creates application tasks.
5. `osKernelStart()` transfers execution to FreeRTOS.

## Task Responsibilities

| Task | Responsibility |
| --- | --- |
| `safety_monitor` | Acquires APPS and brake sensors, evaluates plausibility and faults, controls the brake light and SDC, requests torque transmission, and refreshes the IWDG. |
| `rtd_task` | Runs RTD sequencing and controls the non-blocking buzzer. |
| `canbus_task` | Sends fault-gated torque commands using the HAL CAN driver. |
| `cli_task` | Hosts the serial CLI. |
| `dashboard_task` | Emits VCU state over UART and updates wheel speed. |

## Generated-Code Boundary

Keep CubeMX-regenerated code inside `USER CODE` sections. Put durable application logic in `Core/app/`, `Core/bsp/`, `Core/control/`, `Core/safety/`, `Core/proto/`, and `Core/drivers/`.

The CMake build intentionally compiles the same sources as the CubeIDE project so unresolved stale symbols fail during normal CLI builds.
