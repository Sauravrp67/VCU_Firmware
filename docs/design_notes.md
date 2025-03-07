# Design Notes

This document records the current hardware and firmware design decisions that
affect integration, safety behavior, and on-car validation.

## Hardware Contract

- The target is an **STM32F103RBTx** with 128 KB flash and 20 KB SRAM.
- `Core/bsp/board_config.h` is the single source of truth for physical pins,
  ports, peripherals, and ADC channels.
- APPS channel 1 uses PB0 through ADC2; APPS channel 2 uses PC5 through ADC1.
- The brake-pressure sensor uses PC3 through ADC1.
- PA1 is the shutdown-circuit status input. PA7 is reserved for current-sensor
  monitoring and telemetry.
- The RTD driver-action input is a digital signal on PC1.
- USART1 dashboard flow control is disabled because RTS/CTS is not wired.
- The speed sensor uses PC2 as an EXTI2 rising-edge input. PC2 has no timer
  channel alternate function on the STM32F103, so higher-precision capture
  requires a timer-capable pin or a free-running timer time base.

## Torque and CAN

- CAN1 on PB8/PB9 is the torque-command path to the inverter.
- I2C1 on PB6/PB7 is available for auxiliary devices but is not used for torque
  commands.
- The CAN IDs, payload layouts, scaling, byte order, and heartbeat periods in
  `Core/proto/can_catalog.h` are placeholders. Replace them with the production
  inverter and AMS definitions before on-car use.
- The CAN watchdog is wired into the safety path but remains disarmed through
  `CAN_WATCHDOG_ARMED` until the production inverter heartbeat is configured.
- The pre-charge helper expects DC-bus voltage as a percentage of accumulator
  voltage. The current AMS decode is a placeholder and is not yet used to gate
  RTD entry.

## Safety Boundaries

- The safety monitor is the sole writer of the shutdown-circuit output.
- APPS implausibility and hard sensor or communication faults force zero torque
  and open the shutdown circuit.
- The brake-throttle plausibility latch forces zero torque while leaving the
  shutdown circuit closed. It clears only after APPS returns below 5 percent.
- APPS implausibility clears only after both channels agree and the pedal
  returns to idle.
- The rule-mandated BSPD is standalone, non-programmable hardware. Firmware does
  not implement its trip function; PA7 current sensing is monitoring only.
- The RTD buzzer runs non-blockingly for three seconds before entering Drive.
- The independent watchdog is refreshed only by the safety monitor.

## Integration Requirements

- Validate all CAN definitions against the inverter and AMS documentation.
- Confirm the AMS DC-bus signal and scaling before enabling the pre-charge gate.
- Measure task stack high-water marks on hardware.
- Validate fault responses, RTD sequencing, and watchdog behavior with
  hardware-in-the-loop testing before on-car operation.
- FatFs currently has no SD block-device backend; disk operations return
  `RES_NOTRDY`.
