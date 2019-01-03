/*****************************************************************************
 * EE344, rsLab5Project, LED_Mod
*   Program handles the LED States of the Security System
*
* Robert Sanborn, 11/30/2018
*
 ****************************************************************************/


#include "MCUType.h"            /*    Include project header file           */
#include "Lab5Main.h"
#include "Touch_Sensor_Mod.h"
#include "TemperatureAlarm.h"
#include "AlarmWave.h"
#include "LED_Mod.h"


/***********************************************************************************
 * LEDInit()             Initializes LEDs 9 and 8 *
 * Return Value:        none
 * Arguments:           none
 *************************************************************************************/
void LEDInit(void){

    /*Initialize LEDs for D8 & D9*/
     GpioLED8Init();
     GpioLED9Init();
}



/**********************************************************************************
* LEDTask               public function that controls LEDs D8 and D9 for alarm state
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void LEDTask(void){
    static INT8U timeslccnt = 0U;
    static INT8U timeslccnt1 = 0U;
    static INT8U timeslccnt2 = 0U;
    static INT32U led8_tripped = 0U;
    static INT32U led9_tripped = 0U;


    DB4_TURN_ON();

    /*  reset data for when transitioning states */
    if (L5MAlarmState() != L5MAlarmStatePrev()){
        GPIOA->PSOR |= GPIO_PIN(LED9_BIT) | GPIO_PIN(LED8_BIT);
        led8_tripped = 0U;
        led9_tripped = 0U;
    } else{}



    switch(L5MAlarmState()){

        case(ARMED):
            /* For timeslices 0 to 24, have D8 on and D9 off */
            if (timeslccnt < 25U){
                GPIOA->PCOR |= GPIO_PIN(LED8_BIT);
                GPIOA->PSOR |= GPIO_PIN(LED9_BIT);
            /* For timeslices 25 to 49, have D9 on and D8 off */
            } else if (timeslccnt < 50U){
                GPIOA->PCOR |= GPIO_PIN(LED9_BIT);
                GPIOA->PSOR |= GPIO_PIN(LED8_BIT);
            /* Reset on fiftieth timeslice*/
            } else {
                /*Force an overflow*/
                timeslccnt = 255U;
            }
            /*Update count for number of timeslices since last significant change*/
            timeslccnt++;
            break;

        /* Check Electrodes through TS_SensorStatus and Update LEDS*/
        case(DISARMED):
            timeslccnt = 0U;

            /*Update count for number of timeslices since last significant change*/
            if( SECURITY_BREACH_E1 == (TS_SensorStatus() & SECURITY_BREACH_E1) ){
                /* For timeslices 0 to 24, have D8 on depending on
                 *  which if it is activated */
                if (timeslccnt1 < 25U){
                    GPIOA->PCOR = SECURITY_BREACH_E1;
                /* For timeslices 25 to 49, have D8 off depending on
                 *  which if it is activated */
                } else if (timeslccnt1 < 50U){
                    GPIOA->PSOR = SECURITY_BREACH_E1;
                /* Reset on fiftieth timeslice*/
                } else {
                    /*Force an overflow*/
                    timeslccnt1 = 255U;
                }
                timeslccnt1++;

            } else {
                GPIOA->PSOR |= GPIO_PIN(LED8_BIT);
                timeslccnt1 = 0U;
            }


            if( SECURITY_BREACH_E2 == (TS_SensorStatus() & SECURITY_BREACH_E2) ){
                /* For timeslices 0 to 24, have D9 on depending on
                 *  which if it is activated */
                if (timeslccnt2 < 25U){
                    GPIOA->PCOR = SECURITY_BREACH_E2;
                /* For timeslices 25 to 49, have D9 off depending on
                 *  which if it is activated */
                } else if (timeslccnt2 < 50U){
                    GPIOA->PSOR = SECURITY_BREACH_E2;
                /* Reset on fiftieth timeslice*/
                } else {
                    /*Force an overflow*/
                    timeslccnt2 = 255U;
                }

                timeslccnt2++;
            } else {
                GPIOA->PSOR |= GPIO_PIN(LED9_BIT);
                timeslccnt2 = 0U;
            }
            break;

        case(ALARM):

            led8_tripped |= (TS_SensorStatus() & SECURITY_BREACH_E1);
            led9_tripped |= (TS_SensorStatus() & SECURITY_BREACH_E2);

            timeslccnt = 0U;
                /* For timeslices 0 to 4, have D8 on depending on
             *  which if it is activated */
            if (timeslccnt1 < 5U){
                GPIOA->PCOR = led8_tripped;
            /* For timeslices 5 to 9, have D8 off depending on
             *  which if it is activated */
            } else if (timeslccnt1 < 10U){
                GPIOA->PSOR = led8_tripped;
            /* Reset on tenth timeslice*/
            } else {
                /*Force an overflow*/
                timeslccnt1 = 255U;
            }

            timeslccnt1++;

            /* For timeslices 0 to 4, have D9 on depending on
             *  which if it is activated */
            if (timeslccnt2 < 5U){
                GPIOA->PCOR = led9_tripped;
            /* For timeslices 5 to 9, have D9 off depending on
             *  which if it is activated */
            } else if (timeslccnt2 < 10U){
                GPIOA->PSOR = led9_tripped;
            /* Reset on tenth timeslice*/
            } else {
                /*Force an overflow*/
                timeslccnt2 = 255U;
            }

            timeslccnt2++;

            break;

        default:  /*Do nothing for default*/
            break;
    }

    DB4_TURN_OFF();
}
//>>>All LED timing >10m off -1req
