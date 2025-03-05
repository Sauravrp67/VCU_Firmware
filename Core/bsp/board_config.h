/**
 * @file    board_config.h
 * @brief   Single source of truth for the VCU hardware pin map (BSP layer).
 *
 * This is the ONLY place physical pins, ports, peripherals, and ADC channels
 * are defined. No magic pin numbers anywhere else in the tree. The map below
 * is transcribed from the as-built design (the on-car wiring proven in
 * `Core/Inc/main.h` + `Core/Src/stm32f1xx_hal_msp.c` + `Core/Src/board.c`),
 * which is authoritative per `docs/discrepancies.md`. Where the Re-Architecture
 * Spec §3 whiteboard map disagreed, the code won and the spec was corrected.
 *
 * Layering: this is a BSP/hardware header and may include the STM32 HAL.
 * `control/`, `safety/`, and `proto/` must NOT include it — they take plain
 * scalar inputs instead.
 *
 * Target: STM32F103RBTx (Cortex-M3) — 128 KB flash, 20 KB RAM. See §7.6.
 */

#ifndef BSP_BOARD_CONFIG_H
#define BSP_BOARD_CONFIG_H

#include "stm32f1xx_hal.h"

/* ===========================================================================
 * Analog inputs (ADC)
 * On STM32F103 the ADC input channel number is the same physical pin for both
 * ADC1 and ADC2 (they share GPIOs); the ADC peripheral that samples each
 * channel is wired in Core/Src/board.c.
 * ===========================================================================*/

/* APPS channel 1 — PB0 = ADCx_IN8 (sampled via ADC2, see board.c) */
#define BSP_APPS1_PORT   GPIOB
#define BSP_APPS1_PIN    GPIO_PIN_0
#define BSP_APPS1_ADC_CH 8U

/* APPS channel 2 — PC5 = ADCx_IN15 (sampled via ADC1, see board.c) */
#define BSP_APPS2_PORT   GPIOC
#define BSP_APPS2_PIN    GPIO_PIN_5
#define BSP_APPS2_ADC_CH 15U

/* Brake System Encoder / brake-pressure sensor (BSE) — PC3 = ADC1_IN13 */
#define BSP_BSE_PORT   GPIOC
#define BSP_BSE_PIN    GPIO_PIN_3
#define BSP_BSE_ADC_CH 13U

/* Current sensor (BSPD-related monitoring/telemetry only) — PA7 = ADC1_IN7.
 * NOTE: the BSPD trip itself is hardware, not firmware (Spec §5.3). */
#define BSP_CURRENT_PORT   GPIOA
#define BSP_CURRENT_PIN    GPIO_PIN_7
#define BSP_CURRENT_ADC_CH 7U

/* ===========================================================================
 * Digital inputs
 * ===========================================================================*/

/* AIR / TSAL status (tractive-system-active gate) — PC0 */
#define BSP_AIR_STATUS_PORT GPIOC
#define BSP_AIR_STATUS_PIN  GPIO_PIN_0

/* Ready-To-Drive driver-action input (digital; §7.4 resolved: PC1, not PC4) */
#define BSP_RTD_INPUT_PORT GPIOC
#define BSP_RTD_INPUT_PIN  GPIO_PIN_1

/* Shutdown-circuit status feedback — PA1 (§7.1 resolved: PA1 = SDC status) */
#define BSP_SDC_STATUS_PORT GPIOA
#define BSP_SDC_STATUS_PIN  GPIO_PIN_1

/* User button (board artifact, Nucleo-F103RB) — PC13 */
#define BSP_B1_PORT      GPIOC
#define BSP_B1_PIN       GPIO_PIN_13
#define BSP_B1_EXTI_IRQn EXTI15_10_IRQn

/* ===========================================================================
 * Digital outputs
 * ===========================================================================*/

/* RTD buzzer — PA4 (sound 1-3 s @ >=80 dBA on RTD entry, Spec §5.5) */
#define BSP_RTD_BUZZER_PORT GPIOA
#define BSP_RTD_BUZZER_PIN  GPIO_PIN_4

/* Brake light (signal-conditioned) — PB2 */
#define BSP_BRAKE_LIGHT_PORT GPIOB
#define BSP_BRAKE_LIGHT_PIN  GPIO_PIN_2

/* Shutdown-circuit (SDC) output — PB10 (participates in AIR drive, Spec §5.6) */
#define BSP_SDC_OUT_PORT GPIOB
#define BSP_SDC_OUT_PIN  GPIO_PIN_10

/* On-board LED (board artifact, Nucleo-F103RB) — PA5 */
#define BSP_LD2_PORT GPIOA
#define BSP_LD2_PIN  GPIO_PIN_5

/* ===========================================================================
 * Buses / peripherals (configured in stm32f1xx_hal_msp.c)
 * ===========================================================================*/

/* CAN1 (inverter + AMS/BMS) — PB8 RX / PB9 TX, remap CAN1_2 (§7.2: CAN is the
 * authoritative torque-command path). */
#define BSP_CAN_RX_PORT GPIOB
#define BSP_CAN_RX_PIN  GPIO_PIN_8
#define BSP_CAN_TX_PORT GPIOB
#define BSP_CAN_TX_PIN  GPIO_PIN_9

/* I2C1 (PB6 SCL / PB7 SDA). Present but NOT used for the torque path; MCP4725
 * DAC is dropped per §7.2. Kept for potential auxiliary use. */
#define BSP_I2C1_SCL_PORT GPIOB
#define BSP_I2C1_SCL_PIN  GPIO_PIN_6
#define BSP_I2C1_SDA_PORT GPIOB
#define BSP_I2C1_SDA_PIN  GPIO_PIN_7

/* Dashboard UART = USART1 (PA9 TX / PA10 RX). PA11/PA12 are CTS/RTS-capable but
 * hardware flow control is NOT wired (§7.5): HwFlowCtl = UART_HWCONTROL_NONE. */
#define BSP_DASH_UART_TX_PORT GPIOA
#define BSP_DASH_UART_TX_PIN  GPIO_PIN_9
#define BSP_DASH_UART_RX_PORT GPIOA
#define BSP_DASH_UART_RX_PIN  GPIO_PIN_10

/* CLI UART = USART2 (PA2 TX / PA3 RX) */
#define BSP_CLI_UART_TX_PORT GPIOA
#define BSP_CLI_UART_TX_PIN  GPIO_PIN_2
#define BSP_CLI_UART_RX_PORT GPIOA
#define BSP_CLI_UART_RX_PIN  GPIO_PIN_3

/* SD card = SPI2 (PB12 NSS / PB13 SCK / PB14 MISO / PB15 MOSI) */
#define BSP_SD_NSS_PORT  GPIOB
#define BSP_SD_NSS_PIN   GPIO_PIN_12
#define BSP_SD_SCK_PORT  GPIOB
#define BSP_SD_SCK_PIN   GPIO_PIN_13
#define BSP_SD_MISO_PORT GPIOB
#define BSP_SD_MISO_PIN  GPIO_PIN_14
#define BSP_SD_MOSI_PORT GPIOB
#define BSP_SD_MOSI_PIN  GPIO_PIN_15

/* Speed sensor — PC2 (§7.3 resolved: hall/frequency pulse -> timer input
 * capture). NOT YET wired in hardware init; reserved here as the canonical pin
 * for the timer-capture driver added in the FreeRTOS-redesign step. */
#define BSP_SPEED_SENSOR_PORT GPIOC
#define BSP_SPEED_SENSOR_PIN  GPIO_PIN_2

/* Debug (SWD): PA13 SWDIO, PA14 SWCLK, PB3 SWO. USART3 (PC10/PC11/PC12) is
 * configured in MSP but currently unused by the application. */

/* ===========================================================================
 * Legacy aliases
 * The existing CubeMX-generated names map onto the canonical BSP_* map above so
 * pre-refactor code keeps compiling. New code should use the BSP_* names; these
 * aliases are removed as call sites migrate.
 * ===========================================================================*/

#define B1_Pin                        BSP_B1_PIN
#define B1_GPIO_Port                  BSP_B1_PORT
#define B1_EXTI_IRQn                  BSP_B1_EXTI_IRQn
#define AIR_Status_Pin                BSP_AIR_STATUS_PIN
#define AIR_Status_GPIO_Port          BSP_AIR_STATUS_PORT
#define RTD_Input_Pin                 BSP_RTD_INPUT_PIN
#define RTD_Input_GPIO_Port           BSP_RTD_INPUT_PORT
#define BrakePressureSensor_Pin       BSP_BSE_PIN
#define BrakePressureSensor_GPIO_Port BSP_BSE_PORT
#define SDC_Status_In_Pin             BSP_SDC_STATUS_PIN
#define SDC_Status_In_GPIO_Port       BSP_SDC_STATUS_PORT
#define USART_TX_Pin                  BSP_CLI_UART_TX_PIN
#define USART_TX_GPIO_Port            BSP_CLI_UART_TX_PORT
#define USART_RX_Pin                  BSP_CLI_UART_RX_PIN
#define USART_RX_GPIO_Port            BSP_CLI_UART_RX_PORT
#define RTD_OUTPUT_Pin                BSP_RTD_BUZZER_PIN
#define RTD_OUTPUT_GPIO_Port          BSP_RTD_BUZZER_PORT
#define LD2_Pin                       BSP_LD2_PIN
#define LD2_GPIO_Port                 BSP_LD2_PORT
#define CurrentSensor_Pin             BSP_CURRENT_PIN
#define CurrentSensor_GPIO_Port       BSP_CURRENT_PORT
#define APPS2_Pin                     BSP_APPS2_PIN
#define APPS2_GPIO_Port               BSP_APPS2_PORT
#define APPS1_Pin                     BSP_APPS1_PIN
#define APPS1_GPIO_Port               BSP_APPS1_PORT
#define Brake_Light_Pin               BSP_BRAKE_LIGHT_PIN
#define Brake_Light_GPIO_Port         BSP_BRAKE_LIGHT_PORT
#define SDC_OP_Pin                    BSP_SDC_OUT_PIN
#define SDC_OP_GPIO_Port              BSP_SDC_OUT_PORT
#define TMS_Pin                       GPIO_PIN_13
#define TMS_GPIO_Port                 GPIOA
#define TCK_Pin                       GPIO_PIN_14
#define TCK_GPIO_Port                 GPIOA
#define SWO_Pin                       GPIO_PIN_3
#define SWO_GPIO_Port                 GPIOB

#endif /* BSP_BOARD_CONFIG_H */
