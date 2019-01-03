/****************************************************************************************
* EE344, rsLab5Project, AlarmWave
*   Program handles the Audio Alarm of the Security System, with Audio Wave
*   of harmonics 300, 600, 1200, 2400, 3000 Hz
*
* Robert Sanborn, 11/30/2018
*
****************************************************************************************/




#include "MCUType.h"               /* Include project header file                      */
#include "Lab5Main.h"
#include "AlarmWave.h"
#include "Touch_Sensor_Mod.h"

#define CNTDOWN_DACSPS          3124U
#define SAMPLES_PER_HS           9600U

#define DAC_CNTRL_SETUP            0xE0U
#define DAC_HALF_SCALE_L           0x00U
#define DAC_HALF_SCALE_H           0x08U


#define WAVE_DMA_OUT_CH                  0U
#define WAVE_SAMPLES_PER_BLOCK          64
#define WAVE_BYTES_PER_SAMPLE            2
#define ALARM_SAMPLES_PER_BLOCK         64
#define SIZE_CODE_16BIT                 1


/* Sine Wave Lookup table of values in Q12 notation */
static const INT16U SineWave[64U] = {
        2048,
        2674,
        3204,
        3558,
        3691,
        3598,
        3313,
        2904,
        2457,
        2060,
        1785,
        1676,
        1740,
        1950,
        2254,
        2583,
        2867,
        3050,
        3099,
        3009,
        2799,
        2508,
        2187,
        1884,
        1638,
        1473,
        1397,
        1403,
        1474,
        1590,
        1732,
        1887,
        2048,
        2208,
        2363,
        2505,
        2621,
        2692,
        2698,
        2622,
        2457,
        2211,
        1908,
        1587,
        1296,
        1086,
        996,
        1045,
        1228,
        1512,
        1841,
        2145,
        2355,
        2419,
        2310,
        2035,
        1638,
        1191,
        782,
        497,
        404,
        537,
        891,
        1421

};



/**********************************************************************************
* AlarmWaveInit         public function initializes resources for the K65F_TWR,
*                   note that clk signal is assumed to be 180MHz
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void AlarmWaveInit(void){

    SIM->SCGC6 |= SIM_SCGC6_DMAMUX(1U);
    SIM->SCGC7 |= SIM_SCGC7_DMA(1U);
    DMAMUX->CHCFG[WAVE_DMA_OUT_CH] |= DMAMUX_CHCFG_ENBL(0U) | DMAMUX_CHCFG_TRIG(0U);


    /*Initialize the PIT for the Alarm Audio*/
    SIM->SCGC6 |= SIM_SCGC6_PIT(1);
    PIT->MCR &= PIT_MCR_MDIS(0);     /*Start with standard timer enabled*/
    PIT->MCR |=PIT_MCR_FRZ(1);

    /*Initialize the DAC for the Alarm Audio*/
    SIM->SCGC2 |= SIM_SCGC2_DAC0(1);
    DAC0->C0 = DAC_CNTRL_SETUP;

    /*Configure the DMA, set begging address to lookup table beginning*/
    DMA0->TCD[WAVE_DMA_OUT_CH].SADDR = DMA_SADDR_SADDR(SineWave);


    DMA0->TCD[WAVE_DMA_OUT_CH].ATTR = DMA_ATTR_SMOD(0U) | DMA_ATTR_SSIZE(SIZE_CODE_16BIT)
                               | DMA_ATTR_DMOD(0U) | DMA_ATTR_DSIZE(SIZE_CODE_16BIT);
    /*Set the sample sizes to two bytes*/
    DMA0->TCD[WAVE_DMA_OUT_CH].SOFF = DMA_SOFF_SOFF(WAVE_BYTES_PER_SAMPLE);

    /*Set offset value at the end of iteration to -128 so that beginning of lookup
     * table is reached again for every major loop*/
    DMA0->TCD [WAVE_DMA_OUT_CH].SLAST = DMA_SLAST_SLAST(-(WAVE_SAMPLES_PER_BLOCK *
            WAVE_BYTES_PER_SAMPLE));

    /*Set the destination to the DAC0 data register*/
    DMA0->TCD[WAVE_DMA_OUT_CH]. DADDR = DMA_DADDR_DADDR(&DAC0->DAT[0].DATL);
    DMA0->TCD[WAVE_DMA_OUT_CH]. DOFF = DMA_DOFF_DOFF(0);

    /*Set the mux to be triggered by PIT0 */
    DMAMUX->CHCFG[WAVE_DMA_OUT_CH] = DMAMUX_CHCFG_ENBL(1)|DMAMUX_CHCFG_TRIG(1)|
            DMAMUX_CHCFG_SOURCE(60);
    DMA0->SERQ  = DMA_SERQ_SERQ(WAVE_DMA_OUT_CH);

    /*Set the minor loops to the INT16U entries*/
    DMA0->TCD[WAVE_DMA_OUT_CH].NBYTES_MLNO = DMA_NBYTES_MLNO_NBYTES(WAVE_BYTES_PER_SAMPLE);
    DMA0->TCD[WAVE_DMA_OUT_CH].CITER_ELINKNO = DMA_CITER_ELINKNO_CITER(ALARM_SAMPLES_PER_BLOCK);
    DMA0->TCD[WAVE_DMA_OUT_CH].BITER_ELINKNO = DMA_BITER_ELINKNO_BITER(ALARM_SAMPLES_PER_BLOCK);
    DMA0->TCD[WAVE_DMA_OUT_CH].DLAST_SGA = DMA_DLAST_SGA_DLASTSGA(0U);

    /*Configure the CSR register to initialize the TCD*/
    DMA0->TCD [WAVE_DMA_OUT_CH].CSR = DMA_CSR_ESG(0) | DMA_CSR_MAJORELINK(0) |
            DMA_CSR_BWC(3) | DMA_CSR_INTHALF(0) |DMA_CSR_INTMAJOR(0) | DMA_CSR_DREQ(0) |DMA_CSR_START(0);


}


//>>>DMA +3ex
/**********************************************************************************
* AlarmWaveControlTask  public function that controls the PIT0 and DAC0
*                       to configure and control the alarm waveform timing
*                       based on the AlarmState
*
* Return Value:         none
* Arguments:            none
**********************************************************************************/
void AlarmWaveControlTask(void){
    DB5_TURN_ON();

    switch (L5MAlarmState()){

        /*Turn off PIT for Armed*/
        case(ARMED):
            PIT->CHANNEL[0].TCTRL = (PIT_TCTRL_TIE(0)|PIT_TCTRL_TEN(0));
            DAC0->DAT[0].DATL = (0U);
            DAC0->DAT[0].DATH = (0U);
            break;

        /*Turn off alarm for Disarmed State*/
        case(DISARMED):
            PIT->CHANNEL[0].TCTRL = (PIT_TCTRL_TIE(0)|PIT_TCTRL_TEN(0));
            DAC0->DAT[0].DATL = (0U);
            DAC0->DAT[0].DATH = (0U);
            break;

        /*Generate and Display Wave for Alarm On*/
        case(ALARM):
            /*Start PIT if ALARM state is entered*/
            PIT->CHANNEL[0].LDVAL = CNTDOWN_DACSPS;
            PIT->CHANNEL[0].TCTRL = (PIT_TCTRL_TIE(0U)|PIT_TCTRL_TEN(1U));
            break;

        /*Do nothing for default*/
        default:
            break;
    }
    DB5_TURN_OFF();
}



