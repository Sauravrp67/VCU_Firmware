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

    float brake_raw;
    uint32_t entry;

	for(;;)
	{
		entry = osKernelGetTickCount();

		stm32f103_adc_switch_channel(bse->handle, bse->channel);
		bse->count = stm32f767_adc_read(bse->handle);
		bse->percent = pressure_sensor_get_percent(bse);

		// T.4.3.3 (2022)

		brake_raw = (bse->percent);
		data->brake = (int)brake_raw;
		set_brakelight((data->brake >= BRAKE_LIGHT_THRESH));

		osDelayUntil(entry + (1000 / BSE_FREQ));
	}
}
