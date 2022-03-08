 /**
  * @file    main.c
  * @brief   TCP or UDP with ESP01
  * @details Use Uart1 to communicate with ESP01 for Internet capability
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
// Creates data to write to ESP01-S
void ProviderTask(void* pvParameters) {
	struct ESPController* Controller = (struct ESPController*)pvParameters;
	uint8_t  counter = 0; 
	char  temp[8];
	while(1){
		
		while (counter < 255) {
			memset(temp, 0x0, 8);
			sprintf(temp, "%d", counter);
			// This parts tells the ClientTask to send data in the W_SOCKString
			strcpy(Controller->W_SOCKString, temp);
			Controller->SocketSend = 1;

			vTaskDelay(pdMS_TO_TICKS(100));
			counter++;
		}
		counter = 0;
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
	struct ESPController* Controller = CreateESPController(MXC_UART_GET_UART(uart_1),"SSID","SSID_PWD","TCP or UDP","IP","Port");
	error = ESPConnect(Controller);
	if (error != 0) {
		printf("Connect failed: %d\n", error);
	}
	printf("-->ESP-01 Initialized\n");
	// Create Tasks
	error = xTaskCreate(ClientTask, "ESPSOCKTASK", 2048, (void*)Controller, 1, NULL);
	if (error != 1) {
		printf("Failed to create task: %u\n", error);
		while (1);
	}
	error = xTaskCreate(ProviderTask, "ProviderTask", 1024, (void*)Controller, 1, NULL);
	if (error != 1) {
		printf("Failed to create task: %u\n", error);
		while (1);
	}
	printf("-->Tasks created\n");

	vTaskStartScheduler();

	for (;;); //Should not reach here

}