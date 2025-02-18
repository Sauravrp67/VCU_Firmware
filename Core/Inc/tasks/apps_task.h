/**
* @file apps_task.h
* @author Saurav Raj Paudel
* @brief
* @version 0.1
* @date 2025-01-10
*
* @copyright Copyright (c) 2025
*
*/

#ifndef __APPS_TASK_H_
#define __APPS_TASK_H_

#include "app.h"

#include "cmsis_os.h"

/**
 * @brief Starts the APPS task
 *
 * @param data App data structure pointer
 * @return TaskHandle_t Handle used for task
 */
TaskHandle_t apps_task_start(app_data_t *data);

#endif
