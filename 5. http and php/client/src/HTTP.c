#include "ESP01.h"
#include "HTTP.h"
#include "uart.h"
#include <stdint.h>
#include "uart_regs.h"
#include "mxc_sys.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define BUFF_SIZE           1024
/************************   PUBLIC FUNCTIONS  *******************************/
struct HttpRequest* HTTPCreate(struct ESPController* controller, char* method, char* path, char* host, char* contenttype, char* content) {
	struct HttpRequest* Request = malloc(sizeof(struct HttpRequest));
	if (Request == NULL) return NULL;
	Request->controller = controller;
	strcpy(Request->method, method);
	strcpy(Request->path, path);
	strcpy(Request->host, host);
	strcpy(Request->contenttype, contenttype);
	strcpy(Request->content, content);

	// Initiate strings and flags

	memset(Request->responseheader, 0x0, BUFF_SIZE);
	memset(Request->response, 0x0, BUFF_SIZE);
	memset(Request->controller->W_SOCKString, 0x0, BUFF_SIZE);
	SetSERVER(Request->controller, "TCP", Request->host, "80"); // http 80, https 443
	return Request;
}

uint8_t HTTPsend(struct HttpRequest* HTTP) {
	uint8_t Error;
	char contentlength[3];
	sprintf(contentlength, "%d", strlen(HTTP->content));
	Error = ESPConnect(HTTP->controller);
	memset(HTTP->response, '\0', BUFF_SIZE);
	memset(HTTP->responseheader, '\0', BUFF_SIZE);
	memset(HTTP->controller->W_SOCKString, 0x0, BUFF_SIZE);
	strcpy(HTTP->controller->W_SOCKString, HTTP->method);
	strcat(HTTP->controller->W_SOCKString, " ");
	strcat(HTTP->controller->W_SOCKString, HTTP->path);
	strcat(HTTP->controller->W_SOCKString, " HTTP/1.1\r\nHost: ");
	strcat(HTTP->controller->W_SOCKString, HTTP->host);
	strcat(HTTP->controller->W_SOCKString, "\r\nContent-Type: ");
	strcat(HTTP->controller->W_SOCKString, HTTP->contenttype);
	strcat(HTTP->controller->W_SOCKString, "\r\nContent-Length: ");
	strcat(HTTP->controller->W_SOCKString, contentlength);
	strcat(HTTP->controller->W_SOCKString, "\r\n\r\n");
	strcat(HTTP->controller->W_SOCKString, HTTP->content);

	Error = ESPSocketSend(HTTP->controller);
	if (Error != 0)
	{
		return Error;
	}
	Error = HTTP_Read(HTTP->controller, HTTP->responseheader, HTTP->response);
	if (Error != 0)
	{
		return Error;
	}
	printf(" Response content: \n%s", HTTP->responseheader, HTTP->response);
	return 0;
}
