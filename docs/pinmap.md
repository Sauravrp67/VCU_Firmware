# VCU Pin Map

Source of truth: `Core/bsp/board_config.h`. Target MCU: **STM32F103RBTx**
(Cortex-M3, 128 KB flash, 20 KB RAM). This table is the as-built map; see
`discrepancies.md` for where it differs from the original spec §3.

## Analog inputs (ADC)

| Function | Pin | Channel | ADC peripheral | BSP macro |
|---|---|---|---|---|
| APPS channel 1 | PB0 | IN8 | ADC2 | `BSP_APPS1_*` |
| APPS channel 2 | PC5 | IN15 | ADC1 | `BSP_APPS2_*` |
| Brake sensor (BSE) | PC3 | IN13 | ADC1 | `BSP_BSE_*` |
| Current sensor (monitoring) | PA7 | IN7 | ADC1 | `BSP_CURRENT_*` |

## Digital inputs

| Function | Pin | Notes | BSP macro |
|---|---|---|---|
| AIR / TSAL status | PC0 | tractive-system-active gate | `BSP_AIR_STATUS_*` |
| RTD driver action | PC1 | digital (spec §3 said PC4) | `BSP_RTD_INPUT_*` |
| SDC status feedback | PA1 | spec §3 said current sense | `BSP_SDC_STATUS_*` |
| User button (board artifact) | PC13 | Nucleo-F103RB | `BSP_B1_*` |
| Speed sensor | PC2 | reserved; timer input-capture (Step 7) | `BSP_SPEED_SENSOR_*` |

## Digital outputs

| Function | Pin | Mode | BSP macro |
|---|---|---|---|
| RTD buzzer | PA4 | GPIO push-pull | `BSP_RTD_BUZZER_*` |
| Brake light | PB2 | GPIO push-pull | `BSP_BRAKE_LIGHT_*` |
| Shutdown-circuit (SDC) output | PB10 | GPIO push-pull | `BSP_SDC_OUT_*` |
| On-board LED (board artifact) | PA5 | GPIO push-pull | `BSP_LD2_*` |

## Buses

| Bus | Pins | Notes | BSP macro |
|---|---|---|---|
| CAN1 | PB8 RX / PB9 TX | remap CAN1_2 (inverter + AMS) | `BSP_CAN_*` |
| I2C1 | PB6 SCL / PB7 SDA | present, NOT on torque path (§7.2) | `BSP_I2C1_*` |
| USART1 (dashboard) | PA9 TX / PA10 RX | no HW flow control (§7.5) | `BSP_DASH_UART_*` |
| USART2 (CLI) | PA2 TX / PA3 RX | — | `BSP_CLI_UART_*` |
| SPI2 (SD card) | PB12 NSS / PB13 SCK / PB14 MISO / PB15 MOSI | FatFs | `BSP_SD_*` |
| SWD (debug) | PA13 SWDIO / PA14 SWCLK / PB3 SWO | — | — |
