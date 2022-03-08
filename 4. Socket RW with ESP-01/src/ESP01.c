#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "ESP01.h"
#include "uart.h"
#include "mxc_device.h"
#include <math.h>

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

struct ESPController* CreateESPController(mxc_uart_regs_t* UART, char* SSID, char* SSID_pwd, char* Mode, char* IP, char* Port) {
	struct ESPController* Controller = malloc(sizeof(struct ESPController));
	if (Controller == NULL) return NULL;
	
	Controller->UART = UART;
	strcpy((char*)Controller->SSID, SSID);
	strcpy((char*)Controller->SSID_pwd, SSID_pwd);
	strcpy((char*)Controller->Mode, Mode);
	strcpy((char*)Controller->IP, IP);
	strcpy((char*)Controller->Port, Port);

	// Initiate strings and flags
	memset(Controller->R_UARTString, 0x0, BUFF_SIZE);
	memset(Controller->W_UARTString, 0x0, BUFF_SIZE);
	memset(Controller->R_SOCKString, 0x0, BUFF_SIZE);
	memset(Controller->W_SOCKString, 0x0, BUFF_SIZE);
	Controller->WifiConn = 0;
	Controller->ServerConn = 0;
	Controller->ServerConn = 0;

	return Controller;
};


uint8_t ESPConnect(struct ESPController* Controller) {
	int Error;
	if (Controller->WifiConn == 0) {

		Error = ESP_Send(Controller->UART, "ATE0\r\n"); // No echo
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 1;
		}
		Error = ESP_Read(Controller->UART, Controller->R_UARTString);
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 2;
		}

		Error = ESP_Send(Controller->UART, "AT+CWMODE=1\r\n"); // Set to client mode
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 1;
		}
		memset(Controller->R_UARTString, 0x0, BUFF_SIZE);
		Error = ESP_Read(Controller->UART, Controller->R_UARTString);
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 2;
		}
		printf("%s", Controller->R_UARTString);

		strcpy((char*)Controller->W_UARTString, "AT+CWJAP=\"");
		strcat((char*)Controller->W_UARTString, (char*)Controller->SSID);
		strcat((char*)Controller->W_UARTString, "\",\"");
		strcat((char*)Controller->W_UARTString, (char*)Controller->SSID_pwd);
		strcat((char*)Controller->W_UARTString, "\"\r\n");
		Error = ESP_Send(Controller->UART, Controller->W_UARTString); // Connect to AP ,"AT+CWJAP\"SSID\",\"PWD\"
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 1;
		}

		Error = ESP_Read(Controller->UART, Controller->R_UARTString);
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 3;
		}
		printf("%s", Controller->R_UARTString);
		Controller->WifiConn = 1;
	}

	if (Controller->ServerConn == 0) {
		memset(Controller->R_UARTString, 0x0, BUFF_SIZE);
		strcpy((char*)Controller->W_UARTString, "AT+CIPSTART=\"");
		strcat((char*)Controller->W_UARTString, (char*)Controller->Mode);
		strcat((char*)Controller->W_UARTString, "\",\"");
		strcat((char*)Controller->W_UARTString, (char*)Controller->IP);
		strcat((char*)Controller->W_UARTString, "\",");
		strcat((char*)Controller->W_UARTString, (char*)Controller->Port);
		strcat((char*)Controller->W_UARTString, "\r\n");
		Error = ESP_Send(Controller->UART, Controller->W_UARTString); // Connect to TCP ,"AT+CIPSTART\"TCP or UDP\",\"IP\",port"
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 1;
		}

		Error = ESP_Read(Controller->UART, Controller->R_UARTString);
		if (Error != E_NO_ERROR) {
			printf("-->Error with UART_WriteAsync callback; %d\n", Error);
			return 4;
		}
		printf("%s", Controller->R_UARTString);

	}
	Controller->WifiConn = 1;
	Controller->ServerConn = 1;
	return 0;
};

uint8_t ESPSocketSend(struct ESPController* Controller) {
	char temp[5];
	memset(temp, 0x0, 5);
	int error = 0;

	if (strlen(Controller->W_SOCKString) <= 0) {
		Controller->SocketSend = 0;
		return 0;
	}
	strcpy(Controller->W_UARTString, "AT+CIPSEND=");
	sprintf(temp, "%d", strlen(Controller->W_SOCKString));
	strcat(Controller->W_UARTString, temp);
	strcat(Controller->W_UARTString, "\r\n");
	error = ESP_Send(Controller->UART, Controller->W_UARTString);
	if (error != E_NO_ERROR) {
		return error;
	}
	memset(Controller->R_UARTString, 0x0, BUFF_SIZE);
	while (1) {
		error = MXC_UART_GetRXFIFOAvailable(Controller->UART);// read frome buffer if available
		if (error > 0) {
			mxc_uart_req_t read_req;
			read_req.uart = Controller->UART;
			read_req.rxData = RxData;
			read_req.rxLen = error;
			read_req.txLen = 0;
			read_req.callback = readCallback;
			READ_FLAG = 1;

			error = MXC_UART_TransactionAsync(&read_req);
			MXC_UART_AsyncHandler(Controller->UART);

			if (error != E_NO_ERROR) {
				printf("-->Error starting async read: %d\n", error);
				printf("-->Example Failed\n");
				while (1) {}
			}

			while (READ_FLAG);

			if (READ_FLAG != E_NO_ERROR) {
				return READ_FLAG;

			}
			strcat((char*)Controller->R_UARTString, (char*)RxData);
			memset(RxData, 0x0, BUFF_SIZE);
			if (Controller->R_UARTString[strlen((char*)Controller->R_UARTString) - 1] == '>') {
				break;
			}
		}
	}
	error = ESP_Send(Controller->UART, Controller->W_SOCKString);
	if (error != E_NO_ERROR) {
		return error;
	}
	error = ESP_Read(Controller->UART, Controller->R_UARTString);
	if (error != E_NO_ERROR) {
		return error;
	}
	printf("%s sent\n", Controller->W_SOCKString);
	return 0;
}

void ClientTask(void* pvParameters) {
	struct ESPController* Controller = (struct ESPController*)pvParameters;
	uint8_t error;
	uint8_t length;
	
	while (1) {
		if (Controller->WifiConn == 0 || Controller->ServerConn == 0) {
			printf("Retry Connect...\n");
			error = ESPConnect(Controller);
			if (error != 0) printf("Reconnect Failed\n");
			continue;
		}
		memset(RxData, 0x0, BUFF_SIZE);
		mxc_uart_req_t read_req;
		read_req.uart = Controller->UART;
		read_req.rxData = RxData;
		read_req.rxLen = 7;
		read_req.txLen = 0;
		read_req.callback = readCallback;
		READ_FLAG = 1;

		error = MXC_UART_TransactionAsync(&read_req);
		MXC_UART_AsyncHandler(Controller->UART);

		if (error != E_NO_ERROR) {
			printf("-->Error starting async read: %d\n", error);
			continue;
		}
		while (READ_FLAG) {
			if (Controller->SocketSend == 1) {
				MXC_UART_AbortAsync(Controller->UART);
				error = ESPSocketSend(Controller);
				if (error != E_NO_ERROR) {
					printf("Error Socket Send%d\n",error);
				}
				READ_FLAG = 0;
				break;
			}
		};
		if (READ_FLAG != E_NO_ERROR) {
			printf("-->Error async read: %d\n", READ_FLAG);
			continue;
		}
		if (Controller->SocketSend == 1) {
			Controller->SocketSend = 0;
			continue;
		}
		else {
			if (strcmp((char*)RxData, "CLOSED\r")==0) {// CLOSED\r\n
				printf("Server disconnected\n");
				Controller->ServerConn = 0;
				continue;
			}
			else if (strcmp((char*)RxData, "\r\n+IPD,")==0) { // FORMAT: "+IPD,NUM:"
				length = 0;
				while (1) {
					error = MXC_UART_ReadCharacter(Controller->UART);
					if ((char)error != ':') {
						length = 10 * length;
						length += (error - '0');
					}
					else {
						memset(Controller->R_SOCKString, 0x0, BUFF_SIZE);
						mxc_uart_req_t read_req;
						read_req.uart = Controller->UART;
						read_req.rxData = (uint8_t*)Controller->R_SOCKString;
						read_req.rxLen = length;
						read_req.txLen = 0;
						read_req.callback = readCallback;
						READ_FLAG = 1;

						error = MXC_UART_TransactionAsync(&read_req);
						MXC_UART_AsyncHandler(Controller->UART);

						if (error != E_NO_ERROR) {
							printf("-->Error starting async read: %d\n", error);
							break;
						}

						while (READ_FLAG);

						if (READ_FLAG != E_NO_ERROR) {
							printf("-->Error starting async read: %d\n", READ_FLAG);
							break;
						}
						printf("Received %d: %s\n", length, Controller->R_SOCKString);
						break;
					}

				}
			}
			else {
				printf("Unexpected:%s\nend\n",RxData);
			}
		}
	}
}

int ESP_Send(mxc_uart_regs_t* req, const char* bytes) {

	int error;
	mxc_uart_req_t write_req;
	write_req.uart = req;
	write_req.txData = (uint8_t*)bytes;
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

int ESP_Read(mxc_uart_regs_t* req, char* OutputString) {

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
			strcat((char*)OutputString, (char*)RxData);
			memset(RxData, 0x0, BUFF_SIZE);
			if (strlen((char*)OutputString) > 4) { // read until "OK" or "ERROR" appears
				if (OutputString[strlen((char*)OutputString) - 4] == 'O' && OutputString[strlen((char*)OutputString) - 3] == 'K') {
					return 0;
				}
				else if (OutputString[strlen((char*)OutputString) - 4] == 'O' && OutputString[strlen((char*)OutputString) - 3] == 'R') {
					return 100;
				}
			}
		}
	}
}



