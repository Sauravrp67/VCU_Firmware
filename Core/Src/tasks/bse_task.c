#include "tasks/bse_task.h"
#include "main.h"

/**
* @brief Actual BSE task function
*
* @param arg App_data struct pointer converted to void pointer
*/
void bse_task_fn(void *arg);

TaskHandle_t bse_task_start(app_data_t *data)
{
   TaskHandle_t handle;
   xTaskCreate(bse_task_fn, "BSE task", 128, (void *)data, BSE_PRIO, &handle);
   return handle;
}

void bse_task_fn(void *arg)
{
    app_data_t *data = (app_data_t *)arg;
    pressure_sensor_t *bse = &data->board.bse;

    uint32_t entry;

	for(;;)
	{
		entry = osKernelGetTickCount();

		stm32f103_adc_switch_channel(bse->handle, bse->channel);
		bse->count = stm32f103_adc_read(bse->handle);
		bse->percent = pressure_sensor_get_percent(bse);

		// §5.6: brake-sensor open/short circuit is a safety fault (zero-torque).
		if(bse->count < BSE_RAW_OPEN_LO || bse->count > BSE_RAW_OPEN_HI)
		{
			fault_set(&data->faults, FAULT_BSE);
		}
		else
		{
			fault_clear(&data->faults, FAULT_BSE);
		}

		// T.4.3.3 (2022)
		data->brake = (int)bse->percent;
		set_brakelight((data->brake >= BRAKE_LIGHT_THRESH));

		osDelayUntil(entry + (1000 / BSE_FREQ));
	}
}
