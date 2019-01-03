/*****************************************************************************
* EE344, rsLab5Project, Lab5Main
*   Program for security system that detects if touch sensor channels 11 and 12
*   have been activated, if the enviroment's temperature exits the range
*   40 to 0 degrees Celsius, and if so an alarm of harmonics
*   300Hz, 600Hz, 1200Hz, 2400 Hz, and 3000Hz is generated.
*   Note also that the program has a WatchDog with a timeout of 20 milliseconds
*
* Robert Sanborn, 11/19/2018
*
* Program Completed: 12/7/18
*
*****************************************************************************/
#include "MCUType.h"            /*    Include project header file           */
#include "Lab5Main.h"
#include "LED_Mod.h"
#include "Touch_Sensor_Mod.h"
#include "AlarmWave.h"
#include "TemperatureAlarm.h"
#include "WDogMod.h"
#include "MMA8451Q.h"
#include "Accelerometer.h"


/* Define Constants for denoting state of Keyboard input*/
#define A_KEY         (INT8C)(0x11U)
#define B_KEY         (INT8C)(0x12U)
#define C_KEY         (INT8C)(0x13U)
#define D_KEY         (INT8C)(0x14U)

#define ZERO_ADDR       0x00000000U
#define HIGH_ADDR       0x001FFFFFU

#define ROW_1               1U
#define ROW_2               2U

#define COL_1               1U
#define COL_2               2U
#define COL_3               3U
#define COL_6               6U
#define COL_7               6U
#define COL_11             11U
#define COL_12             12U
#define COL_13             13U
#define COL_16             16U

#define LCD_W                   (INT8C)0x57U
#define LCD_T                   (INT8C)0x54U
#define LCD_SPACE               (INT8C)0x20U


#define CURSOR_ON           1U
#define CURSOR_BLINK        0U


/*Disables Temperature Alarm but not Temperature Detector*/


/**********************************************************************************
* Private Function Prototypes
**********************************************************************************/

/**********************************************************************************
* ControlDisplayTask    public function that controls the state of the alarm
*                       based on keypad inputs and display to the LCD display.
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
static void ControlDisplayTask(void);

/**********************************************************************************
* l5mCalcChkSum() -ascertains checksum for memory block indicated by starting and
*               ending addresses
*
* Description:  Loads every byte from starting address to ending address
*               and takes the first 16 bits of the sum of all said bytes.
*               Note that it if starting address is greater than ending address
*               function will only return byte stored at ending address.
*
* Return Value: c_sum, 16 bit unsigned integer, the first 16 bits of the sum
*               of all bytes between the two addresses
*
* Arguments:    startaddr is pointer to the initial address of the memory block
*               endaddr is the pointer to the last address of the memory block
**********************************************************************************/
static INT16U l5mCalcChkSum(INT8U *startaddr, INT8U *endaddr);



/**********************************************************************************
* Private Globals
**********************************************************************************/
static INT8C AlarmStatus;
static INT8C PrevAlarmStatus;


/**********************************************************************************
* Private Strings
**********************************************************************************/
static const INT8C ArmedNote[]              = {"ARMED     "};
static const INT8C DisarmedNote[]           = {"DISARMED  "};
static const INT8C AlarmNote[]              = {"ALARM     "};

static const INT8C TempAlarmNote[]          = {"TEMP ALARM"};



/**********************************************************************************
* Program
**********************************************************************************/

void main(void){
    INT16U chksum = l5mCalcChkSum( ((INT8U *)ZERO_ADDR), ((INT8U *)HIGH_ADDR) );    //>>>after bootclock, very slow

    /* Initialize TSI, LEDs, the Audio Alarm Module
     * the Temperature Alarm Module, and various other peripherals
     * for the basis of the security system*/
    K65TWR_BootClock();
    KeyInit();
    LcdInit();
    (void)SysTickDlyInit();
    TempAlarmInit();
    AlarmWaveInit();
    LEDInit();
    TSCalib();
    GpioDBugBitsInit();

    /* Output check sum to LCD in the form XXXX, in hexadecimal, at Row 1, Column 13*/
    LcdCursor(CURSOR_ON, CURSOR_BLINK);
    LcdMoveCursor(ROW_2, COL_1);
    LcdDispByte((INT8U)(chksum >> 8U));
    LcdDispByte((INT8U)(chksum & 0xFFFF));

    /*Output ARMED Status on LCD to indicate the Initial State is ARMED*/
    AlarmStatus = A_KEY;
    LcdMoveCursor(ROW_1, COL_1);
    LcdDispStrg(ArmedNote);

    /*Additional Modules, namely MMA8451 Accelerometer and Watchdog Module*/
    I2CInit();

    Accelerometer_Off_Calc();


    /*Check to see if WatchDog was triggered*/
    /*If watchdog reset occurred, display W at row 2, column 12*/
    if ( (RCM->SRS0 & RCM_SRS0_WDOG_MASK) == RCM_SRS0_WDOG_MASK) {
        LcdMoveCursor(ROW_2, COL_6);
        LcdDispChar(LCD_W);
    } else {}

    /*Initialize the WatchDog*/
    WDogInit();

    /* Timeslice Scheduler of the Program */
    while (1U){
        SysTickWaitEvent(10U);
        WDogRefreshTask();
        KeyTask();
        ControlDisplayTask();
        SensorTask();
        LEDTask();
        AlarmWaveControlTask();
        AccelerometerTask();
    }
}


/**********************************************************************************
* l5mCalcChkSum() -ascertains checksum for memory block indicated by starting and
*               ending addresses
*
* Description:  Loads every byte from starting address to ending address
*               and takes the first 16 bits of the sum of all said bytes.
*               Note that it if starting address is greater than ending address
*               function will only return byte stored at ending address.
*
* Return Value: c_sum, 16 bit unsigned integer, the first 16 bits of the sum
*               of all bytes between the two addresses
*
* Arguments:    startaddr is pointer to the initial address of the memory block
*               endaddr is the pointer to the last address of the memory block
**********************************************************************************/
static INT16U l5mCalcChkSum(INT8U *startaddr, INT8U *endaddr){
    INT16U c_sum = 0U;
    INT8U membyte;
    INT8U *addr = startaddr;

    /*Iterate over all memory locations from start address to address
     * right before ending address and sum up all bytes*/
    while(addr < endaddr){
        membyte = *addr;
        addr++;
        c_sum += (INT16U)membyte;
    }
    c_sum += ( (INT16U)(*endaddr) );
    return c_sum;
}


/**********************************************************************************
* L5MAlarmState    public function that returns the program state
*
* Return Value: status, the status of the program and thus the alarm
*
* Arguments:    none
**********************************************************************************/
INT8C L5MAlarmState(void){
    INT8C status = AlarmStatus;
    return status;
}

/**********************************************************************************
* L5MAlarmStatePrev    public function that returns the program state during the most
*               previous timeslice
*
* Return Value: status, the status of the program and thus the alarm during the most
*               previous timeslice
*
* Arguments:    none
**********************************************************************************/
INT8C L5MAlarmStatePrev(void){
    INT8C st = PrevAlarmStatus;
    return st;
}
/**********************************************************************************
* ControlDisplayTask    public function that controls the state of the alarm
*                       based on keypad inputs and display to the LCD display.
*                       If KeyGet returns the value for the D key on the keypad,
*                       the LCD will display ALARM OFF. If KeyGet returns
*                       the value for the A key on the keypad, the LCD will
*                       display ALARM ON.
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
static void ControlDisplayTask(void){
    INT8C keyvalue;
    static INT8C InTemp[6U] = {"20°C"};
    static INT32S curr_reading;
    static INT32S prev_reading = 0xFFFFFFFF; /*Nonsensical initial value*/ //>>>wouldn't -1 be an acceptable reading?
    static INT8U in_fahren = 0U;    /* 0U means display Celsius,
                                     * 1U means display Fahrenheit */
    static INT8U alarm_tampered= FALSE;

    DB1_TURN_ON();

    PrevAlarmStatus = AlarmStatus;
    /*Critical Zone*/
    __disable_irq();    //>>>ISR disabled too long for no reason -1bp


    keyvalue = KeyGet();
    curr_reading = TA_GetTempC();

    /*Do not alter temperature reading unless difference detected, which should
     * not occur for 0.5 seconds*/
    if (prev_reading != curr_reading){
        TA_FormatTemp( curr_reading, in_fahren, InTemp);
        LcdMoveCursor(ROW_2, COL_12);
        LcdDispStrg(InTemp);
        prev_reading = curr_reading;
    } else{}

    /*Do not alter display unless valid key press detected*/
    /*If KeyGet returns the value for the D key, display DISARMED.*/
    if ((keyvalue == D_KEY) && (AlarmStatus != DISARMED)){
        LcdMoveCursor(ROW_1, COL_1);
        LcdDispStrg(DisarmedNote);
        AlarmStatus = DISARMED;

    /*If KeyGet returns the value for the A key, and the Alarm has not been triggered,
     *  display ARMED */
    }else if ( (keyvalue == A_KEY) && (AlarmStatus == DISARMED) ){
        LcdMoveCursor(ROW_1, COL_1);
        LcdDispStrg(ArmedNote);
        AlarmStatus = ARMED;

    } else if (TS_SensorStatus() && (AlarmStatus == ARMED ) ) {
        LcdMoveCursor(ROW_1, COL_1);
        LcdDispStrg(AlarmNote);
        AlarmStatus = ALARM;

        /*Check if temperature is out of desired range enter
         * alarm mode if system is armed*/
    } else if ( (AlarmStatus == ARMED) &&
            ((curr_reading > (INT32S)40000U ) || (curr_reading < (INT32S)0U)) ) {
        LcdMoveCursor(ROW_1, COL_1);
        LcdDispStrg(TempAlarmNote);
        AlarmStatus = ALARM;

        /* Change units of Temperature when B key is pressed*/
    } else if (keyvalue == B_KEY){
        if(in_fahren){/*Display Temperature in F*/
            in_fahren = 0U;
        } else{/*Display Temperature in C*/
            in_fahren = 1U;
        }

        /* If alarm is tampered with while armed, enter Alarm Mode*/
    } else if ((Accel_Tampering() == TRUE)){
        LcdMoveCursor(ROW_2, COL_7);
        LcdDispChar(LCD_T);
        alarm_tampered = TRUE;

        if (AlarmStatus == ARMED){
            LcdMoveCursor(ROW_1, COL_1);
            LcdDispStrg(AlarmNote);
            AlarmStatus = ALARM;
        } else{}

    }else if ( (keyvalue == C_KEY) && alarm_tampered ){
        LcdMoveCursor(ROW_2, COL_7);
        LcdDispChar(LCD_SPACE);
        alarm_tampered = FALSE;

    }else{}

    __enable_irq();

    DB1_TURN_OFF();

}
