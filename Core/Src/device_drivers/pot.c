#include <math.h>

#include "device_drivers/pot.h"
#include "device_drivers/map.h"

void pot_init(pot_t *poten, uint16_t min, uint16_t max, ADC_HandleTypeDef *handle)
{
	poten->min = min;
	poten->max = max;
	poten->handle = handle;
	for(int i = 0; i < HISTORYSIZE; i++) poten->hist[i] = 0;
}

float pot_get_percent(pot_t *root) {
	float percent = 0;
	float raw = (float)map(root->count, root->min, root->max, 0.0, 100.0);
	if(raw > 100.0) raw = 100.0;
	else if(raw < 0.0) raw = 0.0;
	for(int i = 0; i < HISTORYSIZE - 1; i ++)
	{
		root->hist[HISTORYSIZE - i - 1] = root->hist[HISTORYSIZE - i - 2];
		percent += root->hist[HISTORYSIZE - i - 1];
	}
	root->hist[0] = raw;
	percent += raw;
	percent /= (float)HISTORYSIZE;
	return percent;
}

uint16_t pot_percent_to_hex(float percent)
{
    if (percent > 100) percent = 100.0;
    if (percent < 0) percent = 0.0;
    return (uint16_t)percent * 0x5555;
}

uint8_t pot_check_plausibility(float L, float R, int thresh, int count)
{
    static unsigned int counts = 0;

	// Check if APPS1 and APPS2 are more than 10% different
	if(fabs(L - R) > thresh)
	{
		counts++;

		// If there are consecutive errors for more than 100ms, error
		return counts <= count;
	}
	else
	{
		// If potentiometers are within spec, reset count
		counts = 0;
		return 1;
	}
}
