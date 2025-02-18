/**
* @file stm32f103.h
* @author Saurav Raj Paudel
* @brief
* @version 0.1
* @date 2025-01-10
*
* @copyright Copyright (c) 2025
*
*/

#ifndef __STM32F103_H_
#define __STM32F103_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

//This structure provides interface with HAL Library
typedef struct {
	ADC_HandleTypeDef hadc1;
	ADC_HandleTypeDef hadc2;

	CAN_HandleTypeDef hcan1;

	I2C_HandleTypeDef hi2c1;

	SPI_HandleTypeDef hspi2;

	UART_HandleTypeDef huart1;
	UART_HandleTypeDef huart3;

	osMutexId_t can1_mutex;
	osMutexId_t i2c1_mutex;
	osMutexId_t spi2_mutex;
	osMutexId_t	uart1_mutex;
	osMutexId_t	uart3_mutex;
} stm32f103_t;


void stm32f103_init(stm32f103_t *dev);
uint16_t stm32f103_adc_read(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef stm32f103_adc_switch_channel(ADC_HandleTypeDef *hadc, uint32_t channel);

#endif
