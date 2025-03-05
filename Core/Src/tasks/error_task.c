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

		// Central fault manager: this is the ONLY task that drives the SDC.
		// §5.6: any hard fault (APPS implausibility, BSE open-circuit, CAN
		// timeout) opens the shutdown circuit; the BPPC latch and soft faults
		// do not. Torque is held at zero whenever any torque-inhibiting fault
		// (hard fault or BPPC latch) is active.
		set_fw(!fault_is_hard(&data->faults));

		if(fault_torque_inhibited(&data->faults))
		{
			data->torque_cmd = 0;
		}

        osDelayUntil(entry + (1000 / ERR_FREQ));
    }
}
