/****************************************************************************************
* EE344, rsLab5Project, Lab5Main
*   Program for security system that detects if touch sensor channels 11 and 12
*   have been activated, if the enviroment's temperature exits the range
*   40 to 0 degrees Celsius, and if so an alarm of harmonics
*   300Hz, 600Hz, 1200Hz, 2400 Hz, and 3000Hz is generated.
*   Note also that the program has a WatchDog with a timeout of 2 seconds
*
* Robert Sanborn, 11/15/2018
*
****************************************************************************************/

#ifndef LAB5MAIN_H_
#define LAB5MAIN_H_

/* Define Constants for denoting Alarm State */

#define ARMED           (INT8C)(0x11U)

#define DISARMED        (INT8C)(0x14U)

#define ALARM                      0U

#define ALARM_TAMPERED  (INT8C)(0x13U)


#include "K65TWR_ClkCfg.h"
#include "K65TWR_GPIO.h"
#include "Key.h"
#include "LCD.h"
#include "SysTickDelay.h"

/**********************************************************************************
* Public Function Prototypes
**********************************************************************************/


/**********************************************************************************
* L5MAlarmState    public function that returns the program state
*
* Return Value: lrmstate, the status of the program and thus the alarm
*
* Arguments:    state, the status of the program and thus the alarm
**********************************************************************************/
INT8C L5MAlarmState(void);


/**********************************************************************************
* L5MAlarmStatePrev    public function that returns the program state during the most
*                   previous timeslice
*
* Return Value:     status, the status of the program and thus the alarm during the most
*                   previous timeslice
*
* Arguments:        none
**********************************************************************************/
INT8C L5MAlarmStatePrev(void);

#endif /* LAB5MAIN_H_ */
