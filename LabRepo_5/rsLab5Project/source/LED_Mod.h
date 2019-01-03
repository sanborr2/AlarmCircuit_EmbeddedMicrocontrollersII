/*****************************************************************************
 * EE344, rsLab5Project, LED_Mod
*   Program handles the LED States of the Security System
*
* Robert Sanborn, 11/30/2018
*
 ****************************************************************************/

#ifndef LED_MOD_H_
#define LED_MOD_H_

#define NO_DECTECTION               0U  //>>>multiple public defines with same name -1bp
#define SECURITY_BREACH_E1          GPIO_PIN(LED8_BIT)
#define SECURITY_BREACH_E2          GPIO_PIN(LED9_BIT)
#define SECURITY_BREACH_E12         GPIO_PIN(LED8_BIT) | GPIO_PIN(LED9_BIT)

/***********************************************************************************
 * LEDInit()        Initializes Watchdog timer so that program does not reset
 *                      must be called at least every 2 seconds upon initialization
 *                      Watchdog module.
 *
 * Return Value:        none
 * Arguments:           none
 *************************************************************************************/
void LEDInit(void);

/**********************************************************************************
* LEDTask               public function that controls LEDs D8 and D9 for alarm state
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void LEDTask(void);




#endif /* LED_MOD_H_ */
