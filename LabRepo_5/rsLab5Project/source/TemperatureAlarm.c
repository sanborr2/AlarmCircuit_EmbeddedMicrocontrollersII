/****************************************************************************************
* EE344, rsLab5Project, TemperatureAlarm
*   Program handles PIT1 Timer and ADC Peripherals of Security System
*   for Temperature Alarm Module
*
* Robert Sanborn, Nov 25, 2018
*
****************************************************************************************/
#include <AlarmWave.h>
#include "MCUType.h"
#include "TemperatureAlarm.h"       /*          Include project header file         */
#include "Lab5Main.h"
#include "Touch_Sensor_Mod.h"


#define CNTDN_DACSPS            29999999U
#define SAMPLES_PER_S           2U

#define PIT1_TRIG               5U
#define SEL_DADP3               3U
#define ADC_CLK_DIV             3U
#define ADC_BITCONV             3U

#define ADC_32SAMPS_AVGE        3U

#define MSB16                   0x8000U


#define DC_OFFSET_PT4V          (INT32S)26214U
#define TEMP_TO_VOLT_RATIO      (INT32S)1278U

#define INITIAL_TEMP            (INT32S)20000U

/**/
#define LCD_NEGATIVE          (INT8C)0x2DU
#define LCD_DEGREE            (INT8C)0xDFU
#define LCD_DECIMAL_PT        (INT8C)0x2EU
#define LCD_SPACE             (INT8C)0x20U
#define LCD_ZERO              (INT8C)0x30U

#define LCD_C                 (INT8C)0x43U
#define LCD_F                 (INT8C)0x46U

#define HUNDREDS_PLACE(x)       (INT8C)( 48U + (x/100000U) % 10U )
#define TENS_PLACE(x)           (INT8C)( 48U +  (x/10000U) % 10U )
#define ONES_PLACE(x)           (INT8C)( 48U +   (x/1000U) % 10U )
#define TENTHS_PLACE(x)         (INT8C)( 48U +    (x/100U) % 10U )
#define HUNDREDTHS_PLACE(x)     (INT8C)( 48U +     (x/10U) % 10U )

#define ABS_VAL(t) (((t) >= 0) ? (t) : -(t))



static INT32S CurrTempC = INITIAL_TEMP;



/**********************************************************************************
* TA_GetTempC()
*
* Description:  returns calculated value of current temperature of environment
*               via the ADC0 and PIT0 peripherals
*
* Return Value: temp, the value of currtempc in millidegrees Celsius
*
* Arguments:    none
*************************************************************************************/
INT32S TA_GetTempC(void){
    INT32S temp = CurrTempC;
    return temp;
}

/**********************************************************************************
* ADC0_IRQHandler()
*
* Description:  utilizes PIT1 timer for measuring 0.5 second time periods
*               and ADC0 to receive input from temperature sensor where the equation
*               used is Vin = (19.5mV/C)*(Temp) + 400mV
*
* Return Value: none
* Arguments:    none
**********************************************************************************/
void ADC0_IRQHandler(void);


/**********************************************************************************
* TA_FormatTemp()
*
* Description:  returns calculated value of current temperature of environment
*               formatted in an LCD string, size, note that valid range of
*               temperatures that can be formatted by this function is
*                -99 to 999 for either temperature range unit
*                Note also that function rounds to the nearest whole number,
*                if negative round down, if positive round up
*
* Return Value: none
*
* Arguments:    temp, the value of the current temperature in millidegrees Celsius
*               temper_pointer, pointer to LCD string of temperature
*               c_or_f, boolean variable where
*               0 ->    celsius
*               1 ->    fahrenheit
*
*************************************************************************************/
void TA_FormatTemp(INT32S temp, INT8U c_or_f, INT8C *const temper_pointer){
    INT32S temp_x = temp;
    INT8U units = c_or_f;

    /*Boolean values for determining characters of the string*/
    INT8U sign = 0U;    /*0 denotes positive value*/
    INT8U notens = 0U;

    INT8C *t_pointer = temper_pointer;


    /*If Fahrenheit needed, change units to millidegrees Fahrenheit*/
    if(units){
        temp_x = (( (temp_x)*((INT32S)18U) ) /((INT32S)10U) ) + 32000U;
    } else{}

    sign = ((INT8U)(temp_x >> 31U)) & 0x1U; /*Detect Sign of temperature*/

    /* Round to the Nearest Whole Number, round upward */
    if ( ( TENTHS_PLACE( ABS_VAL(temp_x) ) > (INT8C)0x34U )) {
        if (sign){/*negative case*/
            temp_x = temp_x - ((INT32S)1000U);
        } else {/* positive case*/
            temp_x = temp_x + ((INT32S)1000U);
        }
    } else{}

    /*If sign is negative, add negative sign to string*/
    if (sign){
        *t_pointer = LCD_NEGATIVE;
        t_pointer++;
        temp_x = -temp_x;


        /*Check if there is a hundreds place*/
    } else if( temp_x >= ((INT32S)100000U) ) {

        *t_pointer = HUNDREDS_PLACE(temp_x);
        t_pointer++;
    } else{
        *t_pointer = LCD_SPACE;
        t_pointer++;
    }

    if ((TENS_PLACE(temp_x) != LCD_ZERO) || (HUNDREDS_PLACE(temp_x) != LCD_ZERO)){
        *t_pointer = TENS_PLACE(temp_x);
         t_pointer++;
    } else {
        notens = 1U;
    }


    *t_pointer = ONES_PLACE(temp_x);
    t_pointer++;

    /*Input units*/
    *t_pointer = LCD_DEGREE;
    t_pointer++;

    if(units){
        *t_pointer = LCD_F;
        t_pointer++;
    } else{
        *t_pointer = LCD_C;
        t_pointer++;
    }

    /* If no tens, overwrite leftover spot */
    if(notens){
        *t_pointer = ' ';
        t_pointer++;
    } else{}


}






/**********************************************************************************
* TempAlarmInit()
*
* Description:  initializes PIT1 timer and ADC0 for temperature alarm for
*               security system along with calibrating the ADC0
*
* Return Value: none
* Arguments:    none
**********************************************************************************/
void TempAlarmInit(void){
    INT16U calib_adc;
    INT16U cnt = 0;

    /*Initialize ADC0 */
    SIM->SCGC6 |= SIM_SCGC6_ADC0(1U);


    ADC0->SC3 |= ADC_SC3_CAL_MASK;
    while((ADC0->SC1[0] & ADC_SC1_COCO_MASK) == 0U){}

    /*Begin Plus Side Calibration*/ //>>>+1ex
    calib_adc = (INT16U) ( (ADC0->CLP0 & 0x3FU) + (ADC0->CLP0 & 0x3FU)
                + (ADC0->CLP2 & 0x3FU) + (ADC0->CLP3 & 0x3FU)
                + (ADC0->CLP4 & 0x3FU) + (ADC0->CLPS & 0x3FU));
    calib_adc >>= 1U;
    calib_adc |= MSB16;
    ADC0 -> PG = (INT32U)calib_adc;

    /*Begin Minus Side Calibration*/
    calib_adc = (INT16U)( (ADC0->CLM0 & 0x3FU) + (ADC0->CLM0 & 0x3FU)
                        + (ADC0->CLM2 & 0x3FU) + (ADC0->CLM3 & 0x3FU)
                        + (ADC0->CLM4 & 0x3FU) + (ADC0->CLMS & 0x3FU));
    calib_adc >>= 1U;
    calib_adc |= MSB16;
    ADC0 -> MG = (INT32U)calib_adc;

    while ( ((ADC0->SC3 & ADC_SC3_CALF_MASK) == ADC_SC3_CALF_MASK) && (cnt <1000U)){
        /*Wait for Calibration to end or give up after a thousand tries*/

        ADC0->SC3 |= ADC_SC3_CAL_MASK;
        while((ADC0->SC1[0] & ADC_SC1_COCO_MASK) == 0U){}

        /*Begin Plus Side Calibration*/
        calib_adc = (INT16U) ( (ADC0->CLP0 & 0x3FU) + (ADC0->CLP0 & 0x3FU)
                    + (ADC0->CLP2 & 0x3FU) + (ADC0->CLP3 & 0x3FU)
                    + (ADC0->CLP4 & 0x3FU) + (ADC0->CLPS & 0x3FU));
        calib_adc >>= 1U;
        calib_adc |= MSB16;
        ADC0 -> PG = (INT32U)calib_adc;


        /*Begin Minus Side Calibration*/
        calib_adc = (INT16U)( (ADC0->CLM0 & 0x3FU) + (ADC0->CLM0 & 0x3FU)
                            + (ADC0->CLM2 & 0x3FU) + (ADC0->CLM3 & 0x3FU)
                            + (ADC0->CLM4 & 0x3FU) + (ADC0->CLMS & 0x3FU));
        calib_adc >>= 1U;
        calib_adc |= MSB16;
        ADC0 -> MG = (INT32U)calib_adc;


        ADC0->SC3 |= ADC_SC3_CAL_MASK;
        /*Check COCO Flag is Calibration Process is commplete*/
        while((ADC0->SC1[0] & ADC_SC1_COCO_MASK) == 0U){}

        cnt++;
    }

    /*Set Trigger to PIT1*/
    SIM->SOPT7 |= SIM_SOPT7_ADC0TRGSEL(PIT1_TRIG)| SIM_SOPT7_ADC0ALTTRGEN(1U);

    /*Single ended input mode using channel AD4, start initial conversion process */
    ADC0->SC1[0] = ADC_SC1_DIFF(0U)| ADC_SC1_ADCH(SEL_DADP3) |ADC_SC1_AIEN(1U);
    ADC0->CFG1 = ADC_CFG1_ADIV(ADC_CLK_DIV)|ADC_CFG1_MODE(ADC_BITCONV)
                    |ADC_CFG1_ADLSMP_MASK;


    ADC0->SC3 |= ADC_SC3_ADCO(0U)|ADC_SC3_AVGE(1U)|ADC_SC3_AVGS(ADC_32SAMPS_AVGE);
    ADC0->SC2 = ADC_SC2_ADTRG(1U);

    NVIC_ClearPendingIRQ(ADC0_IRQn);
    NVIC_EnableIRQ(ADC0_IRQn);


    /*Initialize the PIT for the Temperature Alarm*/
    SIM->SCGC6 |= SIM_SCGC6_PIT(1U);
    PIT->MCR &= PIT_MCR_MDIS(0);     /*Start with standard timer enabled*/
    PIT->MCR |=PIT_MCR_FRZ(1U);

    PIT->CHANNEL[1].LDVAL = CNTDN_DACSPS;
    PIT->CHANNEL[1].TCTRL = (PIT_TCTRL_TIE(1U)|PIT_TCTRL_TEN(1U));

}

/**********************************************************************************
* ADC0_IRQHandler()
*
* Description:  utilizes PIT1 timer for measuring 0.5 second time periods
*               and ADC0 to receive input from temperature sensor  where the equation used is
*               Vin = (19.5mV/C)*(Temp) + 400mV
*
* Return Value: none
* Arguments:    none
**********************************************************************************/
void ADC0_IRQHandler(void){
    INT32S rawdata;
    INT32S qvolts;

    rawdata = ((INT32S)(ADC0->R[0])  & 0xFFFF); //>>>Better to do these calculations outside ISR
    qvolts = (rawdata * 33) / 10; /*Voltage input, units V, now in *Q16* notation*/
    /*signal reverted back from voltage into milli Celsius degrees*/
    CurrTempC = ((INT32S)(qvolts - DC_OFFSET_PT4V) * (INT32S)1000U)/TEMP_TO_VOLT_RATIO;
    //>>>Loss of data...divide before multiple -1req
}
