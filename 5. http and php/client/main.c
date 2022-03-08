//uart read
//p0_13 TX
//P0_12 RX
 /**
  * @file    main.c
  * @brief   UART communicate with ESP-01 wifi module
  * @details Use Uart
  */

  /***** Includes *****/
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "mxc_device.h"
#include "board.h"
#include "mxc_delay.h"
#include "uart.h"
#include "nvic_table.h"
#include "FreeRTOS.h" /* Must come first. */
#include "FreeRTOSConfig.h"
#include "task.h"     /* RTOS task related API prototypes. */
#include <stdlib.h>
#include "ESP01.h"
#include "HTTP.h"

/***** Definitions *****/
#define UART_BAUD           115200
#define BUFF_SIZE           1024

#if defined (BOARD_EVKIT_V1)
#define uart_0        0
#define uart_1        1
#elif defined (BOARD_FTHR_REVA)
#define READING_UART        2
#define WRITING_UART        3
#else
#error "This example has been written for the MAX78000 Ev Kit or FTHR board."
#endif

/***** Globals *****/

/***** Functions *****/
void UART_Handler(void)
{
	MXC_UART_AsyncHandler(MXC_UART_GET_UART(uart_1));
}

/***** TASKS *****/
// Do a http request every ten second
void httpTask(void* pvParameters) {
	struct HttpRequest* Request = (struct HttpRequest*)pvParameters;
	
	while (1) {
		//this is a test
		strcpy(Request->content, "ID=Device3");
		HTTPsend(Request);
		printf("Done\n");
		vTaskDelay(pdMS_TO_TICKS(10000));

	}
}

/******************************************************************************/
int main(void)
{

	uint32_t error;
	printf("start\n");

	NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(uart_1));
	NVIC_DisableIRQ(MXC_UART_GET_IRQ(uart_1));
	NVIC_SetVector(MXC_UART_GET_IRQ(uart_1), UART_Handler);
	NVIC_EnableIRQ(MXC_UART_GET_IRQ(uart_1));
	MXC_UART_Shutdown(MXC_UART_GET_UART(uart_1));
	// Initialize UART
	if ((error = MXC_UART_Shutdown(MXC_UART_GET_UART(uart_1))) != E_NO_ERROR) {
		printf("-->Error closing UART: %d\n", error);
		printf("-->Example Failed\n");
		while (1) {}
	}
	if ((error = MXC_UART_Init(MXC_UART_GET_UART(uart_1), UART_BAUD, MXC_UART_APB_CLK)) != E_NO_ERROR) {
		printf("-->Error initializing UART: %d\n", error);
		printf("-->Example Failed\n");
		while (1) {}
	}
	printf("-->UART Initialized\n");

	// Clean FIFO
	MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(uart_1));
	MXC_UART_ClearTXFIFO(MXC_UART_GET_UART(uart_1));

	//Initialize ESP-01
	struct ESPController* Controller = CreateESPController(MXC_UART_GET_UART(uart_1), "SSID", "SSID_pwd");
	//connect to AP
	error = ESPAPConnect(Controller);
	if (error != 0) {
		printf("Connect failed: %d\n", error);
	}
	printf("-->AP Conntected\n");
	

	// Create a variable that contains the necessary info to conduct a http request.
	struct HttpRequest* Request = HTTPCreate(Controller,"POST","/MAX.php", "HOSTNAME", "application/x-www-form-urlencoded", "");
	
	error = xTaskCreate(httpTask, "httpTask", 2048, (void*)Request, 1, NULL);
	if (error != 1) {
		printf("Failed to create task: %u\n", error);
		while (1);
	}
	printf("-->Tasks created\n");

	vTaskStartScheduler();

	for (;;); //Should not reach here

}