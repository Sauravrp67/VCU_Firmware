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

typedef struct
{
	uint16_t min;
	uint16_t max;

	uint16_t count;
	float percent;
	float hist[HISTORYSIZE];

	ADC_HandleTypeDef *handle;
} pot_t;

void pot_init(pot_t *pot, uint16_t min, uint16_t max, ADC_HandleTypeDef *handle);
float pot_get_percent(pot_t *raw);
/* APPS plausibility moved to the hardware-free control/apps module (host-tested). */

#endif
