/*
 * buttonWrapper.h
 *
 *  Created on: 23-Mar-2019
 *      Author: gmahez
 */

#ifndef GPIOWRAPPER_H_
#define GPIOWRAPPER_H_

#include "sysConfigEsab.h"

#define READ_PIN(a,b)       HAL_GPIO_ReadPin((GPIO_TypeDef*)(a), (uint16_t) b )
#define WRITE_PIN(a,b,c )   { HAL_GPIO_WritePin((GPIO_TypeDef*) (a), (uint16_t) (b), (GPIO_PinState) (c)) ; }
#define TOGGLE_PIN( a, b )	{ HAL_GPIO_TogglePin( (GPIO_TypeDef*) (a), (uint16_t) (b) ) ; }
#define READ_PORT( a )      esab_stmF4HAL_ReadPort( GPIO_TypeDef* GPIOx )


//#define PIN_INIT( a )        { HAL_GPIO_Init(key [ a ].port, (GPIO_InitTypeDef *) &key [ a ].pinConfig); }
#define PIN_INIT( a, b )        { HAL_GPIO_Init((GPIO_TypeDef*)(a), (GPIO_InitTypeDef *) b ); }


typedef struct
{
    GPIO_TypeDef*   port;
    GPIO_InitTypeDef pinConfig;
}gpioPinConfig_t;



#endif /* GPIOWRAPPER_H_ */
