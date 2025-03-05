#ifndef __RTD_TASK_H_
#define __RTD_TASK_H_

#include "app/app.h"

#include "cmsis_os.h"

/**
 * @brief Starts the RTD task
 *
 * @param data App data structure pointer
 * @return TaskHandle_t Handle used for task
 */
TaskHandle_t rtd_task_start(app_data_t *data);

#endif
