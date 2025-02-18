/**
* @file pot.h
* @author Saurav Raj Paudel
* @brief
* @version 0.1
* @date 2025-01-10
*
* @copyright Copyright (c) 2025
*
*/

#ifndef __POT_H_
#define __POT_H_

#include <stdint.h>
#include "stm32f1xx_hal.h"

#define HISTORYSIZE 50

typedef struct {
	uint16_t min;
	uint16_t max;

	uint16_t count;
	float percent;
	float hist[HISTORYSIZE];

	ADC_HandleTypeDef *handle;
} pot_t;

void pot_init(pot_t *pot,uint16_t min,uint16_t max,ADC_HandleTypeDef *handle);
float pot_get_percent(pot_t *raw);
uint16_t pot_percent_to_hex(float percent);
uint8_t pot_check_plausibility(float L,float R,int thresh, int count);

#endif
