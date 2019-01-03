/*****************************************************************************
 * EE344, rsLab5Project, Accelerometer
*   Program handles the Accelerometer of the Security System
*
* Robert Sanborn, 11/30/2018
*
 ****************************************************************************/


#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_




/*******************************************************************************************
 * Accelerometer_Off_Calc()     Calculates the offset value of the accelerometer then
 *                              saves the new baseline values of the x, y, z data points
 *                              to the private array Accelerometer_Base[]
 *
 *  Return Value:               void
 *  Arguments:                  void
 *******************************************************************************************/
void Accelerometer_Off_Calc(void);


/*******************************************************************************************
 * AccelerometerTask()       Calculates if the current x, y, or z data points is dangerously
 *                              too high or too low in comparison to the initial calculated
 *                              baseline values
 *
 *  Return Value:               void
 *  Arguments:                  void
 *******************************************************************************************/
void AccelerometerTask(void);


/*******************************************************************************************
 * Accel_Tampering()      Getter Function that allows other modules to read
 *                              private global variable Motion_Detected, but then resets
 *                              the global private variable to zero
 *
 *  Return Value:               motion, the current value of Motion_Detected
 *                              0 -> no motion
 *                              1 -> motion detected
 *
 *  Arguments:                  void
 *******************************************************************************************/
INT8U Accel_Tampering(void);

#endif /* ACCELEROMETER_H_ */
