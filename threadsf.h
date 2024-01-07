// threads.h
// Date Created: 2023-07-26
// Date Updated: 2023-07-26
// Threads

#ifndef THREADSF_H_
#define THREADSF_H_

/************************************Includes***************************************/

#include "./G8RTOS/G8RTOS.h"

/************************************Includes***************************************/

/*************************************Defines***************************************/

#define JOYSTICK_FIFO       1

/*************************************Defines***************************************/

/***********************************Semaphores**************************************/

semaphore_t sem_UART;
semaphore_t sem_I2CA;
semaphore_t sem_SPIA;
semaphore_t sem_PCA9555_Debounce;
semaphore_t sem_Joystick_Debounce;

/***********************************Semaphores**************************************/

/***********************************Structures**************************************/
/***********************************Structures**************************************/


/*******************************Background Threads**********************************/

void Idle_Thread(void);
void startup(void);
void newHighScore(void);
//void game_options(void);
void read_buttons(void);
void start_game(void);
void background(void);
void car(void);
//void pothole(void);
void pothole2(void);
void pothole3(void);
//void restart(void);
void lose_condition(void);

/*******************************Background Threads**********************************/

/********************************Periodic Threads***********************************/

//void Print_WorldCoords(void);
void Get_Joystick(void);

/********************************Periodic Threads***********************************/

/*******************************Aperiodic Threads***********************************/


void GPIOE_Handler(void);
void GPIOD_Handler(void);

/*******************************Aperiodic Threads***********************************/


#endif /* THREADSF_H_ */

