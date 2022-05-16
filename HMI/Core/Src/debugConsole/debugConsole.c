/*
 * debugConsole.c
 *
 *  Created on: 04-Apr-2021
 *      Author: gmahez
 *
 */
#include <string.h>
#include <stdlib.h>

#include "cmsis_os.h"
#include "debugConsole.h"

UART_HandleTypeDef debugPort;          //!< There is a LL driver / Generated code of STM32 FW have limitation
                                    // ... We need to keep the variable name for uart configuration structer


SemaphoreHandle_t xMutexDebugUart = NULL;
QueueHandle_t gDebugConsoleQ;

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart);

static bool debugConsoletxCptF = true;

static bool debugConsoleInit( void );
static bool debugConsoleFlushOut( void );


volatile static bool uartError_f = false;

//!< @@@@@@@@@@@@@@@@@@@@ RING BUFF CONFIG @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//#define RING_BUFF_SIZE 50

void addToRing( char *strPtr, unsigned int strLeg );

unsigned char debugOutBuffer[RING_BUFF_SIZE] = { 0 };
unsigned int writePtr = 0;		//!< Initialize both to zero.
unsigned int readPtr = 0;

//!< @@@@@@@@@@@@@@@@@@@@ END RING BUFF @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

char tempBuff[50] = {0};
bool mDebugConInit = false;

unsigned char hiw[] = "Hello World\n\r";

//!< @@@@@@@@@@@@@@@@ Receive Command @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define MAX_CMD_LENGTH	25

volatile static uint16_t RxByteCnt = 0;
volatile static bool cmdRx_t = false;
volatile static uint8_t RxString[ MAX_CMD_LENGTH ] = { 0 };
volatile static uint8_t inputString[ MAX_CMD_LENGTH ] = { 0 };
volatile static bool cmdError = false;



uint8_t rxByte = 0;


void debugconsoleTask(void)
{
	bool txStatus = true;


	while( NULL == xMutexDebugUart );

	if( ! debugConsoleInit() )
    {
        while(1);           ///!< If uart initialization failed, don't run the task
    }else
    {

    	if( HAL_UART_STATE_READY == HAL_UART_GetState( &debugPort ) )
		{
			debugText("\n1******************************************** ");
			debugText("\nThis is Base code Generation ");
			debugText("\n############    BASE CODE     ############ ");
			debugText("\n*********************************************1\n\r");

		}else
		{
			while(1);
		}
    }

    HAL_UART_Receive_IT( &debugPort, &rxByte, 1);

    for (;;)
    {
    	if( ( HAL_UART_STATE_READY == ( HAL_UART_GetState( &debugPort ) & HAL_UART_STATE_READY ) ) && ( writePtr != readPtr ) )
    	txStatus = debugConsoleFlushOut();

    	if( (true == uartError_f )  || ( HAL_UART_ERROR_NONE != HAL_UART_GetError(&debugPort) ) )
    	{
    		HAL_UART_DeInit(&debugPort);
    		debugConsoleInit();

    	}
    	if( false == txStatus )
    	{
    		HAL_UART_AbortTransmit_IT( &debugPort );
    		txStatus = true;
    	     writePtr = 0;
    	     readPtr = 0;
    	}

    	if( true == cmdRx_t )
    	{
    		cmdRx_t = false;

    		debugText( inputString );
    	}

        vTaskDelay(100);

    }
}

/*********************************************************************************
 *Name :- debugConsoleInit
 *Para1:- N/A
 *Return:-N/A
 *Details:-  Initialize the uart used for debug.
 **********************************************************************************/
static bool debugConsoleInit( void )
{

    bool returnValue = true;
     /* USER CODE END USART2_Init  */
     debugPort.Instance = DEBUG_UART;
     debugPort.Init.BaudRate = 115200;
     debugPort.Init.WordLength = UART_WORDLENGTH_8B;
     debugPort.Init.StopBits = UART_STOPBITS_1;
     debugPort.Init.Parity = UART_PARITY_NONE;
     debugPort.Init.Mode = UART_MODE_TX_RX;
     debugPort.Init.HwFlowCtl = UART_HWCONTROL_NONE;
     debugPort.Init.OverSampling = UART_OVERSAMPLING_16;

     if (HAL_UART_Init(&debugPort) != HAL_OK)
     {
         returnValue = false;
       // Error_Handler(); NEED TO ADD error handling function.
     }else
     {
        // __HAL_UART_ENABLE_IT(&debugPort, UART_IT_TC);
     }

     writePtr = 0;
     readPtr = 0;
     mDebugConInit = returnValue;
     return ( returnValue );

}
/*********************************************************************************
 *Name :- debugText
 *Para1:- debugMsg
 *Return:-N/A
 *Details:-
 **********************************************************************************/
bool debugText( const char *debugMsg )
{
    char *msg =(char*) debugMsg;
    bool returnValue = false;

	//if ( true == xSemaphoreTake( xMutexDebugUart, portMAX_DELAY  ) )
    {
        if( mDebugConInit )
        {
        	addToRing( msg, strlen(msg) );
        }

        while ( pdTRUE != xSemaphoreGive( xMutexDebugUart ) );
    }

    return ( returnValue );
}
/*********************************************************************************
 *Name :- debugValue
 *Para1:- debugMsg
 *Para2:- value
 *Para3:- baseValue
 *Return:- PASS / FAIL
 *Details:-
 **********************************************************************************/
bool debugValue( int64_t value, uint8_t baseValue)
{

    IntToText(value, baseValue, tempBuff);
    debugText( tempBuff );

    return 1;
}
/*********************************************************************************
 *Name :- debugTextValue
 *Para1:- debugMsg
 *Para2:- value
 *Para3:- baseValue
 *Return:-N/A
 *Details:-
 **********************************************************************************/
bool debugTextValue( const char *debugMsg, int64_t value, uint8_t baseValue )
{
    bool returnValue = false;
    char localbuff[100] = { 0 };


    strcpy(localbuff, debugMsg );

    IntToText(value, baseValue, tempBuff);
    strcat( localbuff, tempBuff );
    debugText( localbuff );

    return returnValue;
}
/*********************************************************************************
 *Name :- IntToText
 *Para1:- value
 *Para2:- base ( Numerical base )
 *Para3:- str  ( pointer to buffer )
 *Return:-N/A
 *Details:-
 **********************************************************************************/
bool IntToText(int64_t value, uint8_t base, char * str )
{
	int64_t temp = 0;
    uint8_t i = 0;
    int64_t division = value;

    if( 0 == division )
    {
        *(str+i++) = temp|0x30;
        *(str+i) = '\0';
    }else
    {
    	if( 0 > division  )
    	{
    		division = abs(division);
    	}
        while ( 0 != division )
        {
            temp = division%base;
            division /= base;

            if( ( base == HEX ) && (temp >= DECIMAL) )
            {
                *(str+i++) = (temp - DECIMAL ) + 'A';
             }
            else
            {
                *(str+i++) = temp|0x30;
            }
        }
       	if( base == HEX )
		{
			*(str+i++) = 'x';
			*(str+i++) = '0';
			*(str+i++) = ' ';
		}else if( base == BINARY )
		{
			*(str+i++) = '.';
			*(str+i++) = 'b';
			*(str+i++) = ' ';
		}

    	if( ( 0 > value ) && ( base == DECIMAL ) )
    	{
    		*(str+i++) = '-';
    	}

        *(str+i) = '\0';
        reverseStr(str, i);
    }


    return true;
}
/*********************************************************************************
 *Name :- reverseStr
 *Para1:- pointer to string ( buffer)
 *Para2:- size of the string.
 *Return:-N/A
 *Details:-
 **********************************************************************************/
void reverseStr(char *str, uint8_t size)
{
    uint8_t i = size-1;
    uint8_t j = 0;
    uint8_t temp = i;

    do
    {
        temp     = *(str+j);
        *(str+j) = *(str+i);
        *(str+i) = temp;
        if( i == 0 ) break;		//!<  NEED TO FIND A BETTER WAY TO HANDLE THE UNDERFLOW !
        i = i-1;
        j = j +1;
        temp = (unsigned char) i/2;
    }while( i > j );

}
/*********************************************************************************
 *Name :- addToRing
 *Para1:- pointer to string ( buffer)
 *Para2:- size of the string.
 *Return:-N/A
 *Details:-
 **********************************************************************************/
void addToRing( char *strPtr, unsigned int strLeg )
{
	unsigned int remaingBuff = (RING_BUFF_SIZE - writePtr) - 1 ;

	if ( true == xSemaphoreTake( xMutexDebugUart, portMAX_DELAY  ) )
    {
	if( remaingBuff > strLeg )
	{
		memcpy( &debugOutBuffer[writePtr], strPtr, strLeg);
		writePtr +=  strLeg;
	}else
	{
		memcpy(&debugOutBuffer[writePtr], strPtr, remaingBuff );
		memcpy(debugOutBuffer, (unsigned char *)(strPtr+remaingBuff), (strLeg-remaingBuff) );

		writePtr = (strLeg-remaingBuff);
	}

    }
}

/*********************************************************************************
 *Name :- addToRing
 *Para1:- last send out pointer ( read pointer )
 *Para2:- Upto which pointer/place ( write pointer ).
 *Return:-N/A
 *Details:-
 **********************************************************************************/
static bool debugConsoleFlushOut( void )
{
    bool returnValue = true;


    if( ( mDebugConInit ) && ( writePtr != readPtr ) )
    {

		if( writePtr > readPtr )
		{
     		if( HAL_OK != HAL_UART_Transmit_IT(&debugPort, (uint8_t *)(debugOutBuffer+readPtr), (uint16_t) (writePtr-readPtr)) )
         	//if( HAL_OK != HAL_UART_Transmit(&debugPort, (uint8_t *)(debugOutBuffer+readPtr), (writePtr-readPtr), 100) )   //!< If don't want to use interrupt
			{
				returnValue &= false;
			}else
			{
				readPtr = writePtr;
			}

		}else
		{
			if( HAL_OK != HAL_UART_Transmit_IT(&debugPort, (uint8_t *)(debugOutBuffer+readPtr), (uint16_t) (RING_BUFF_SIZE-readPtr)) )
			//if( HAL_OK != HAL_UART_Transmit(&debugPort, (uint8_t *)(debugOutBuffer+readPtr), (RING_BUFF_SIZE-readPtr), 100 ) )   //!< If don't want to use interrupt
			{
				returnValue &= false;
			}else
			{
				debugConsoletxCptF = false;
				readPtr = 0;
			}
		}
	}


return returnValue;
}

bool debugTaskStatusGet( void )
{
	return(debugConsoletxCptF);
}

/***************************************************************************************
 * @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 * ************************************************************************************/
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
	//while(1);
	uartError_f = true;
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
	debugConsoletxCptF = true;
	return;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{


    RxString[RxByteCnt] = rxByte;


    if( ('\r' == rxByte) || ('\n' == rxByte) )
    {
        if( RxByteCnt != 0 )
        {
            strncpy( inputString, RxString, (RxByteCnt) );
            inputString[RxByteCnt] = '\0';
            RxByteCnt = 0;
            cmdRx_t = true;

        }else
        {
            //RxByteCnt = 0;
        }

    }else
    {
        if( (MAX_CMD_LENGTH-1) <= RxByteCnt++ )
        {
        	HAL_UART_Receive_IT( &debugPort, &rxByte, 1);
        	RxByteCnt = 0;
        	//debugText("\nExceed Max Length !\n1:>");
        }
    }


    HAL_UART_Receive_IT( &debugPort, &rxByte, 1);


}

