/**
* @file pressure_sense.h
* @author Saurav Raj Paudel
* @brief
* @version 0.1
* @date 2025-01-10
*
* @copyright Copyright (c) 2025
*
*/

#ifndef __PRESSURE_SENSE_H_
#define __PRESSURE_SENSE_H_

#include <stdint.h>

#include "stm32f1xx_hal.h"

//Encapsulating the pressure sensor data within this datastructure
typedef struct {
	uint16_t min;
	uint16_t max;

	uint16_t count;
	float percent;

	ADC_HandleTypeDef *handle;
	uint8_t channel;
} pressure_sensor_t;

//Function Prototypes

//Initializes the pressure Sensor that is sets the variables of this structure
void pressure_sensor_init(pressure_sensor_t *sensor,uint16_t min, uint16_t max, ADC_HandleTypeDef *handle, uint8_t channel);

//Converts the values received from the adc to percentage of the brake applied
float pressure_sensor_get_percent(pressure_sensor_t *root);

#endif
