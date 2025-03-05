/**
 *@file pressure_sense.c
 *@author Saurav Raj Paudel
 *@brief
 *@version 0.1
 *@date 2025-01-10
 *
 *@copyright Copyright (c) 2025
 */

#include "drivers/pressure_sense.h"
#include "drivers/map.h"
#include <math.h>

void pressure_sensor_init(pressure_sensor_t *sensor, uint16_t min, uint16_t max,
                          ADC_HandleTypeDef *handle, uint8_t channel)
{
	sensor->min = min;
	sensor->max = max;
	sensor->handle = handle;
	sensor->channel = channel;
}

float pressure_sensor_get_percent(pressure_sensor_t *root)
{
	float percent = (float)map(root->count, root->min, root->max, 0, 100);

	if (percent > 100.0)
	{
		return 100.0;
	}
	else if (percent < 0.0)
	{
		return 0.0;
	}
	else
	{
		return percent;
	}
}
