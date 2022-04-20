/*
 * sysConfigEsab.h
 *
 *  Created on: 22-Mar-2022
 *      Author: gmahez
 */

#ifndef SYSCONFIGESAB_H_
#define SYSCONFIGESAB_H_

#include <stdbool.h>


#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#ifdef STM32G070xx
#include "stm32g0xx_hal.h"
#include "stm32g0xx_hal_conf.h"
#endif

#if defined(STM32Fxx) || defined(STM32F427xx ) || defined(STM32F407xx)
#include "stm32f4xx_hal.h"
#endif

#if defined (STM32F769xx)
#include "stm32f7xx_hal.h"
#endif

#ifndef ON
#define ON 1
#endif

#ifndef OFF
#define OFF 0
#endif

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

#ifndef SET
#define SET 1
#endif

#ifndef CLEAR
#define CLEAR 0
#endif


#endif /* SYSCONFIGESAB_H_ */
