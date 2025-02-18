/**
* @file current_sensor.h
* @author Saurav Raj Paudel
* @brief
* @version 0.1
* @date 2025-01-10
*
* @copyright Copyright (c) 2025
*
*/

#ifndef __CURRENT_SENSOR_H_
#define __CURRENT_SENSOR_H_

#include <stdint.h>

#include "stm32f1xx_hal.h"

typedef struct {
	uint16_t min;
	uint16_t max;

	uint16_t count;
	float Amps;

	ADC_HandleTypeDef *handle;
	uint8_t channel;
} current_sensor_t;

void current_sensor_init(current_sensor_t *current,uint16_t min, uint16_t max,ADC_HandleTypeDef *handle,uint8_t channel);
float Amps_conversion(current_sensor_t *sensor);

#endif
