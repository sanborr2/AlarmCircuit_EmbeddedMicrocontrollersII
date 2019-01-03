/****************************************************************************************
* EE344, rsLab5Project, AlarmState
*   Program Handles Alarm Audio of the Security System
*
* Robert Sanborn, 11/30/2018
*
****************************************************************************************/
#ifndef ALARMWAVE_H_
#define ALARMWAVE_H_


/* Defined Constants, describing state of the Sensor*/
#define NO_DECTECTION               0U
#define SECURITY_BREACH_E1          GPIO_PIN(LED8_BIT)
#define SECURITY_BREACH_E2          GPIO_PIN(LED9_BIT)
#define SECURITY_BREACH_E12         GPIO_PIN(LED8_BIT) | GPIO_PIN(LED9_BIT)


/**********************************************************************************
* Public Function Prototypes
**********************************************************************************/




/**********************************************************************************
* AlarmWaveControlTask  public function that controls the PIT and DAC
*                       to configure and control the alarm waveform timing
*                       based on the AlarmState
*
*
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void AlarmWaveControlTask(void);



/**********************************************************************************
* AlarmWaveInit         public function initializes resources for Alarm Wave
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void AlarmWaveInit(void);




#endif /* ALARMWAVE_H_ */
