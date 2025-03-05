#include "tasks/bppc_task.h"
#include "main.h"

/**
* @brief Actual BPPC task function
*
* @param arg App_data struct pointer converted to void pointer
*/
void bppc_task_fn(void *arg);

TaskHandle_t bppc_task_start(app_data_t *data)
{
   TaskHandle_t handle;
   xTaskCreate(bppc_task_fn, "BPPC task", 128, (void *)data, BPPC_PRIO, &handle);
   return handle;
}

void bppc_task_fn(void *arg)
{
    app_data_t *data = (app_data_t *)arg;

    uint32_t entry;

	for(;;)
	{
		entry = osKernelGetTickCount();

		// §5.2 (EV.4.7): latch a torque cut when brake > 10% AND throttle > 25%;
		// clears only when throttle < 5%. The fault manager (error_task) is the
		// single owner of the SDC/torque response — this task no longer writes
		// the shutdown circuit directly (removes a 3-way write race), and the
		// latch now cuts torque rather than killing the tractive system, which
		// is sufficient per §5.2.
		if(bppc_update(&data->bppc_state, (float)data->brake, (float)data->throttle))
		{
			fault_clear(&data->faults, FAULT_BPPC);
		}
		else
		{
			fault_set(&data->faults, FAULT_BPPC);
		}

		osDelayUntil(entry + (1000 / BPPC_FREQ));
	}
}
