/*
 * application.h
 *
 *  Created on: 22-Mar-2022
 *      Author: gmahez
 */

#ifndef APPLICATION_APPLICATION_H_
#define APPLICATION_APPLICATION_H_

#include "cmsis_os.h"   //!< included it to use some macro definition.
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "sysConfigEsab.h"

typedef struct taskpara
{
	TaskFunction_t  vTaskfunPtr;
    char vTaskName[configMAX_TASK_NAME_LEN];
	unsigned short stacksize;
	void * VTaskparaeter;
	UBaseType_t VtaskPriority;
	TaskHandle_t *pxCreatedTask;
}sysTask_t;

//!<  THIS BELOW ENUM SEQUENCE IS DIRECTLY CONNECT WITH BELOW TASK PARAMETER STRUCTURE ( sysTask ).
//!< PLEASE TAKECARE WHILE MODIFING "sysTaskList" and sysTask
typedef enum{
	CONSOLE_TASK,
	OUTERHMI_TASK,
	//CANMASTER_TASK,
	MAXMUM_TASK
}sysTaskList_t;


//Extern Function
void application( void );

#endif /* APPLICATION_APPLICATION_H_ */
