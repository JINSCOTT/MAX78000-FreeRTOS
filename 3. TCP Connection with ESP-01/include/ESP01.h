#ifndef _ESP01_H_
#define _ESP01_H_

#include "uart.h"
#include <stdint.h>

/*****************************     VARIABLES *********************************/


/*****************************     FUNCTIONS *********************************/
void writeCallback(mxc_uart_req_t* req, int error);
void readCallback(mxc_uart_req_t* req, int error);

unsigned int ESP_Send(mxc_uart_req_t* req, const uint8_t* bytes);	
unsigned int ESP_Read(mxc_uart_req_t* req, uint8_t* OutputString);

#endif