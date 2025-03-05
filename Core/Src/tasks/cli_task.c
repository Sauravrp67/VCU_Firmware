#include "tasks/cli_task.h"

void cli_task_fn(void *arg);

TaskHandle_t cli_task_start(app_data_t *data)
{
    TaskHandle_t handle;
    xTaskCreate(cli_task_fn, "CLI task", 128, (void *)data, CLI_PRIO, &handle);
    return handle;
}

void cli_task_fn(void *arg)
{
    app_data_t *data = (app_data_t *)arg;

    for(;;)
    {
        fault_clear(&data->faults, FAULT_CLI);
        osDelay(1000 / CLI_FREQ);
    }
}
