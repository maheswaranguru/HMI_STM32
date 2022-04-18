/*
 * OuterHmiLed.c
 *
 *  Created on: 05-Apr-2022
 *      Author: gmahez
 */

#include "cmsis_os.h"
#include "OuterHmiLed.h"
#include "debugConsole.h"

#define LED_BYTE_SIZE   18      //!< The led status container size in bytes

void InitLedSpi ( void );
bool UpdateLED ( uint8_t *data );

SPI_HandleTypeDef ledDriverSpi;

uint8_t oHmiLedStatus[ LED_BYTE_SIZE ] = { 0 };
bool oHmiLedUpdateRequired_f = false;

const uint8_t decode7segment[10] ={
    0x3F,        // '0'
    0x06,        // '1'
    0x5B,        // '2'
    0x4F,        // '3'
    0x66,        // '4
    0x6D,        // '5'
    0x7D,        // '6'
    0x07,        // '7'
    0x7F,        // '8'
    0x6F,        // '9'
};

/*********************************************************************************
 *Name :- outerHMITask
 *Para1:- N/A
 *Return:-N/A
 *Details:-  OuterHMI task to update outerpanel
 **********************************************************************************/
void outerHMITask ( void )
{
uint16_t i = 0;

int8_t testEye = 0;

    InitLedSpi ();
    osDelay ( 100 );

    debugText ( "\n\n********** Enter into OuterHMI Task ***********\n\n" );


    while( i++ < 1000)
    {
        oHmiRightSegmentUpdate( i, 0);
        oHmiLeftSegmentUpdate( i, 0 );

        oHmiRightEyeBrowUpdate( testEye );
        oHmiLeftEyeBrowUpdate( testEye );
        testEye +=10;
        if( testEye > 100 )
        {
            testEye = -100;
        }

        (void) UpdateLED( &oHmiLedStatus[0] );
        osDelay ( 500 );
    }

    for ( ;; )
    {
        if( true == oHmiLedUpdateRequired_f )
        {
            (void) UpdateLED( &oHmiLedStatus[0] );
        }

        LEDSelfTest();
        osDelay ( 100 );
    }
}
/*********************************************************************************
 *Name :- UpdateLED
 *Para1:- N/A
 *Return:-N/A
 *Details:-  update LED driver with new data.
 **********************************************************************************/
void InitLedSpi ( void )
{

/* SPI1 parameter configuration*/
    ledDriverSpi.Instance = SPI1;
    ledDriverSpi.Init.Mode = SPI_MODE_MASTER;
    ledDriverSpi.Init.Direction = SPI_DIRECTION_2LINES;
    ledDriverSpi.Init.DataSize = SPI_DATASIZE_8BIT;
    ledDriverSpi.Init.CLKPolarity = SPI_POLARITY_LOW;
    ledDriverSpi.Init.CLKPhase = SPI_PHASE_1EDGE;
    ledDriverSpi.Init.NSS = SPI_NSS_SOFT;
    ledDriverSpi.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
    ledDriverSpi.Init.FirstBit = SPI_FIRSTBIT_MSB;
    ledDriverSpi.Init.TIMode = SPI_TIMODE_DISABLE;
    ledDriverSpi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    ledDriverSpi.Init.CRCPolynomial = 7;
    ledDriverSpi.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    ledDriverSpi.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;

    if ( HAL_SPI_Init ( &ledDriverSpi ) != HAL_OK )
    {
        //Error_Handler ();
    }

}
/*********************************************************************************
 *Name :- UpdateLED
 *Para1:- N/A
 *Return:-N/A
 *Details:-  update LED driver with new data.
 **********************************************************************************/
bool UpdateLED ( uint8_t *data )
{
    bool retValue = false;

    HAL_GPIO_WritePin ( GPIOC, GPIO_PIN_13, GPIO_PIN_SET );        //OE;

    if( HAL_OK == HAL_SPI_Transmit ( &ledDriverSpi, data, LED_BYTE_SIZE, 0x10 ) )
    {
        retValue = true;
    }
    HAL_GPIO_WritePin ( GPIOG, GPIO_PIN_11, GPIO_PIN_SET );        //LE
    osDelay ( 1 );
    HAL_GPIO_WritePin ( GPIOG, GPIO_PIN_11, GPIO_PIN_RESET );        //LE;
    osDelay ( 1 );
    HAL_GPIO_WritePin ( GPIOC, GPIO_PIN_13, GPIO_PIN_RESET );        //OE;

    oHmiLedUpdateRequired_f = false;

    return retValue;

}
/*********************************************************************************
 *Name :- LEDSelfTest
 *Para1:- N/A
 *Return:-N/A
 *Details:-  Turn LED one By One for a second.
 **********************************************************************************/
bool LEDSelfTest ( void )
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t Bytecnt = 0;
    bool retvalue = true;

    debugText ( "\n\n\n LED BIT Wise\n\n" );

    for( j=0; j<18; j++)
    {
        for ( i = 0; i < 8; i++ )
        {
            debugTextValue ( "\tBIT", i, DECIMAL );
            oHmiLedStatus[ Bytecnt ] |= (1 << i);

            if( HAL_OK != UpdateLED ( oHmiLedStatus ) )
            {
                retvalue = false;
            }
            oHmiLedStatus[ Bytecnt ] = 0;
            osDelay ( 500 );
        }
        debugTextValue ( "\nBYTEs  :  ", Bytecnt, DECIMAL );

        if ( LED_BYTE_SIZE <= Bytecnt++ )
        {
            oHmiLedStatus[ Bytecnt - 1 ] = 0x00;
            if( HAL_OK != UpdateLED ( oHmiLedStatus ) )
            {
                retvalue = false;
            }

            Bytecnt = 0;
        }
    }

    return retvalue;
}

/*********************************************************************************
 *Name :- oHmiAllLedOff
 *Para1:- N/A
 *Return:-bool
 *Details:-  Turn Off all the LEDs in outer panel
 **********************************************************************************/
bool oHmiAllLedOff ( void )
{
    uint8_t LedData[ LED_BYTE_SIZE ] = { 0 };

    return ( UpdateLED ( LedData ) );

}
/*********************************************************************************
 *Name :- oHmiprocessUpdate
 *Para1:- eProcess
 *Return:-N/A
 *Details:-  Update Process LED on outerpanel
 **********************************************************************************/
void oHmiprocessUpdate ( enum_outer_hmi_weld_app_t process )
{

/*
2   Optr (G)    Optr(Y) Optr(Y)     N/A     Optr (Red)  SYN     MMA      Goughing
3   CRAFT       V-UP    DEEP        THIN    Speed       GMA (M) Root        Pulse
*/

    oHmiLedStatus[ PROCESSBYTE_1 ] = 0;
    oHmiLedStatus[ PROCESSBYTE_2 ] &= ~( (1<< BIT_SYNERCY) || (1<< BIT_MMA) || (1<< BIT_GOUGING) );

    switch ( process )
    {
        case OUTER_HMI_WELD_APP_MIG_MANUAL :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_MIG_MAN);
            break;
        case OUTER_HMI_WELD_APP_MIG_SYNERGY :
            oHmiLedStatus[ PROCESSBYTE_2 ] |= (BIT_SYNERCY);
            break;
        case OUTER_HMI_WELD_APP_PULSE :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_PULSE);
            break;
        case OUTER_HMI_WELD_APP_SPEED :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_SPEED);
            break;
        case OUTER_HMI_WELD_APP_DEEP :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_DEEP);
            break;
        case OUTER_HMI_WELD_APP_CRAFT :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_CRAFT);
            break;
        case OUTER_HMI_WELD_APP_V_UP :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_VUP);
            break;
        case OUTER_HMI_WELD_APP_THIN :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_THIN);
            break;
        case OUTER_HMI_WELD_APP_ROOT :
            oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_ROOT);
            break;
        case OUTER_HMI_WELD_APP_MMA :
            oHmiLedStatus[ PROCESSBYTE_2 ] |= (BIT_MMA);
            break;
        case OUTER_HMI_WELD_APP_GOUGING :
            oHmiLedStatus[ PROCESSBYTE_2 ] |= (BIT_GOUGING);
            break;
        case OUTER_HMI_WELD_APP_COMPENSATION :
            //oHmiLedStatus[ PROCESSBYTE_1 ] |= (BIT_CC_G | BIT_CC_Y);
            break;
        case OUTER_HMI_WELD_APP_INVALID :
            default:
            break;
    }

    oHmiLedUpdateRequired_f = true;

}

/*********************************************************************************
 *Name :- oHmiJobUpdate
 *Para1:- Job number
 *Return:-N/A
 *Details:-  Job LED on outerpanel
 **********************************************************************************/
void oHmiJobUpdate( uint8_t jobNo )
{

/*
0-->  Green -3,   White -3 ,      3  DOT ( G),        Green -2,   White-2,    2 DOT (G),  Green-1,    White -1
1-->  1 DOT(G),   PROCESS BUTTON, PROCESS DOT (g),    NA,         NA,         4-T,        2-T,        Optr (G)
*/

    //!< Turn OFF all Job related LEDs , before update.
    oHmiLedStatus[ JOBBYTE_1 ] = 0;
    oHmiLedStatus[ JOBBYTE_2 ] &= ~(1<< jOB_ONE_DOT) ;
    oHmiLedStatus[ JOBBYTE_1 ] |= ( ( 1<<JOB_ONE_WHITE ) || ( 1<<JOB_TWO_WHITE ) || ( 1<<JOB_THREE_WHITE ) );

    switch( jobNo )
    {
        case 1 :
            oHmiLedStatus[ JOBBYTE_1 ] |=  ( 1<<JOB_ONE_GREEN );
            oHmiLedStatus[ JOBBYTE_1 ] &= ~( 1<<JOB_ONE_WHITE );
            oHmiLedStatus[ JOBBYTE_2 ] |=  ( 1<<jOB_ONE_DOT );
            break;
        case 2 :
            oHmiLedStatus[ JOBBYTE_1 ] |=  ( ( 1<<JOB_TWO_GREEN ) || ( 1<<jOB_TWO_DOT ) );
            oHmiLedStatus[ JOBBYTE_1 ] &= ~( 1<<JOB_TWO_WHITE ) ;
            break;
        case 3 :
            oHmiLedStatus[ JOBBYTE_1 ] |=  ( ( 1<<JOB_THREE_GREEN ) || ( 1<<jOB_THREE_DOT ) );
            oHmiLedStatus[ JOBBYTE_1 ] &= ~( 1<<JOB_TWO_WHITE ) ;
            break;
        default :
        {
            //!< Don't do anything .... already default set on top.
        }
    }

    oHmiLedUpdateRequired_f = true;

}
/*********************************************************************************
 *Name :- oHmiRightSegmentUpdate
 *Para1:- value to display
 *Para2:- unit LED need to update
 *Return:-N/A
 *Details:-  Right side 3 digit 7-segment update on outerpanel
 **********************************************************************************/
void oHmiRightSegmentUpdate( int16_t value, uint8_t units )
{
    int16_t localTemp = value;
    uint8_t digit[3] = { 0 };
    int8_t mod = 0;
    uint8_t i = 0;

    if( ( 1000 > localTemp ) )
    {

        oHmiLedStatus[ RIGHT_SEGMENT_BYTE1 ] = 0;
        oHmiLedStatus[ RIGHT_SEGMENT_BYTE2 ] = 0;
        oHmiLedStatus[ RIGHT_SEGMENT_BYTE3 ] = 0;

        while( localTemp > 0 )
        {
            mod = localTemp % 10;
            digit[i++] = mod;
            localTemp = localTemp/10;
        }

        oHmiLedStatus[ RIGHT_SEGMENT_BYTE1 ] = decode7segment[ digit[0] ];

        if( ( 0 != digit[2] ) || ( 0 != digit[1] ) )   //!< if we have to update 2nd or 3rd digit
        {
            oHmiLedStatus[ RIGHT_SEGMENT_BYTE2 ] = decode7segment[ digit[1] ];

            if( 0 != digit[2] ) //!< if we have to update 3rd digit
            {
                oHmiLedStatus[ RIGHT_SEGMENT_BYTE3 ] = decode7segment[ digit[2] ];
            }
        }

        oHmiLedUpdateRequired_f = true;
    }

}
/*********************************************************************************
 *Name :- oHmiLeftSegmentUpdate
 *Para1:- value to display
 *Para2:- unit LED need to update
 *Return:-N/A
 *Details:-  Left side 3 digit 7-segment update on outerpanel
 **********************************************************************************/
void oHmiLeftSegmentUpdate( int16_t value, uint8_t units )
{
    int16_t localTemp = value;
    uint8_t digit[3] = { 0 };
    int8_t mod = 0;
    uint8_t i = 0;

    if( ( 1000 > localTemp ) )
    {

        oHmiLedStatus[ LEFT_SEGMENT_BYTE1 ] = 0;
        oHmiLedStatus[ LEFT_SEGMENT_BYTE2 ] = 0;
        oHmiLedStatus[ LEFT_SEGMENT_BYTE3 ] = 0;

        while( localTemp > 0 )
        {
            mod = localTemp % 10;
            digit[i++] = mod;
            localTemp = localTemp/10;
        }

        oHmiLedStatus[ LEFT_SEGMENT_BYTE1 ] = decode7segment[ digit[0] ];

        if( ( 0 != digit[2] ) || ( 0 != digit[1] ) )   //!< if we have to update 2nd or 3rd digit
        {
            oHmiLedStatus[ LEFT_SEGMENT_BYTE2 ] = decode7segment[ digit[1] ];

            if( 0 != digit[2] ) //!< if we have to update 3rd digit
            {
                oHmiLedStatus[ LEFT_SEGMENT_BYTE3 ] = decode7segment[ digit[2] ];
            }
        }

        oHmiLedUpdateRequired_f = true;
    }

}
/*********************************************************************************
 *Name :- oHmiRightEyeBrowUpdate
 *Para1:- percent to show in bars
 *Return:-N/A
 *Details:-  Eyebrow LED bars control.
 **********************************************************************************/
void oHmiRightEyeBrowUpdate( int8_t percent )
{
    if( ( percent >= -100 ) || ( percent <= 100 ) )
    {
        oHmiLedStatus[ RIGHT_EYEBROW_BYTE1 ] &= (uint8_t) ~(RIGHT_EYEBROW_MASK_ALL1);
        oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] &= (uint8_t) ~(RIGHT_EYEBROW_MASK_ALL2);

        if( 0 < percent  )  //!< Positive Side
        {
            if( ( percent > 0 ) && ( percent < 10 ) )
            {
               oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_CTRW ;

            }else if( ( percent >= 10 ) && ( percent < 20 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2] |= RIGHT_EYEBROW_MASK_CTRG ;

            }else if( ( percent >= 20 ) && ( percent < 40 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_20 ;

            }else if( ( percent >= 40 ) && ( percent < 60 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE1 ] |= RIGHT_EYEBROW_MASK_40 ;

            }else if( ( percent >= 60 ) && ( percent < 80 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE1 ] |= RIGHT_EYEBROW_MASK_60 ;

            }else if( ( percent >= 80 ) && ( percent < 100 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE1 ] |= RIGHT_EYEBROW_MASK_80 ;

            }else if( percent == 100 )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE1 ] |= RIGHT_EYEBROW_MASK_100 ;
            }

        }else  //!< Negative side
        {
            if( ( percent < 0 ) && ( percent >= -10 ) )
            {
               oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_CTRW ;

            }else if( ( percent < -10 ) && ( percent > -20 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_CTRG ;

            }else if( ( percent <= -20 ) && ( percent > -40 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_N20 ;

            }else if( ( percent <= -40 ) && ( percent >-60 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_N40 ;

            }else if( ( percent <= -60 ) && ( percent > -80 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_N60 ;

            }else if( ( percent <= -80 ) && ( percent > -100 ) )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_N80 ;

            }else if( percent == -100 )
            {
                oHmiLedStatus[ RIGHT_EYEBROW_BYTE2 ] |= RIGHT_EYEBROW_MASK_N100 ;
            }

        }

        oHmiLedUpdateRequired_f = true;

    }
}

/*********************************************************************************
 *Name :- oHmiLeftEyeBrowUpdate
 *Para1:- percent to show in bars
 *Return:-N/A
 *Details:-  Eyebrow LED bars control.
 **********************************************************************************/
void oHmiLeftEyeBrowUpdate( int8_t percent )
{
    if( ( percent >= -100 ) || ( percent <= 100 ) )
    {
        oHmiLedStatus[ LEFT_EYEBROW_BYTE1 ] &= (uint8_t) ~(LEFT_EYEBROW_MASK_ALL1);
        oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] &= (uint8_t) ~(LEFT_EYEBROW_MASK_ALL2);

        if( 0 < percent  )  //!< Positive Side
        {
            if( ( percent > 0 ) && ( percent < 10 ) )
            {
               oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_CTRW ;

            }else if( ( percent >= 10 ) && ( percent < 20 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2] |= LEFT_EYEBROW_MASK_CTRG ;

            }else if( ( percent >= 20 ) && ( percent < 40 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_20 ;

            }else if( ( percent >= 40 ) && ( percent < 60 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE1 ] |= LEFT_EYEBROW_MASK_40 ;

            }else if( ( percent >= 60 ) && ( percent < 80 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE1 ] |= LEFT_EYEBROW_MASK_60 ;

            }else if( ( percent >= 80 ) && ( percent < 100 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE1 ] |= LEFT_EYEBROW_MASK_80 ;

            }else
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE1 ] |= LEFT_EYEBROW_MASK_100 ;
            }

        }else  //!< Negative side
        {
            if( ( percent < 0 ) && ( percent >= -10 ) )
            {
               oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_CTRW ;

            }else if( ( percent < -10 ) && ( percent > -20 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_CTRG ;

            }else if( ( percent <= -20 ) && ( percent > -40 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_N20 ;

            }else if( ( percent <= -40 ) && ( percent >-60 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_N40 ;

            }else if( ( percent <= -60 ) && ( percent > -80 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_N60 ;

            }else if( ( percent <= -80 ) && ( percent > -100 ) )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_N80 ;

            }else if ( -100 == percent )
            {
                oHmiLedStatus[ LEFT_EYEBROW_BYTE2 ] |= LEFT_EYEBROW_MASK_N100 ;
            }

        }

        oHmiLedUpdateRequired_f = true;

    }
}

