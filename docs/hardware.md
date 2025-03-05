# Hardware Notes

## Target Device

- STM32F103RBTx, LQFP64
- Board profile in CubeMX: NUCLEO-F103RB
- Flash/RAM from linker script: 128 KB flash, 20 KB SRAM
- Debug: SWD on PA13/PA14

## Peripheral Summary

| Peripheral | Purpose in repo | Notes |
| --- | --- | --- |
| ADC1 | Brake pressure / shared analog input | CubeMX regular channel is ADC_CHANNEL_13 on PC3. |
| ADC2 | APPS1 analog input | CubeMX regular channel is ADC_CHANNEL_8 on PB0. |
| CAN1 | Vehicle/network messaging | PB8 RX, PB9 TX. |
| I2C1 | Reserved device bus | PB6 SCL, PB7 SDA. |
| SPI2 | Reserved storage/peripheral bus | PB12 NSS, PB13 SCK, PB14 MISO, PB15 MOSI. |
| USART1 | Dashboard text output in board support | PA9 TX, PA10 RX. |
| USART2 | CLI/debug UART in board support | PA2 TX, PA3 RX. |
| USART3 | CubeMX synchronous USART config | PC10 TX, PC11 RX, PC12 CK. |
| TIM3 | HAL time base | Used instead of SysTick for HAL tick. |

## GPIO Signals

| Signal | Pin | Direction | Firmware use |
| --- | --- | --- | --- |
| AIR_Status | PC0 | Input | Used as TS-active proxy in RTD task. |
| RTD_Input | PC1 | Input | Used as ready-to-drive button/input. |
| SDC_Status_In | PA1 | Input | Present in pinout, not consumed by current app logic. |
| RTD_OUTPUT | PA4 | Output | Used by `set_buzzer()`. |
| LD2 | PA5 | Output | Board LED. |
| Brake_Light | PB2 | Output | Driven from brake percentage threshold. |
| SDC_OP | PB10 | Output | Used by `set_fw()` as shutdown/enable output. |

## Open Hardware Questions

- Confirm final custom VCU connector mapping versus the NUCLEO-F103RB `.ioc` assumptions.
- Assign physical inputs for IMD, BMS, and BSPD faults before using those flags in vehicle logic.
- Decide whether FatFs should be removed or backed by a real SPI storage device.
