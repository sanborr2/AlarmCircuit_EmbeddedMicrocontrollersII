/*****************************************************************************
 * EE344, rsLab5Project, Accelerometer
*   Program handles the Accelerometer of the Security System
*
*   Warning: Upon initialization, the alarm device must be on the intended surface
*   and stable in order for the alarm to work properly
*
* Robert Sanborn, 11/30/2018
*
 ****************************************************************************/
#include "MCUType.h"            /*    Include project header file           */
#include "Accelerometer.h"
#include "Lab5Main.h"
#include "LED_Mod.h"
#include "Touch_Sensor_Mod.h"
#include "AlarmWave.h"
#include "TemperatureAlarm.h"
#include "WDogMod.h"
#include "MMA8451Q.h"



/*Formats output from MSB and LSB data registers for a specific axis, x, y, or z
 * as an INT16U, works well */
#define XYZ_DATA_OUT_READ(msb,lsb)     (((INT16U)(msb)) << 8U) | ((INT16U)(lsb))

#define XY_OFF(x)                                            ( ((x)/8) * (-1) )
#define Z_OFF(x)                                      ( 4096 + (x)/(8) ) * (-1)

#define ACCELERATION_VAL                7500U

#define ABS_VAL(t) (((t) >= 0) ? (t) : -(t))

static INT8U Motion_Detected = 0U;

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
INT8U Accel_Tampering(void){
    INT8U motion = Motion_Detected;
    /*Reset Detection value to 0, false, after variable has been read*/
    Motion_Detected = 0U;
    return motion;
}

/* Coordinates x, y, z for normal position*/
static INT16S Accelerometer_Base[3U];   //>>>declaration after code -1ex


/*******************************************************************************************
 * Accelerometer_Off_Calc()     Calculates the offset value of the accelerometer then
 *                              saves the new baseline values of the x, y, z data points
 *                              to the private array Accelerometer_Base[]
 *
 *  Return Value:               void
 *  Arguments:                  void
 *******************************************************************************************/
void Accelerometer_Off_Calc(void){
    INT16S x;
    INT16S y;
    INT16S z;

    /*Read from the MSB and LSB for all three coordinates */
    x = (INT16S)XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_X_MSB), MMA8451RegRd(MMA8451_OUT_X_LSB));
    y = (INT16S)XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_Y_MSB), MMA8451RegRd(MMA8451_OUT_Y_LSB));
    z = (INT16S)XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_Z_MSB), MMA8451RegRd(MMA8451_OUT_Z_LSB));

    /*Go into StandBy Mode for the MMA8451Q*/
    MMA8451RegWr(MMA8451_CTRL_REG1, 0x10U);

    /*Write the calculated offset values to the offset registers*/
    x = 0xff & (XY_OFF(x));
    y = 0xff & (XY_OFF(y));
    z = 0xff & (Z_OFF(z));
    MMA8451RegWr(MMA8451_OFF_X, (INT8U)(x) );
    MMA8451RegWr(MMA8451_OFF_Y, (INT8U)(y) );
    MMA8451RegWr(MMA8451_OFF_Z, (INT8U)(z) );

    /*Turn MMA8451 back on*/
    MMA8451RegWr(MMA8451_CTRL_REG1, 0x01U);


    /*Save the new baseline values*/
    x = XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_X_MSB),
        MMA8451RegRd(MMA8451_OUT_X_LSB));

    y = XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_Y_MSB),
        MMA8451RegRd(MMA8451_OUT_Y_LSB));

    z = XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_Z_MSB),
        MMA8451RegRd(MMA8451_OUT_Z_LSB));

    Accelerometer_Base[0U] = x;
    Accelerometer_Base[1U] = y;
    Accelerometer_Base[2U] = z;

}


/*******************************************************************************************
 * AccelerometerTask()       Calculates if the current x, y, or z data points is dangerously
 *                              too high or too low in comparison to the initial calculated
 *                              baseline values
 *
 *  Return Value:               void
 *  Arguments:                  void
 *******************************************************************************************/
void AccelerometerTask(void){
    INT16S x;
    INT16S y;
    INT16S z;

    DB7_TURN_ON();
    /* Check all three coordinates to see if alarm is accelerating and thus being
     * tampered with*/

    /*X coordinate*/
    x = XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_X_MSB),
        MMA8451RegRd(MMA8451_OUT_X_LSB));
    if ( (INT16U)(ABS_VAL(x - Accelerometer_Base[0U])) >= ACCELERATION_VAL){
        Motion_Detected = 1U;
    }else {}

    /*Y coordinate*/
    y = XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_Y_MSB),
        MMA8451RegRd(MMA8451_OUT_Y_LSB));
    if ( (INT16U)(ABS_VAL(y - Accelerometer_Base[1U]) ) >= ACCELERATION_VAL){
        Motion_Detected = 1U;
    }else {}

    /*Z coordinate*/
    z = XYZ_DATA_OUT_READ(MMA8451RegRd(MMA8451_OUT_Z_MSB),
        MMA8451RegRd(MMA8451_OUT_Z_LSB));
    if ( (INT16U)(ABS_VAL(z - Accelerometer_Base[2U])) >= ACCELERATION_VAL){
        Motion_Detected = 1U;
    }else {}


    DB7_TURN_OFF();
}
//>>>Accel +3ex
//>>>Hangs on flag at times -1ex
