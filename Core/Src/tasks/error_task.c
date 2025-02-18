#include "main.h"
#include "tasks/error_task.h"

/**
 * @brief Actual ERROR task function
 *
 * @param arg App_data struct pointer converted to void pointer
 */
void error_task_fn(void *arg);

TaskHandle_t error_task_start(app_data_t *data)
{
    TaskHandle_t handle;
    xTaskCreate(error_task_fn, "ERROR task", 128, (void *)data, ERR_PRIO, &handle);
    return handle;
}

void error_task_fn(void *arg)
{
	app_data_t *data = (app_data_t *)arg;

    uint32_t entry;

    for(;;)
    {
        entry = osKernelGetTickCount();

		data->imd_fail = HAL_GPIO_ReadPin(IMD_Fail_GPIO_Port, IMD_Fail_Pin);
		data->bms_fail = HAL_GPIO_ReadPin(BMS_Fail_GPIO_Port, BMS_Fail_Pin);
		data->bspd_fail = HAL_GPIO_ReadPin(BSPD_Fail_GPIO_Port, BSPD_Fail_Pin);



		data->hard_fault = (data->apps_fault ||
				            data->bse_fault);

        data->soft_fault =(data->bppc_fault ||
        				   data->cli_fault ||
						   data->canbus_fault ||
						   data->dashboard_fault
						   );

		set_cascadia_enable(!data->hard_fault);

        osDelayUntil(entry + (1000 / ERR_FREQ));
    }
}
