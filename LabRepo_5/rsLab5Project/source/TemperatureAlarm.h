/****************************************************************************************
* EE344, rsLab5Project, TemperatureAlarm
*   Program handles PIT1 Timer and ADC Peripherals of Security System
*   for Temperature Alarm Module
*
* Robert Sanborn, Nov 30, 2018
*
****************************************************************************************/
#ifndef TEMPERATUREALARM_H_
#define TEMPERATUREALARM_H_



/**********************************************************************************
* TempAlarmInit()
*
* Description:  initializes PIT1 timer and ADC0 for temperature alarm for
*               security system
*
* Return Value: none
* Arguments:    none
**********************************************************************************/
void TempAlarmInit (void);


/**********************************************************************************
* TA_FormatTemp()
*
* Description:  returns calculated value of current temperature of environment
*               formatted in an ascii string, size, note that valid range of
*               temperatures that can be formatted by this function is
*                -99.9 to 999.9 for either temperature range unit
*
* Return Value: none
*
* Arguments:    temp, the value of the current temperature in millidegrees Celsius
*               temper_pointer, pointer to ascii string of temperature
*               c_or_f, boolean variable where
*               0 ->    celsius
*               1 ->    fahrenheit
*
*************************************************************************************/
void TA_FormatTemp(INT32S temp, INT8U c_or_f, INT8C *const temper_pointer);



/**********************************************************************************
* TA_GetTempC()
*
* Description:  returns calculated value of current temperature of environment
*               via the ADC0 and PIT0 peripherals
*
* Return Value: temp, the value of CurrTempC
* Arguments:    none
*************************************************************************************/
INT32S TA_GetTempC(void);

#endif /* TEMPERATUREALARM_H_ */
