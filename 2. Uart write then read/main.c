//uart read
//p0_13 TX
//P0_12 RX
 /**
  * @file    main.c
  * @brief   Uart Write then read
  * @details Reads from UART1
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
#include "task.h"     /* RTOS task related API prototypes. */
#include <stdlib.h>

/***** Definitions *****/
#define UART_BAUD           115200
#define BUFF_SIZE           1024

#if defined (BOARD_EVKIT_V1)
#define uart_0        1
#define uart_1        2
#elif defined (BOARD_FTHR_REVA)
#define READING_UART        2
#define WRITING_UART        3
#else
#error "This example has been written for the MAX78000 Ev Kit or FTHR board."
#endif

/***** Globals *****/
volatile int READ_FLAG;
volatile int WRITE_FLAG;
uint8_t RxData[BUFF_SIZE];
uint8_t ReceiveString[255];
uint8_t Current[255];

/***** Functions *****/
void UART_Handler(void)
{
	MXC_UART_AsyncHandler(MXC_UART_GET_UART(uart_0));
}

void writeCallback(mxc_uart_req_t* req, int error)
{
	WRITE_FLAG = error;
}


/***** Tasks *****/
void vTask1(void* pvParameters) //receive task
{
	volatile int vTask1error;
	while (1) {

		/***** write part *****/
		char s[50] = "123456789\n";
		mxc_uart_req_t write_req;
		write_req.uart = MXC_UART_GET_UART(uart_0);
		write_req.txData = s;
		write_req.txLen = strlen(s);
		write_req.rxLen = 0;
		write_req.callback = writeCallback;
		WRITE_FLAG = 1;
		
		vTask1error = MXC_UART_TransactionAsync(&write_req);
		MXC_UART_AsyncHandler(MXC_UART_GET_UART(uart_0));
		if (vTask1error != E_NO_ERROR) {
			printf("-->Error starting async write: %d\n", vTask1error);
			printf("-->Example Failed\n");
			while (1) {}
		}
		while (WRITE_FLAG) {
			//printf("flag stop\n");
			vTaskDelay(pdMS_TO_TICKS(20));
		};
		if (WRITE_FLAG != E_NO_ERROR) {
			printf("-->Error with UART_writeAsync callback; %d\n", WRITE_FLAG);
		}
		MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(uart_0));

		/***** read part *****/
		// will only work if incoming is nl or cl nl
		while (1) {
			vTask1error = MXC_UART_GetRXFIFOAvailable(MXC_UART_GET_UART(uart_0));// read frome buffer if available
			if (vTask1error > 0) {
				vTask1error = MXC_UART_ReadRXFIFO(MXC_UART_GET_UART(uart_0), RxData, vTask1error);
				strcat(ReceiveString, RxData);
				volatile int length = strlen(ReceiveString);
				if (ReceiveString[length - 1] == '\n') {
					ReceiveString[length - 1] = '\0'; // remove nl
					if (ReceiveString[length - 2] == '\r')ReceiveString[length - 2] = '\0'; // remove cr
					strcpy(Current, ReceiveString);
					printf("%s\n", Current);
					/*printf("%s\n%d\n", Current, strlen(Current));*/ // print received and length
					MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(uart_0));
					break;
					
				}
			}
		}
		strcpy(ReceiveString, 0);
		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}


void vTask2(void* pvParameters)
{

	uint32_t error;
	for (;; )
	{
		//printf("Task 2.\n");
		vTaskDelay(pdMS_TO_TICKS(100000));
	}
}

/******************************************************************************/
int main(void)
{
	uint32_t error;
	printf("start\n");
	NVIC_ClearPendingIRQ(MXC_UART_GET_IRQ(uart_0));
	NVIC_DisableIRQ(MXC_UART_GET_IRQ(uart_0));
	NVIC_SetVector(MXC_UART_GET_IRQ(uart_0), UART_Handler);
	NVIC_EnableIRQ(MXC_UART_GET_IRQ(uart_0));

	// Initialize UART
	if ((error = MXC_UART_Init(MXC_UART_GET_UART(uart_0), UART_BAUD, MXC_UART_APB_CLK)) != E_NO_ERROR) {
		printf("-->Error initializing UART: %d\n", error);
		printf("-->Example Failed\n");
		while (1) {}
	}
	printf("-->UART Initialized\n");

	// Initialize memory
	memset(RxData, 0x0, BUFF_SIZE);
	memset(ReceiveString, 0x0, 255);
	memset(Current, 0x0, 255);
	MXC_UART_ClearRXFIFO(MXC_UART_GET_UART(uart_0));

	// Create Tasks
	uint32_t Taskerror = xTaskCreate(vTask1, "Task 1", 250, NULL, 1, NULL);
	printf("error: %u\n", Taskerror);
	Taskerror = xTaskCreate(vTask2, "Task 2", 250, NULL, 1, NULL);
	printf("error: %u\n", Taskerror);
	printf("Task creation 1. pass, 2. fail\n");

	vTaskStartScheduler();

	for (;; );// Should not reach hear

}