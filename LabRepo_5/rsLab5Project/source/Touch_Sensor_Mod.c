/******************************************************************************
 * EE344, rsLab5Project, Touch_Sensor_Mod
*   Program handles the Touch Sensor Inputs of the Security System
*
* Robert Sanborn, 10/30/2018
*
 ****************************************************************************/


#include "MCUType.h"            /*    Include project header file           */
#include "Lab5Main.h"
#include "LED_Mod.h"
#include "Touch_Sensor_Mod.h"
#include "AlarmWave.h"
#include "TemperatureAlarm.h"
#include "WDogMod.h"
#include "MMA8451Q.h"



#define ELECT1                   0U   /* Electrodes */
#define ELECT2                   1U

#define E1_TOUCH_OFFSET         2000U   /* Touch Offsets for Electrodes */
#define E2_TOUCH_OFFSET         2000U


static INT32U SensorState = 0U;
static INT16U TSIBaseLvls[2U];
static INT16U TSITouchLvls[2U];



/**********************************************************************************
* TSCalib()            public function that initializes and calibrates the TSI for K65FTWR
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void TSCalib(void){
    /*Initialize Touch Screen Inputs */
    SIM->SCGC5 |= SIM_SCGC5_TSI(1U) | SIM_SCGC5_PORTB(1U);

    PORTB->PCR[18] &= PORT_PCR_MUX(0U); /* Set PIN MUXes to ALT0 for Channels 12 and 11*/
    PORTB->PCR[19] &= PORT_PCR_MUX(0U);

    /*Configure and enable TSI_GENCS to recommended settings, Kinetis TSI Mod Canvas Doc*/
    TSI0->GENCS |= TSI_GENCS_REFCHRG(5U)|TSI_GENCS_DVOLT(1U);
    TSI0->GENCS |= TSI_GENCS_EXTCHRG(5U)|TSI_GENCS_PS(5U)|TSI_GENCS_NSCN(15U);
    TSI0->GENCS &= ~TSI_GENCS_STM_MASK;
    TSI0->GENCS |= TSI_GENCS_TSIEN(1U);

    /* Calibrate Channel 12, Electrode 1*/
    TSI0->DATA = TSI_DATA_TSICH(12U);
    TSI0->DATA |= TSI_DATA_SWTS(1U);
    while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK)){} /* Start Scan*/
    TSI0->GENCS |= TSI_GENCS_EOSF(1U);
    TSIBaseLvls[ELECT1] = (INT16U)(TSI0->DATA & TSI_DATA_TSICNT_MASK); /*Set Base*/
    TSITouchLvls[ELECT1] = (INT16U)(TSIBaseLvls[ELECT1] + E1_TOUCH_OFFSET);

    /* Calibrate Channel 11, Electrode 2*/
    TSI0->DATA = TSI_DATA_TSICH(11U);
    TSI0->DATA |= TSI_DATA_SWTS(1U);
    while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK)){}
    TSI0->GENCS |= TSI_GENCS_EOSF(1U);
    TSIBaseLvls[ELECT2] = (INT16U)(TSI0->DATA & TSI_DATA_TSICNT_MASK);
    TSITouchLvls[ELECT2] = (INT16U)(TSIBaseLvls[ELECT2] + E2_TOUCH_OFFSET);
}



/**********************************************************************************
* TS_SensorStatus    public function that returns the sensor status
*
* Return Value: state, the status of the program and thus the sensor status
*
* Arguments:    none
**********************************************************************************/
INT32U TS_SensorStatus(void){
    INT32U state = SensorState;
    return state;
}

/**********************************************************************************
* SensorTask()  public function that controls Sensor
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void SensorTask(void){
    DB3_TURN_ON();
    /* If reverting back to ARMED from DISARMED, deactivate sensor levels*/

    /* Scan Channel 12, Electrode 1*/
   TSI0->DATA = TSI_DATA_TSICH(12U);
   TSI0->DATA |= TSI_DATA_SWTS(1U);
   while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK)){}
   TSI0->GENCS |= TSI_GENCS_EOSF(1U);
   /* Check for security breach in system */
   if(( (INT16U)(TSI0->DATA & TSI_DATA_TSICNT_MASK) ) > TSITouchLvls[0U]){
       SensorState |= SECURITY_BREACH_E1;
   } else{
       SensorState &= ~SECURITY_BREACH_E1;
   }

   /* Scan Channel 11, Electrode 2*/
   TSI0->DATA = TSI_DATA_TSICH(11U);
   TSI0->DATA |= TSI_DATA_SWTS(1U);
   while(!(TSI0->GENCS & TSI_GENCS_EOSF_MASK)){}
   TSI0->GENCS |= TSI_GENCS_EOSF(1U);
   /* Check for security breach in system */
   if(( (INT16U)(TSI0->DATA & TSI_DATA_TSICNT_MASK) ) > TSITouchLvls[1U]){
       SensorState |= SECURITY_BREACH_E2;
   } else{
       SensorState &= ~SECURITY_BREACH_E2;
   }

   DB3_TURN_OFF();
}
