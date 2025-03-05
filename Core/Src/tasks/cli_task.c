#include "tasks/cli_task.h"

void cli_task_fn(void *arg);

#define CLI_STACK_WORDS 128
TaskHandle_t cli_task_start(app_data_t *data)
{
    static StackType_t stack[CLI_STACK_WORDS];
    static StaticTask_t tcb;
    return xTaskCreateStatic(cli_task_fn, "CLI task", CLI_STACK_WORDS,
                             (void *)data, CLI_PRIO, stack, &tcb);
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
