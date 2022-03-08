#ifndef _HTTP_h_
#define _HTTP_h_

#define BUFF_SIZE           1024

#include "ESP01.h"
/*****************************     STRUCTS     *********************************/
struct HttpRequest {
	struct ESPController* controller;
	char method[5];					// GET or POST
	char path[50];					// e.g. /xxx.php ,if none "/"
	char host[50];					// hostname or ip  
	char contenttype[50];			//e.g.  application/x-www-form-urlencoded
	char content[BUFF_SIZE];		// Send content
	char response[BUFF_SIZE];		// response content
	char responseheader[BUFF_SIZE];	// response header
};
/*****************************     FUNCTIONS *********************************/

// Create http request
// the ESPController should be created first using CreateESPController()
struct HttpRequest* HTTPCreate(struct ESPController* controller, char* method, char* path, char* host, char* contenttype, char* content);

//Send HTTP request
uint8_t HTTPsend(struct HttpRequest* HTTP);


#endif