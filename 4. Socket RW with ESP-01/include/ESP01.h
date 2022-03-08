#ifndef _ESP01_H_
#define _ESP01_H_

#include "uart.h"
#include <stdint.h>
#include "uart_regs.h"
#include "mxc_sys.h"
#define BUFF_SIZE           1024

/*****************************     VARIABLES *********************************/
// Controller for exchanging data with ESP-01
struct ESPController {
	char Mode[5];			// "TCP" or "UDP"
	char SSID[33];			// SSID of AP to be connected. MAX 32 char
	char SSID_pwd[33];		// Password of AP; 
	char IP[17];			// IP to conncect. IPv4
	char Port[5];			// Port to connect;
	mxc_uart_regs_t* UART;	// UART used;

	uint8_t SocketSend;		// Something new to send by socket?
	uint8_t WifiConn;		// Wifi connected?
	uint8_t ServerConn;		// Server Connected

	char R_UARTString[BUFF_SIZE];	// UART read string
	char W_UARTString[BUFF_SIZE];	// UART write string
	char R_SOCKString[BUFF_SIZE];	// Socket read string
	char W_SOCKString[BUFF_SIZE];	// Socket write string
};

/*****************************     FUNCTIONS *********************************/
void writeCallback(mxc_uart_req_t* req, int error);	// Callback for UART async write
void readCallback(mxc_uart_req_t* req, int error);  // Callback for UART async read

// initiate tcp function
// 1. UART Used
// 2. AP SSID
// 3. AP Password 
// 4. Socket mode "TCP" or "UDP"
// 5. Server IP
// 6. Server Port
struct ESPController* CreateESPController(mxc_uart_regs_t* UART, char* SSID, char* SSID_pwd, char* Mode, char* IP, char* Port);

// Connect with ESPController
// Return value
// 0. Success 
// 1. Can't send UART
// 2. Unable to change mode
// 3. Unable to connect to AP
// 4. Unable to connect to Server
uint8_t  ESPConnect(struct ESPController* Controller);
 
// Send through socket
uint8_t ESPSocketSend(struct ESPController* Controller);

// Socket Client Task
void ClientTask(void* pvParameters);

// Uart command through ESP01
int ESP_Send(mxc_uart_regs_t* req, const char* bytes);
int ESP_Read(mxc_uart_regs_t* req, char* OutputString);

#endif