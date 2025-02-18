#ifndef __BPPC_TASK_H_
#define __BPPC_TASK_H_

#include "app.h"

#include "cmsis_os.h"

/**
* @brief Starts the BPPC task
*
* @param data App data structure pointer
* @return TaskHandle_t Handle used for task
*/
TaskHandle_t bppc_task_start(app_data_t *data);

#endif
