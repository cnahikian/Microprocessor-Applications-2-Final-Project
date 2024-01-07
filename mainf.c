

/**
 * mainf.c
 */
#include <threadsf.h>
#include "G8RTOS/G8RTOS.h"
#include "./MultimodDrivers/multimod.h"

#include "driverlib/eeprom.h"   //for saving high score
#include <driverlib/timer.h>


extern uint32_t SystemTime;

uint32_t ui32EEPROMInit;

void memory_init() {
    // Enable the EEPROM module
    SysCtlPeripheralEnable(SYSCTL_PERIPH_EEPROM0);

    // Wait for the EEPROM module to be ready
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_EEPROM0));

    // Wait for the EEPROM Initialization to complete
    ui32EEPROMInit = EEPROMInit();

    // Check if the EEPROM Initialization returned an error and inform the application
    if(ui32EEPROMInit != EEPROM_INIT_OK) {
        while(1);
    }
}

void timer_init() {     //timer used for pothole movement
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) {
    }

    TimerDisable(TIMER0_BASE, TIMER_BOTH);

    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC_UP);     //full-width periodic timer counting up
//    TimerPrescaleSet(TIMER0_BASE, TIMER_A, 4);              //set prescaler to 4 (doesn't matter for half-width mode)
    TimerLoadSet(TIMER0_BASE, TIMER_A, 0xFFFFFFFF);         //load max value with 0xFFFFFFFF for Timer0A (32-bit)

    TimerEnable(TIMER0_BASE, TIMER_BOTH);
}

int main(void)
{

    SysCtlClockSet(SYSCTL_SYSDIV_2_5 | SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);
    G8RTOS_Init();
    multimod_init();
    memory_init();
    timer_init();

    G8RTOS_InitSemaphore(&sem_UART, 1);
    G8RTOS_InitSemaphore(&sem_I2CA, 1);
    G8RTOS_InitSemaphore(&sem_SPIA, 1);
    G8RTOS_InitSemaphore(&sem_PCA9555_Debounce, 0);
//    G8RTOS_InitSemaphore(&sem_Joystick_Debounce, 0);

    G8RTOS_AddThread(Idle_Thread, 254, "idle");
    G8RTOS_AddThread(startup, 20, "startup");
    G8RTOS_AddThread(read_buttons, 30, "buttons");

    G8RTOS_Add_APeriodicEvent(GPIOE_Handler, 4, INT_GPIOE); //0x20 for GPIOE


//    G8RTOS_AddThread(game_options, 20, "options");   //not sure why it isn't working
//    G8RTOS_AddThread(start_game, 230, "start");
//    G8RTOS_AddThread(background, 229, "background");
//    G8RTOS_AddThread(car, 230, "car");
//    G8RTOS_AddThread(pothole, 230, "pothole");
//    G8RTOS_AddThread(pothole2, 230, "pothole2");
//    G8RTOS_AddThread(pothole3, 230, "pothole3");
//    G8RTOS_AddThread(restart, 230, "restart");
//    G8RTOS_AddThread(lose_condition, 230, "lose");

//    G8RTOS_Add_PeriodicEvent(Get_Joystick, 100, SystemTime+1);

//    G8RTOS_Add_APeriodicEvent(GPIOD_Handler, 4, INT_GPIOD); //0x19 for GPIOD

//    G8RTOS_InitFIFO(JOYSTICK_FIFO);

    G8RTOS_Launch();
    while(1);
}


