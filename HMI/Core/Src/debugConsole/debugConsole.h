/*
 * debugConsole.h
 *
 *  Created on: 09-Apr-2019
 *      Author: gmahez
 */

#ifndef DEBUGCONSOLE_DEBUGCONSOLE_H_
#define DEBUGCONSOLE_DEBUGCONSOLE_H_

#include "FreeRTOS.h"
#include "sysConfigEsab.h"
#include "semphr.h"




//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
#define RING_BUFF_SIZE	512
#define DEBUG_UART	USART2

//@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define DECIMAL 10
#define HEX     16
#define BINARY  2


extern QueueHandle_t gDebugConsoleQ;
extern SemaphoreHandle_t xMutexDebugUart;


void debugconsoleTask(void);

bool debugText( const char *debugMsg );
bool debugValue( uint32_t value, uint8_t baseValue );
bool debugTextValue( const char *debugMsg, uint32_t value, uint8_t baseValue );


bool IntToText(uint32_t value, uint8_t base, char * str );
void reverseStr(char *str, uint8_t size);

//void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

#endif /* DEBUGCONSOLE_DEBUGCONSOLE_H_ */
