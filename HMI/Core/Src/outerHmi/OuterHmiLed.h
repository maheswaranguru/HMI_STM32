/*
 * OuterHmiLed.h
 *
 *  Created on: 05-Apr-2022
 *      Author: gmahez
 */

#ifndef SRC_OUTERHMI_OUTERHMILED_H_
#define SRC_OUTERHMI_OUTERHMILED_H_

#include "FreeRTOS.h"
#include "sysConfigEsab.h"

void outerHMITask ( void );


//!< PROCESS RELATED..

#define PROCESSBYTE_1   3
#define PROCESSBYTE_2   2

typedef enum
{
    BIT_MIG_MAN = 1,
    BIT_SYNERCY = 2,
    BIT_PULSE   = 0,
    BIT_SPEED   = 3,
    BIT_DEEP    = 5,
    BIT_CRAFT   = 7,
    BIT_VUP     = 6,
    BIT_THIN    = 4,
    BIT_ROOT    = 1,
    BIT_MMA     = 1,
    BIT_GOUGING = 0,
    BIT_CC_G    = 1,
    BIT_CC_Y    = 2
}processLedBit_t;

typedef enum
{
    OUTER_HMI_WELD_APP_MIG_MANUAL = 1,  //!< 1
    OUTER_HMI_WELD_APP_MIG_SYNERGY,     //!< 2
    OUTER_HMI_WELD_APP_PULSE,           //!< 3
    OUTER_HMI_WELD_APP_SPEED,           //!< 4
    OUTER_HMI_WELD_APP_DEEP,            //!< 5
    OUTER_HMI_WELD_APP_CRAFT,           //!< 6
    OUTER_HMI_WELD_APP_V_UP,            //!< 7
    OUTER_HMI_WELD_APP_THIN,            //!< 8
    OUTER_HMI_WELD_APP_ROOT,            //!< 9
    OUTER_HMI_WELD_APP_MMA,             //!< 10
    OUTER_HMI_WELD_APP_GOUGING,         //!< 11
    OUTER_HMI_WELD_APP_COMPENSATION = 100,
    OUTER_HMI_WELD_APP_INVALID = 255,
} enum_outer_hmi_weld_app_t;


//!< JOBs Related...

#define JOBBYTE_1   0
#define JOBBYTE_2   1

typedef enum
{
    JOB_ONE_WHITE = 0,
    JOB_ONE_GREEN = 1,
    jOB_ONE_DOT = 7,
    JOB_TWO_WHITE = 3,
    JOB_TWO_GREEN = 4,
    jOB_TWO_DOT = 2,
    JOB_THREE_WHITE = 6,
    JOB_THREE_GREEN = 7,
    jOB_THREE_DOT = 5,
}eJobLeds_t;

//!< Right Side Digits Related

#define RIGHT_SEGMENT_BYTE1 12
#define RIGHT_SEGMENT_BYTE2 13
#define RIGHT_SEGMENT_BYTE3 14

//!< Left Side Digits Related

#define LEFT_SEGMENT_BYTE1 15
#define LEFT_SEGMENT_BYTE2 16
#define LEFT_SEGMENT_BYTE3 17

//!< Right Eye Brow

#define RIGHT_EYEBROW_BYTE1 8
#define RIGHT_EYEBROW_BYTE2 9

#define RIGHT_EYEBROW_MASK_20   0x80
#define RIGHT_EYEBROW_MASK_40   0x01
#define RIGHT_EYEBROW_MASK_60   0x02
#define RIGHT_EYEBROW_MASK_80   0x04
#define RIGHT_EYEBROW_MASK_100  0x08

#define RIGHT_EYEBROW_MASK_N20   0x10
#define RIGHT_EYEBROW_MASK_N40   0x08
#define RIGHT_EYEBROW_MASK_N60   0x04
#define RIGHT_EYEBROW_MASK_N80   0x02
#define RIGHT_EYEBROW_MASK_N100  0x01

#define RIGHT_EYEBROW_MASK_CTRG     0x20
#define RIGHT_EYEBROW_MASK_CTRW     0x40

#define RIGHT_EYEBROW_MASK_ALL1  0x0F
#define RIGHT_EYEBROW_MASK_ALL2  0xFF

//!< LEFT Eye Brow

#define LEFT_EYEBROW_BYTE1 10
#define LEFT_EYEBROW_BYTE2 11

#define LEFT_EYEBROW_MASK_20   0x80
#define LEFT_EYEBROW_MASK_40   0x01
#define LEFT_EYEBROW_MASK_60   0x02
#define LEFT_EYEBROW_MASK_80   0x04
#define LEFT_EYEBROW_MASK_100  0x08

#define LEFT_EYEBROW_MASK_N20   0x10
#define LEFT_EYEBROW_MASK_N40   0x08
#define LEFT_EYEBROW_MASK_N60   0x04
#define LEFT_EYEBROW_MASK_N80   0x02
#define LEFT_EYEBROW_MASK_N100  0x01

#define LEFT_EYEBROW_MASK_CTRG     0x20
#define LEFT_EYEBROW_MASK_CTRW     0x40

#define LEFT_EYEBROW_MASK_ALL1  0x0F
#define LEFT_EYEBROW_MASK_ALL2  0xFF


//!< Units LED

#define UNITS_BYTE          4
#define UNITS_MASK          0xFF

#define UNITS_AMPS_MASK     0x80
#define UNITS_EMBERAGE_MASK 0x40
#define UNITS_ARC_MASK      0x20
#define UNITS_VOLT_MASK     0x10
#define UNITS_INC_MIN_MASK  0x08
#define UNITS_MTR_MIN_MASK  0x02
#define UNITS_DYNAMICS_MASK 0x01

//!< Status LEDS
#define STATUS_BYTE1    1
#define STATUS_BYTE2    2
#define STATUS_BYTE3    5
#define STATUS_BYTE4    6
#define STATUS_BYTE5    7


#define OPTR_YELLOW_MASK    0x60
#define OPTR_RED_MASK       0x18
#define OPTR_GREEN_MASK     0x80

#define TOOLS_RED_MASK      0x01
#define TOOLS_GREEN_MASK    0x80

#define CC_GREEN_MASK       0x02
#define CC_YELLOW_MASK      0x04

#define TEMP_RED_MASK       0x08
#define TEMP_GREEN_MASK     0x10

#define GAS_WARNING_RED_MASK    0x20
#define GAS_WARNING_GREEN_MASK  0x40

#define ANALOG_REMOTE_MASK  0x04

#define ST1_TO_2_MASK       0x08
#define ST1_TO_3_MASK       0x10

#define STANDBY_GREEN_MASK  0x20
#define STANDBY_RED_MASK    0x40

#define BT_RED_MASK         0x20
#define BT_GREEN_MASK       0x10

#define LOCK_RED_MASK       0x80
#define LOCK_GREEN_MASK     0x40


typedef enum{

    OPERATOR,
    WELDCLOUD,
    BLUETOOTH,
    LOCK,
    ANALOG_REMOTE,
    SHIFT_TRIGGER_1_2,
    SHIFT_TRIGGER_1_3,
    STANDBY,
    MAINTANACE,
    CABLE_COMPENSATION,
    WARNING,
    GAS_ERROR
}eStatusLed_t;

//!< Extern functions

bool LEDSelfTest ( void );

void oHmiprocessUpdate ( enum_outer_hmi_weld_app_t process );
bool oHmiAllLedOff ( void );
void oHmiJobUpdate( uint8_t jobNo );
void oHmiRightSegmentUpdate( int16_t value, uint8_t units );
void oHmiLeftSegmentUpdate( int16_t value, uint8_t units );
void oHmiLeftEyeBrowUpdate( int8_t percent );
void oHmiRightEyeBrowUpdate( int8_t percent );

#endif /* SRC_OUTERHMI_OUTERHMILED_H_ */
