# Socket read and write with ESP01
* Use UART1. Connect P0_13(TXD) to ESP01 RXD and P0_12(RXD) to ESP01 TXD
* To send: 
	* Change the content in ESPController.W_SOCKString
	* Set ESPController.SocketSend to 1
"""
strcpy(Controller->W_SOCKString, temp);
Controller->SocketSend = 1;
"""