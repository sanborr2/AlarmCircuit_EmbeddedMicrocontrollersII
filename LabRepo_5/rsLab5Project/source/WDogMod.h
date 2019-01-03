/****************************************************************************************
* EE344, rsLab5Project, WDogMod
*   Program handles WatchDog of Security System
*
* Robert Sanborn, Nov 30, 2018
*
****************************************************************************************/

#ifndef WDOGMOD_H_
#define WDOGMOD_H_

#define WDOG_PRESCAL        6U  //>>>should not be public
#define WDOG_TOVL           200000U



/***********************************************************************************
 * WDogRefreshTask()        Refreshes Watchdog timer so that program does not reset
 *                      must be called at least every 2 seconds upon initialization
 *                      Watchdog module.
 *
 * Return Value:        none
 * Arguments:           none
 *************************************************************************************/
void WDogRefreshTask(void);



/***********************************************************************************
 * WDogInit()           Initializes Watchdog timer so that program does not reset
 *                      must be called at least every 2 seconds upon initialization
 *                      Watchdog module.
 *
 * Return Value:        none
 * Arguments:           none
 *************************************************************************************/
void WDogInit(void);

#endif /* WDOGMOD_H_ */
