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
uint8_t ReceiveString[BUFF_SIZE];


/***** Functions *****/
void UART_Handler(void)
{
	MXC_UART_AsyncHandler(MXC_UART_GET_UART(uart_1));
}


/***** Tasks *****/
void vTask1(void* pvParameters) //receive task
{
	int vTask1error;

	vTask1error = ESP_Send(MXC_UART_GET_UART(uart_1), "AT+CWMODE=1\r\n"); // Set to client mode
	if (vTask1error != E_NO_ERROR) {
		printf("-->Error with UART_WriteAsync callback; %d\n", vTask1error);
	}
	memset(ReceiveString, 0x0, BUFF_SIZE);
	vTask1error = ESP_Read(MXC_UART_GET_UART(uart_1), ReceiveString);
	if (vTask1error != E_NO_ERROR) {
		printf("-->Error with UART_WriteAsync callback; %d\n", vTask1error);
	}
	printf("%s", ReceiveString);


	vTask1error = ESP_Send(MXC_UART_GET_UART(uart_1), "AT+CWJAP=\"SSID\",\"PWD\"\r\n"); // Connect to AP ,"AT+CWJAP\"SSID\",\"PWD\"
	if (vTask1error != E_NO_ERROR) {
		printf("-->Error with UART_WriteAsync callback; %d\n", vTask1error);
	}
	memset(ReceiveString, 0x0, BUFF_SIZE);
	vTask1error = ESP_Read(MXC_UART_GET_UART(uart_1), ReceiveString);
	if (vTask1error != E_NO_ERROR) {
		printf("-->Error with UART_WriteAsync callback; %d\n", vTask1error);
	}
	printf("%s", ReceiveString);

	vTask1error = ESP_Send(MXC_UART_GET_UART(uart_1), "AT+CIPSTART=\"TCP\",\"IP\",PORT\r\n"); // Connect to TCP ,"AT+CIPSTART\"TCP or UDP\",\"IP\",port"
	if (vTask1error != E_NO_ERROR) {
		printf("-->Error with UART_WriteAsync callback; %d\n", vTask1error);
	}
	memset(ReceiveString, 0x0, BUFF_SIZE);
	vTask1error = ESP_Read(MXC_UART_GET_UART(uart_1), ReceiveString);
	if (vTask1error != E_NO_ERROR) {
		printf("-->Error with UART_WriteAsync callback; %d\n", vTask1error);
	}
	printf("%s", ReceiveString);



	printf("complete\n");
	while (1);

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

	// Initialize memory
	memset(ReceiveString, 0x0, BUFF_SIZE);

	MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(uart_1));
	MXC_UART_ClearTXFIFO(MXC_UART_GET_UART(uart_1));
	// Create Tasks
	uint32_t Taskerror = xTaskCreate(vTask1, "Task 1", 250, NULL, 1, NULL);
	printf("error: %u\n", Taskerror);
	
	printf("Task creation 1. pass, 2. fail\n");

	vTaskStartScheduler();

	for (;;);// Should not reach here

}