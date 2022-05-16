/*
 * encoder.c
 *
 *  Created on: 20-Apr-2022
 *      Author: gmahez
 */
#include "encoder.h"
#include "debugConsole.h"


static volatile bool timerICtrigger_f = false;
static volatile bool vAstatus_f = false;  //!<  PB8 status
static volatile bool vBstatus_f = false;  //!<  PC6 status

static volatile uint8_t leftEncoderCnt = 0;
static volatile uint8_t leftrCnt = 0;

static volatile enoderData_t encoderVolt;

void userInputTask(void)
{

	//HAL_TIM_IC_Start_IT(&htim4, TIM_CHANNEL_3);

	for(;;)
	{
		if( true ==timerICtrigger_f )
		{
			timerICtrigger_f = false;
			if( 0 < encoderVolt.count)
			{
				debugTextValue("\r\nClockwis\tCount - ", encoderVolt.count, DECIMAL );
			}else if( 0 > encoderVolt.count)
			{
				debugTextValue("\r\nAnti-Clockwis\tCount - ", encoderVolt.count, DECIMAL );
			}


			encoderVolt.count = 0;
		}

		vTaskDelay(1);
	}
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if( ENCODER_DEBOUNCING_TICK_COUNT <= ( osKernelGetTickCount() - encoderVolt.tickTimer ) )
	{
		encoderVolt.tickTimer = osKernelGetTickCount();

		timerICtrigger_f = true;

		if( true == HAL_GPIO_ReadPin( GPIOC, GPIO_PIN_6 ) )
		{
			encoderVolt.count++;
		}else
		{
			encoderVolt.count--;
		}
	}

}

void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim)
{

}
