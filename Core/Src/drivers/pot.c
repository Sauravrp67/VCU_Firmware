#include "drivers/pot.h"
#include "drivers/map.h"

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
