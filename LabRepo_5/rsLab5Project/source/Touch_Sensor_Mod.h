/******************************************************************************
 * EE344, rsLab5Project, Touch_Sensor_Mod
*   Program handles the Touch Sensor Inputs of the Security System
*
* Robert Sanborn, 10/30/2018
*
 ****************************************************************************/

#ifndef TOUCH_SENSOR_MOD_H_
#define TOUCH_SENSOR_MOD_H_

#define NO_DECTECTION               0U
#define SECURITY_BREACH_E1          GPIO_PIN(LED8_BIT)
#define SECURITY_BREACH_E2          GPIO_PIN(LED9_BIT)
#define SECURITY_BREACH_E12         GPIO_PIN(LED8_BIT) | GPIO_PIN(LED9_BIT)

/**********************************************************************************
* TSCalib()            public function that initializes and calibrates the TSI for K65FTWR
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void TSCalib(void);

/**********************************************************************************
* TS_SensorStatus    public function that returns the sensor status
*
* Return Value: state, the status of the program and thus the sensor status
*
* Arguments:    none
**********************************************************************************/
INT32U TS_SensorStatus(void);



/**********************************************************************************
* SensorTask()  public function that controls Sensor
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void SensorTask(void);
#endif /* TOUCH_SENSOR_MOD_H_ */
