#ifndef __task_os_H
#define __task_os_H

#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

void DefaultTask(void const * argument);
void Task02(void const * argument);
void Task03(void const * argument);
void Task04(void const * argument);
void Task05(void const * argument);
void Task06(void const * argument);

#endif // __task_os_H
