/*
 * encoder.h
 *
 *  Created on: 20-Apr-2022
 *      Author: gmahez
 */

#ifndef SRC_ENCODER_ENCODER_H_
#define SRC_ENCODER_ENCODER_H_

#include <stdint.h>

#define ENCODER_DEBOUNCING_TICK_COUNT 10

typedef struct
{
	int16_t count;
	int32_t tickTimer;

}enoderData_t;

void userInputTask(void);


#endif /* SRC_ENCODER_ENCODER_H_ */
