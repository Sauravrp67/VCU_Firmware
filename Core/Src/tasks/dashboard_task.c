#include <stdio.h>

#include "tasks/dashboard_task.h"
#include "device_drivers/dashboard.h"

#define NEWLINE "\r\n"

void dashboard_task_fn(void *arg);

TaskHandle_t dashboard_task_start(app_data_t *data)
{
   TaskHandle_t handle;
   xTaskCreate(dashboard_task_fn, "Dashboard task", 128, (void *)data, DASH_PRIO, &handle);
   return handle;
}

void dashboard_task_fn(void *arg)
{
    app_data_t *data = (app_data_t *)arg;
    dashboard_t *dash = &data->board.dashboard;
    uint32_t entry;
    HAL_StatusTypeDef ret;

	for(;;)
	{
		entry = osKernelGetTickCount();

		ret = HAL_OK;
		snprintf(dash->line, DASH_LINESZ, "throttle %d" NEWLINE, data->throttle);
		ret |= dashboard_write(dash, dash->line);
		snprintf(dash->line, DASH_LINESZ, "brake %d" NEWLINE, data->brake);
		ret |= dashboard_write(dash, dash->line);
		snprintf(dash->line, DASH_LINESZ, "torque %d" NEWLINE, data->torque_cmd);
		ret |= dashboard_write(dash, dash->line);
		snprintf(dash->line, DASH_LINESZ, "state %d" NEWLINE, (int)data->vcu_state);
		ret |= dashboard_write(dash, dash->line);
		snprintf(dash->line, DASH_LINESZ, "faults 0x%lx" NEWLINE,
		         (unsigned long)data->faults.latched);
		ret |= dashboard_write(dash, dash->line);
		snprintf(dash->line, DASH_LINESZ, "hard_fault %d" NEWLINE,
		         (int)fault_is_hard(&data->faults));
		ret |= dashboard_write(dash, dash->line);
		// Additional metrics can be added after dashboard hardware validation.
		if(ret != HAL_OK)
			fault_set(&data->faults, FAULT_DASHBOARD);
		else
			fault_clear(&data->faults, FAULT_DASHBOARD);

		osDelayUntil(entry + (1000 / DASH_FREQ));
	}
}
