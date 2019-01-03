/****************************************************************************************
* EE344, rsLab5Project, WDogMod
*   Program handles WatchDog of Security System
*
* Robert Sanborn, Nov 30, 2018
*
****************************************************************************************/
#include "MCUType.h"            /*    Include project header file           */
#include "WDogMod.h"
#include "Lab5Main.h"
//#include "SysTickDelay.h"

/***********************************************************************************
 * WDogRefreshTask()        Refreshes Watchdog timer so that program does not reset
 *                      must be called at least every 20 milliseconds upon initialization
 *                      Watchdog module.
 *
 * Return Value:        none
 * Arguments:           none
 *************************************************************************************/
void WDogRefreshTask(void){
    DB6_TURN_ON();

    WDOG->REFRESH = 0xA602U;
    WDOG->REFRESH = 0xB480U;
//    SysTickDelay(20);   //>>>Check Wdog
    DB6_TURN_OFF();
}



/***********************************************************************************
 * WDogInit()           Initializes Watchdog timer so that program does not reset
 *                      must be called at least every 20 milliseconds upon initialization
 *                      Watchdog module.
 *
 * Return Value:        none
 * Arguments:           none
 *************************************************************************************/
void WDogInit(void){

    /* WDOG->UNLOCK: WDOGUNLOCK=0xC520 */
    WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xC520); /* Key 1 */
    /* WDOG->UNLOCK: WDOGUNLOCK=0xD928 */
    WDOG->UNLOCK = WDOG_UNLOCK_WDOGUNLOCK(0xD928); /* Key 2 */
    WDOG->TOVALH = (WDOG_TOVL >> 16U);  //>>>23ms -1ex
    WDOG->TOVALL = WDOG_TOVL & 0xFFFFU;
    WDOG->PRESC  = WDOG_PRESC_PRESCVAL(WDOG_PRESCAL);
    WDOG->STCTRLH |= WDOG_STCTRLH_WDOGEN_MASK;

}
//>>>WDog +3ex
