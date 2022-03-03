#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "ESP01.h"
#include "uart.h"
#include "mxc_device.h"

#define BUFF_SIZE           1024

/***************************** VARIABLES *************************************/

volatile int READ_FLAG;
volatile int WRITE_FLAG;
uint8_t RxData[BUFF_SIZE];

/************************   STATIC FUNCTIONS  *******************************/

/************************   PUBLIC FUNCTIONS  *******************************/

void writeCallback(mxc_uart_req_t* req, int error)
{
	WRITE_FLAG = error;
}

void readCallback(mxc_uart_req_t* req, int error)
{
	READ_FLAG = error;
}

unsigned int ESP_Send(mxc_uart_req_t* req, const uint8_t *bytes) {
	
	unsigned int error;
	mxc_uart_req_t write_req;
	write_req.uart = req;
	write_req.txData = bytes;
	write_req.txLen = strlen(bytes);
	write_req.rxLen = 0;
	write_req.callback = writeCallback;
	WRITE_FLAG = 1;

	error = MXC_UART_TransactionAsync(&write_req);

	if (error != E_NO_ERROR) {
		printf("-->Error starting async read: %d\n", error);
		printf("-->Example Failed\n");
		while (1) {}
	}
	MXC_UART_AsyncHandler(req);

	while (WRITE_FLAG);

	return WRITE_FLAG;
}

unsigned int ESP_Read(mxc_uart_req_t* req, uint8_t* OutputString) {

	unsigned int error;
	memset(RxData, 0x0, BUFF_SIZE);
	while (1) {
		error = MXC_UART_GetRXFIFOAvailable(req);// read frome buffer if available
		if (error > 0) {
			mxc_uart_req_t read_req;
			read_req.uart = req;
			read_req.rxData = RxData;
			read_req.rxLen = error;
			read_req.txLen = 0;
			read_req.callback = readCallback;
			READ_FLAG = 1;

			error = MXC_UART_TransactionAsync(&read_req);
			MXC_UART_AsyncHandler(req);

			if (error != E_NO_ERROR) {
				printf("-->Error starting async read: %d\n", error);
				printf("-->Example Failed\n");
				while (1) {}
			}

			while (READ_FLAG);

			if (READ_FLAG != E_NO_ERROR) {
				return READ_FLAG;

			}
			strcat(OutputString, RxData);
			memset(RxData, 0x0, BUFF_SIZE);
			if (strlen(OutputString) > 4) { // read until "OK" or "ERROR" appears
				if (OutputString[strlen(OutputString) - 4] == 'O' && OutputString[strlen(OutputString) - 3] == 'K') {
					return 0;
				}
				else if (OutputString[strlen(OutputString) - 4] == 'O' && OutputString[strlen(OutputString) - 3] == 'R') {
					return 100;
				}
			}
		}
	}
}



