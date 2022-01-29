/*******************************************************************************
 * Copyright (C) 2016 Maxim Integrated Products, Inc., All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL MAXIM INTEGRATED BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of Maxim Integrated
 * Products, Inc. shall not be used except as stated in the Maxim Integrated
 * Products, Inc. Branding Policy.
 *
 * The mere transfer of this software does not imply any licenses
 * of trade secrets, proprietary technology, copyrights, patents,
 * trademarks, maskwork rights, or any other form of intellectual
 * property whatsoever. Maxim Integrated Products, Inc. retains all
 * ownership rights.
 *
 * $Date: 2018-09-05 16:46:11 -0500 (Wed, 05 Sep 2018) $
 * $Revision: 37695 $
 *
 ******************************************************************************/

 /**
  * @file    main.c
  * @brief   FreeRTOS
  * @details This example demonstrates the tasks with the same priority.
  */

  /***** Includes *****/
#include <stdio.h>
#include "mxc_device.h"
#include "mxc_delay.h"
#include "FreeRTOS.h" /* Must come first. */
#include "task.h"     /* RTOS task related API prototypes. */

/***** Definitions *****/
/* Used as a loop counter to create a very crude delay. */
#define mainDELAY_LOOP_COUNT   ( 0xfffff )



/***** Functions *****/

/***** Tasks *****/
void vTask1(void* pvParameters)
{
	const char* pcTaskName = "Task 1 is running\r\n";
	volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */
	 /* As per most tasks, this task is implemented in an infinite loop. */
	for (;; )
	{
		/* Print out the name of this task. */
		printf("%s", pcTaskName);
		/* Delay for a period. */
		MXC_Delay(10000);
	}
}

void vTask2(void* pvParameters)
{
	const char* pcTaskName = "Task 2 is running\r\n";
	volatile uint32_t ul; /* volatile to ensure ul is not optimized away. */
	 /* As per most tasks, this task is implemented in an infinite loop. */
	for (;; )
	{
		/* Print out the name of this task. */
		printf("%s",pcTaskName);
		/* Delay for a period. */
		MXC_Delay(10000);
	}
}
/******************************************************************************/
int main(void)
{
	/* Create one of the two tasks. Note that a real application should check
the return value of the xTaskCreate() call to ensure the task was created
successfully. */
		uint32_t error = xTaskCreate(vTask1, /* Pointer to the function that implements the task. */
		"Task 1",/* Text name for the task. This is to facilitate
		debugging only. */
		250, /* Stack depth - small microcontrollers will use much
		less stack than this. */
		NULL, /* This example does not use the task parameter. */
		1, /* This task will run at priority 1. */
		NULL); /* This example does not use the task handle. */
		/* Create the other task in exactly the same way and at the same priority. */
		printf("error: %ud", error);
		error = xTaskCreate(vTask2, "Task 2", 250, NULL, 1, NULL);
		printf("error: %ud", error);
	/* Start the scheduler so the tasks start executing. */
	vTaskStartScheduler();

	/* If all is well then main() will never reach here as the scheduler will
	now be running the tasks. If main() does reach here then it is likely that
	there was insufficient heap memory available for the idle task to be created.
	Chapter 2 provides more information on heap memory management. */
	for (;; );

}
