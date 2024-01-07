// G8RTOS_Threads.c
// Date Created: 2023-07-25
// Date Updated: 2023-07-27
// Defines for thread functions.

/************************************Includes***************************************/

#include "./threadsf.h"

#include "./MultimodDrivers/multimod.h"

#include "driverlib/eeprom.h"   //for saving high score
#include <driverlib/timer.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

/*********************************Global Variables**********************************/
#define X_MAX                       240
#define Y_MAX                       280

//car
//square for quiz
#define car_W           20
#define car_H           30
#define car_x_pos_start 110
#define car_y_pos       1

//bike

//pothole
#define pothole_W           40
#define pothole_H           5
#define pothole_y_pos_start 260

/*********************************Global Variables**********************************/

uint8_t lose_flag = 0;

uint16_t car_x_pos = car_x_pos_start;   //will range from 1 to 220 depending on accelerometer
uint16_t car_movement = 0;      //how far the car moves; depends on the accelerometer data

uint16_t pothole_x_pos = 82;    //will change with switch case in pothole thread
uint16_t pothole_y_pos = pothole_y_pos_start;   //will move down consistently from 275 to 1

uint16_t pothole2_x_pos = 100;    //will change with switch case in pothole2 thread
uint16_t pothole2_y_pos = pothole_y_pos_start;   //will move down consistently from 275 to 1

uint16_t pothole3_x_pos = 100;    //will change with switch case in pothole3 thread
uint16_t pothole3_y_pos = pothole_y_pos_start;   //will move down consistently from 275 to 1

uint8_t last_pothole2 = 0;   //does pothole match other pothole?
uint8_t last_pothole3 = 0;   //does pothole match other pothole?

int16_t accel_x_data_old = 500;

uint16_t score_count = 0;   //counter to keep track of score

char *YouLose = "You Lose!";

uint16_t car_color = 0x24E4;     //color of car chosen by user

char *ChooseColor = "Choose Color";

char *Welcome = "Welcome to";
char *Title = "Pothole Panic!";

char *count3 = "3...";
char *count2 = "2...";
char *count1 = "1...";
char *go = " Go! ";

char *new_high_score = "New High Score!";

char *reset_high_score = "High score has been reset";

uint8_t start_flag = 0;     //shows if game is in progress or not

uint32_t high_score[1];     //stores high score data
uint32_t score_count_high = 0;  //value to compare with score and store in high_score
uint8_t new_hs_flag;        //indicates new high score

uint16_t old_accel_data = 110;

uint8_t reset_flag1, reset_flag2, reset_flag3;  //flags for resetting high score in eeprom

int speed_index2, speed_index3 = 0;    //determines speed of potholes

uint16_t pothole2_y_change;
uint16_t pothole2_y_pos_old = 259;
uint16_t pothole3_y_change;
uint16_t pothole3_y_pos_old = 259;

int16_t speed_rate = 150;
int16_t score_temp = 0;

/*************************************Threads***************************************/

void Idle_Thread(void) {
    time_t t;
    srand((unsigned) time(&t));
    while(1);
}

//void game_options() {
//    uint8_t buttons;
//    uint8_t color_select = 0;
//    while(1) {
//        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
//
//        // For switch debouncing
//        sleep(100);
//
////        G8RTOS_WaitSemaphore(&sem_SPIA);
////        ST7789_DrawText(&FontStyle_Emulogic, (const char *)ChooseColor, 72, 130, car_color, ST7789_BLACK);
////        G8RTOS_SignalSemaphore(&sem_SPIA);
//
//        // Get buttons
//        G8RTOS_WaitSemaphore(&sem_I2CA);
//        buttons = ~(MultimodButtons_Get());
//        G8RTOS_SignalSemaphore(&sem_I2CA);
//
//        if (buttons & SW2) {      //not working yet
//            if(color_select < 5) {
//            color_select = color_select + 1;    //changes car color choice
//            }
//            else {
//                color_select = 0;   //back to starting color
//            }
//
//            //choose color of car
//            switch(color_select) {
//                case 0:
//                    car_color = 0xFFE0;     //cyan
//                    break;
//                case 1:
//                    car_color = 0xF81F;     //magenta
//                    break;
//                case 2:
//                    car_color = 0x07E0;     //green
//                    break;
//                case 3:
//                    car_color = 0x001F;     //red
//                    break;
//                case 4:
//                    car_color = 0xF800;     //blue
//                default:
//                    car_color = 0x07E0;     //green
//                    break;
//            }
//            G8RTOS_WaitSemaphore(&sem_SPIA);
//            ST7789_DrawText(&FontStyle_Emulogic, (const char *)ChooseColor, 72, 130, car_color, ST7789_BLACK);
//            G8RTOS_SignalSemaphore(&sem_SPIA);
//        }
//
//
//        //press start button to start game
//        if (buttons & SW4) {
//            G8RTOS_AddThread(start_game, 230, "start");
////            G8RTOS_AddThread(restart, 230, "restart");
//
//            G8RTOS_KillSelf();
//        }
//
//        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
//        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
//    }
//}

void startup() {
    while(1) {
        //welcome to pothole panic on top of screen
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)Welcome, 80, 210, ST7789_WHITE, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        //pothole panic design in top/mid of screen (above choose color)    --> above y=140 and below y=270
        G8RTOS_WaitSemaphore(&sem_SPIA);
//        ST7789_DrawTitleScreen(180);  //does not print correctly
//        ST7789_DrawTitle1();  //does not work
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)Title, 64, 195, ST7789_RED, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        //choose color option in middle of screen
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)ChooseColor, 72, 130, ST7789_WHITE, ST7789_BLACK);  //display car color choice with text
        G8RTOS_SignalSemaphore(&sem_SPIA);

        EEPROMRead(high_score, 0x400, sizeof(high_score));
        score_count_high = high_score[0];

        G8RTOS_WaitSemaphore(&sem_UART);
        UARTprintf("High Score Saved: %d \n", score_count_high);   //test
        G8RTOS_SignalSemaphore(&sem_UART);

        //flags for high score reset in eeprom
        reset_flag1 = 0;
        reset_flag2 = 0;
        reset_flag3 = 0;

        G8RTOS_KillSelf();
    }
}

void background() {
    while(1) {
        G8RTOS_WaitSemaphore(&sem_SPIA);
//        ST7789_Fill(ST7789_BLACK);  //colors background black
        ST7789_DrawRectangle(0, 80, 240, 170, ST7789_BLACK);   //clears middle section of screen
        G8RTOS_SignalSemaphore(&sem_SPIA);

        score_count = 0;

        char score[20];
        sprintf(score, " Score: %d  ", score_count);
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        char hscore[20];
        sprintf(hscore, "High Score: %d  ", score_count_high);
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)hscore, 106, 265, 0xFC80, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

//        ST7789_DrawCar(110);
//        old_accel_data = 110;


//        G8RTOS_WaitSemaphore(&sem_SPIA);  //test
//        ST7789_DrawLine(25, 30, 136, 200, ST7789_GREEN);
//        G8RTOS_SignalSemaphore(&sem_SPIA);
        G8RTOS_KillSelf();
    }
}

void newHighScore() {
    while(1) {
        high_score[0] = score_count_high;

        EEPROMProgram(high_score, 0x400, sizeof(high_score));
//        EEPROMRead(high_score, 0x400, sizeof(high_score));

//        G8RTOS_WaitSemaphore(&sem_UART);
//        UARTprintf("eeprom data: %d\n", *high_score); //test
//        G8RTOS_SignalSemaphore(&sem_UART);
//
//        G8RTOS_WaitSemaphore(&sem_UART);
//        UARTprintf("Score: %d \n", score_count_high);   //test
//        G8RTOS_SignalSemaphore(&sem_UART);

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)new_high_score, 60, 130, ST7789_GREEN, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        char hscore[20];
        sprintf(hscore, "High Score: %d  ", score_count_high);
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)hscore, 106, 265, ST7789_GREEN, ST7789_BLACK);       //different color for emphasis
        G8RTOS_SignalSemaphore(&sem_SPIA);

        G8RTOS_KillSelf();
    }
}

void car(void) {
//    uint16_t joyYdata = 0;
    uint16_t new_accel_data = 110;
    uint16_t offset;
//    uint16_t old_accel_data;
//    int16_t accel_x_data = 200;

    while(1) {
//        G8RTOS_WaitSemaphore(&sem_SPIA);
//        ST7789_DrawRectangle(car_x_pos, car_y_pos, car_W, car_H, 0x0000);  //erase car
//        G8RTOS_SignalSemaphore(&sem_SPIA);

        old_accel_data = new_accel_data;

        G8RTOS_WaitSemaphore(&sem_I2CA);
        int16_t accel_x_data = BMI160_AccelXGetResult();
        G8RTOS_SignalSemaphore(&sem_I2CA);

        if(abs(accel_x_data - accel_x_data_old) < 2500) {

            accel_x_data_old = accel_x_data;

    //        G8RTOS_WaitSemaphore(&sem_UART);
    //        UARTprintf("Accel X value: %2d\n", accel_x_data);   //test
    //        G8RTOS_SignalSemaphore(&sem_UART);

            //steering  **figure out range for limits/speed of car movement
            if(accel_x_data > 1000) {
                //move car right
                if(accel_x_data > 12000) {
                    accel_x_data = 12000;

                }
                new_accel_data = 220 - (((12000 - accel_x_data) / 11000.0) * 110.0);
            }
            else if(accel_x_data >= 300 && accel_x_data <= 1000){
                new_accel_data = 110;
            }
            else if(accel_x_data < 300) {
                //move car left
                if(accel_x_data >=0){
                    offset = accel_x_data + 10700;
                    new_accel_data = (offset / 11000.0) * 110.0;    //offset data by 300
                }
                if(accel_x_data < 0){
                    accel_x_data = -accel_x_data;
                    if(accel_x_data > 11000){
                        accel_x_data = 11000;
                        new_accel_data = 0;
                    }
                    if(accel_x_data <= 11000 && accel_x_data > 0) {
                        uint16_t offset = 11300 - accel_x_data;
                        new_accel_data = (offset / 11000.0) * 110.0;
                    }
                }
            }

            car_x_pos = new_accel_data;

            if(lose_flag == 1){
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(car_x_pos, car_y_pos, car_W, car_H, ST7789_BLACK);  //erase car at collision
                G8RTOS_SignalSemaphore(&sem_SPIA);

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(0, 0, 240, 125, ST7789_BLACK);  //erase bottom of screen at lose
                G8RTOS_SignalSemaphore(&sem_SPIA);

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(0, 142, 240, 120, ST7789_BLACK);  //erase top of screen at lose
                G8RTOS_SignalSemaphore(&sem_SPIA);

                G8RTOS_KillSelf();  //kills thread is lose condition is met
            }
            else {
                if(new_accel_data != old_accel_data) {
                    G8RTOS_WaitSemaphore(&sem_SPIA);
//                    ST7789_DrawRectangle(car_x_pos, car_y_pos, car_W, car_H, car_color);  //draw a green car
                    ST7789_DrawCar(car_x_pos);      //draws custom color car
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }

                if(old_accel_data < new_accel_data) {
                    uint16_t car_change = new_accel_data - old_accel_data;

                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(old_accel_data, car_y_pos, car_change, car_H, 0x0000);  //erase left part of car
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }
                else if(old_accel_data > new_accel_data) {
                    uint16_t car_change = old_accel_data - new_accel_data;
                    uint16_t car_x_pos_erase = (old_accel_data + 20) - car_change;

                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(car_x_pos_erase, car_y_pos, car_change, car_H, 0x0000);  //erase right part of car
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }
            }
        }
    }
}

//void pothole(void) {
//    while(1){
//        uint8_t position = rand() % 3;
//
//        sleep(100);     //delay starting shifts
//
//        switch(position) {
//            case 0:
//                pothole_x_pos = 2;
//                break;
//            case 1:
//                pothole_x_pos = 82;
//                break;
//            case 2:
//                pothole_x_pos = 162;
//                break;
//            default:
//                pothole_x_pos = 82;
//                break;
//        }
////        if(pothole_y_pos > 1){
//        for(int i=0; i < pothole_y_pos_start; i++){
////            G8RTOS_WaitSemaphore(&sem_SPIA);
////            ST7789_DrawRectangle(pothole_x_pos, pothole_y_pos, pothole_W, pothole_H, 0x0000);  //init to black
////            G8RTOS_SignalSemaphore(&sem_SPIA);
//
//            pothole_y_pos = pothole_y_pos_start - i;  //shift y value down by i
//
//            if(pothole_y_pos > 1){
//                G8RTOS_WaitSemaphore(&sem_SPIA);
//                ST7789_DrawRectangle(pothole_x_pos, pothole_y_pos, pothole_W, pothole_H, 0xFFFF); //fit with obstacle dimensions and color it white
//                G8RTOS_SignalSemaphore(&sem_SPIA);
//            }
//            else {
//                G8RTOS_WaitSemaphore(&sem_SPIA);
//                ST7789_DrawRectangle(pothole_x_pos, pothole_y_pos, pothole_W, pothole_H + 1, 0x0000);  //init to black
//                G8RTOS_SignalSemaphore(&sem_SPIA);
//                pothole_y_pos = pothole_y_pos_start;
//            }
//
//            sleep(2);
//
//            G8RTOS_WaitSemaphore(&sem_SPIA);
//            ST7789_DrawRectangle(pothole_x_pos, (pothole_y_pos + 5), pothole_W, 1, 0x0000);  //init to black
//            G8RTOS_SignalSemaphore(&sem_SPIA);
//        }
////        }
////        else{
////            G8RTOS_WaitSemaphore(&sem_SPIA);
////            ST7789_DrawRectangle(pothole_x_pos, pothole_y_pos, pothole_W, pothole_H, 0x0000);  //init to black
////            G8RTOS_SignalSemaphore(&sem_SPIA);
////            pothole_y_pos = pothole_y_pos_start;
////        }
////        sleep(500);
////        G8RTOS_KillSelf();
//    }
//}

void pothole2(void) {   //for second set of potholes (at delayed time from other pothole thread)
//    uint32_t timervalue_old = 0;
    while(1){

//        uint32_t timervalue = TimerValueGet(TIMER0_BASE, TIMER_A);
//        G8RTOS_WaitSemaphore(&sem_UART);
//        UARTprintf("timer count 1: %d \n", timervalue);   //test
//        G8RTOS_SignalSemaphore(&sem_UART);

//        timervalue = TimerValueGet(TIMER0_BASE, TIMER_A);
//        G8RTOS_WaitSemaphore(&sem_UART);
//        UARTprintf("timer count 2: %d \n", timervalue);   //test
//        G8RTOS_SignalSemaphore(&sem_UART);

        if(lose_flag == 1){
            if(new_hs_flag == 0) {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)YouLose, 84, 130, ST7789_RED, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
            }
            else {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)new_high_score, 60, 130, ST7789_GREEN, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
            }

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(0, 0, 240, 35, ST7789_BLACK);  //makes sure car is erased
            G8RTOS_SignalSemaphore(&sem_SPIA);

            G8RTOS_KillSelf();
        }

        uint8_t position = rand() % 5;

        last_pothole2 = position;

//        sleep(10);     //delay starting shifts

        switch(position) {
            case 0:
                pothole2_x_pos = 4;
                break;
            case 1:
                pothole2_x_pos = 52;
                break;
            case 2:
                pothole2_x_pos = 100;
                break;
            case 3:
                pothole2_x_pos = 148;
                break;
            case 4:
                pothole2_x_pos = 196;
                break;
            default:
                pothole2_x_pos = 100;
                break;
        }

        if(position != last_pothole3) {

            int32_t index = 0;

            //disable interrupts
            int32_t start_position = (int32_t)((TimerValueGet(TIMER0_BASE, TIMER_A))>>12);  //20-bit timer start value

            pothole2_y_pos = pothole_y_pos_start;

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawPothole2(pothole2_x_pos, pothole2_y_pos);    //draws a custom pothole2 design
            G8RTOS_SignalSemaphore(&sem_SPIA);

            while(pothole2_y_pos > 1) {
                int32_t pos_val = (int32_t)((TimerValueGet(TIMER0_BASE, TIMER_A))>>12) - start_position;        //change in timer value

                if(pos_val < 0) {       //handles rollover
                    pos_val += 0x100000;
                }

                index = (pos_val / speed_rate);

                if(index > 260) {
                    index = 260;
                }

                pothole2_y_change = pothole2_y_pos_old - pothole2_y_pos;
//                if(pothole2_y_change > 5) {
//                    pothole2_y_change = 5;
//                }
                if(pothole2_y_pos > 255) {
                    pothole2_y_change = 261 - pothole2_y_pos;
                }

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(pothole2_x_pos, (pothole2_y_pos + 5), pothole_W, pothole2_y_change, 0x0000);  //init to black
                G8RTOS_SignalSemaphore(&sem_SPIA);

                pothole2_y_pos_old = pothole2_y_pos;

                pothole2_y_pos = pothole_y_pos_start - index;

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawPothole2(pothole2_x_pos, pothole2_y_pos);    //draws a custom pothole2 design
                G8RTOS_SignalSemaphore(&sem_SPIA);

                if(lose_flag == 1){
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(pothole2_x_pos, pothole2_y_pos, pothole_W, pothole_H + 3, 0x0000);  //init to black
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                    break;
                }
            }
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(pothole2_x_pos, pothole2_y_pos, pothole_W, pothole_H, 0x0000);  //init to black
            G8RTOS_SignalSemaphore(&sem_SPIA);

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(pothole2_x_pos, 0, pothole_W, 50, 0x0000);  //init to black
            G8RTOS_SignalSemaphore(&sem_SPIA);

            pothole2_y_pos = pothole_y_pos_start;


            if(lose_flag == 0) {
                score_count = score_count + 1;  //increment score counter
                char score[20];
                sprintf(score, " Score: %d", score_count);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                if(score_count >= 20) {         //speed of pothole movement increases every 20 points
                    if(score_temp + 20 == score_count) {
                        score_temp = score_count;
                        speed_rate = speed_rate * 0.9;
                    }
                }

//                if(score_count >= 20) {     //determines speed of pothole2 movement
//                    if(score_count >= 40) {
//                        if(score_count >= 60) {
//                            if(score_count >= 80) {
//                                if(score_count >= 100) {
//                                    if(score_count >= 120){
//                                        speed_rate = speed_rate * 0.9;
//                                    }
//                                    else {
//                                    speed_rate = speed_rate * 0.9;
//                                    }
//                                }
//                                else {
//                                    speed_rate = speed_rate * 0.9;
//                                }
//                            }
//                            else {
//                                speed_rate = speed_rate * 0.9;
//                            }
//                        }
//                        else {
//                            speed_rate = speed_rate * 0.9;
//                        }
//                    }
//                    else {
//                        speed_rate = speed_rate * 0.9;
//                    }
//                }
            }

//            for(int i=0; i < pothole_y_pos_start; i++){
//
//                pothole2_y_pos = pothole_y_pos_start - (i + speed_index2);  //shift y value down by (i + speed_index2)  --> this determines the speed of potholes
//
//                if(pothole2_y_pos > 1){
//                    G8RTOS_WaitSemaphore(&sem_SPIA);
////                    ST7789_DrawRectangle(pothole2_x_pos, pothole2_y_pos, pothole_W, pothole_H, 0xFFFF); //fit with obstacle dimensions and color it white
//                    ST7789_DrawPothole2(pothole2_x_pos, pothole2_y_pos);    //draws a custom pothole2 design
//                    G8RTOS_SignalSemaphore(&sem_SPIA);
//
////                    //timer test
////                    uint32_t timervalue = TimerValueGet(TIMER0_BASE, TIMER_A);
////
////                    uint32_t timer_diff = abs(timervalue - timervalue_old);
////                    G8RTOS_WaitSemaphore(&sem_UART);
////                    UARTprintf("timer value: %d \n", timervalue>>12);   //test
////                    G8RTOS_SignalSemaphore(&sem_UART);
////                    timervalue_old = timervalue;
//                }
//                else {
//                    G8RTOS_WaitSemaphore(&sem_SPIA);
//                    ST7789_DrawRectangle(pothole2_x_pos, pothole2_y_pos, pothole_W, pothole_H + 1, 0x0000);  //init to black
//                    G8RTOS_SignalSemaphore(&sem_SPIA);
//                    pothole2_y_pos = pothole_y_pos_start;
//
//                    if(lose_flag == 0) {
//                        score_count = score_count + 1;  //increment score counter
//                        char score[20];
//                        sprintf(score, " Score: %d", score_count);
//                        G8RTOS_WaitSemaphore(&sem_SPIA);
//                        ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
//                        G8RTOS_SignalSemaphore(&sem_SPIA);
//
//                        if(score_count >= 20) {     //determines speed of pothole2 movement
//                            if(score_count >= 60) {
//                                if(score_count >= 100) {    //maxes out at 100 points
//                                    speed_index2 = 3;
//                                }
//                                else {
//                                    speed_index2 = 2;
//                                }
//                            }
//                            else {
//                                speed_index2 = 1;
//                            }
//                        }
//                    }
//                    break;
//                }
//
//                sleep(1);
//
//                G8RTOS_WaitSemaphore(&sem_SPIA);
//                ST7789_DrawRectangle(pothole2_x_pos, (pothole2_y_pos + 5), pothole_W, 1, 0x0000);  //init to black
//                G8RTOS_SignalSemaphore(&sem_SPIA);
//            }
        }
    }
}

void pothole3(void) {   //for second set of potholes (at delayed time from other pothole thread)
    while(1){

        if(lose_flag == 1){
            if(new_hs_flag == 0) {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)YouLose, 84, 130, ST7789_RED, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
            }
            else {
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)new_high_score, 60, 130, ST7789_GREEN, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);
            }

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(0, 0, 240, 35, ST7789_BLACK);  //makes sure car is erased
            G8RTOS_SignalSemaphore(&sem_SPIA);

            G8RTOS_KillSelf();
        }

        uint8_t position = rand() % 5;

        last_pothole3 = position;

        sleep(250);     //delay starting shifts

        switch(position) {
            case 0:
                pothole3_x_pos = 4;
                break;
            case 1:
                pothole3_x_pos = 52;
                break;
            case 2:
                pothole3_x_pos = 100;
                break;
            case 3:
                pothole3_x_pos = 148;
                break;
            case 4:
                pothole3_x_pos = 196;
                break;
            default:
                pothole3_x_pos = 100;
                break;
        }

        if(position != last_pothole2) {

            int32_t index = 0;

            //disable interrupts
            int32_t start_position = (int32_t)((TimerValueGet(TIMER0_BASE, TIMER_A))>>12);  //20-bit timer start value

            pothole3_y_pos = pothole_y_pos_start;

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawPothole3(pothole3_x_pos, pothole3_y_pos);    //draws a custom pothole3 design
            G8RTOS_SignalSemaphore(&sem_SPIA);

            while(pothole3_y_pos > 1) {
                int32_t pos_val3 = (int32_t)((TimerValueGet(TIMER0_BASE, TIMER_A))>>12) - start_position;        //change in timer value

                if(pos_val3 < 0) {       //handles rollover
                    pos_val3 += 0x100000;
                }

                index = (pos_val3 / speed_rate);

                if(index > 260) {
                    index = 260;
                }

                pothole3_y_change = pothole3_y_pos_old - pothole3_y_pos;
//                if(pothole2_y_change > 5) {
//                    pothole2_y_change = 5;
//                }
                if(pothole3_y_pos > 255) {
                    pothole3_y_change = 261 - pothole3_y_pos;
                }

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(pothole3_x_pos, (pothole3_y_pos + 5), pothole_W, pothole3_y_change, 0x0000);  //init to black
                G8RTOS_SignalSemaphore(&sem_SPIA);

                pothole3_y_pos_old = pothole3_y_pos;

                pothole3_y_pos = pothole_y_pos_start - index;

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawPothole3(pothole3_x_pos, pothole3_y_pos);    //draws a custom pothole3 design
                G8RTOS_SignalSemaphore(&sem_SPIA);

                if(lose_flag == 1){
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawRectangle(pothole3_x_pos, pothole3_y_pos, pothole_W, pothole_H + 3, 0x0000);  //init to black
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                    break;
                }
            }
            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(pothole3_x_pos, pothole3_y_pos, pothole_W, pothole_H, 0x0000);  //init to black
            G8RTOS_SignalSemaphore(&sem_SPIA);

            G8RTOS_WaitSemaphore(&sem_SPIA);
            ST7789_DrawRectangle(pothole3_x_pos, 0, pothole_W, 50, 0x0000);  //init to black
            G8RTOS_SignalSemaphore(&sem_SPIA);

            pothole3_y_pos = pothole_y_pos_start;


            if(lose_flag == 0) {
                score_count = score_count + 1;  //increment score counter
                char score[20];
                sprintf(score, " Score: %d", score_count);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);


                if(score_count >= 20) {         //speed of pothole movement increases every 20 points
                    if(score_temp + 20 == score_count) {
                        score_temp = score_count;
                        speed_rate = speed_rate * 0.9;
                    }
                }

//                if(score_count >= 25) {     //determines speed of pothole2 movement
//                    if(score_count >= 50) {
//                        if(score_count >= 75) {
//                            if(score_count >= 100) {
//                                if(score_count >= 125) {
//                                    speed_rate = 25;
//                                }
//                                else {
//                                    speed_rate = 50;
//                                }
//                            }
//                            else {
//                                speed_rate = 75;
//                            }
//                        }
//                        else {
//                            speed_rate = 100;
//                        }
//                    }
//                    else {
//                        speed_rate = 125;
//                    }
//                }
            }

//            for(int i=0; i < pothole_y_pos_start; i++){
//
//                pothole3_y_pos = pothole_y_pos_start - (i + speed_index3);  //shift y value down by (i + speed_index3)
//
//                if(pothole3_y_pos > 1){
//                    G8RTOS_WaitSemaphore(&sem_SPIA);
////                    ST7789_DrawRectangle(pothole3_x_pos, pothole3_y_pos, pothole_W, pothole_H, 0xFFFF); //fit with obstacle dimensions and color it white
//                    ST7789_DrawPothole3(pothole3_x_pos, pothole3_y_pos);    //draws a custom pothole3 design
//                    G8RTOS_SignalSemaphore(&sem_SPIA);
//                }
//                else {
//                    G8RTOS_WaitSemaphore(&sem_SPIA);
//                    ST7789_DrawRectangle(pothole3_x_pos, pothole3_y_pos, pothole_W, pothole_H + 1, 0x0000);  //init to black
//                    G8RTOS_SignalSemaphore(&sem_SPIA);
//                    pothole3_y_pos = pothole_y_pos_start;
//
//                    if(lose_flag == 0) {
//                        score_count = score_count + 1;  //increment score counter
//                        char score[20];
//                        sprintf(score, " Score: %d", score_count);
//                        G8RTOS_WaitSemaphore(&sem_SPIA);
//                        ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
//                        G8RTOS_SignalSemaphore(&sem_SPIA);
//
//                        if(score_count >= 40) {     //determines speed of pothole3 movement
//                            if(score_count >= 80) {
//                                if(score_count >= 120) {    //maxes out at 120 points
//                                    speed_index3 = 3;
//                                }
//                                else {
//                                    speed_index3 = 2;
//                                }
//                            }
//                            else {
//                                speed_index3 = 1;
//                            }
//                        }
//                    }
//                    break;
//                }
//
//                sleep(1);
//
//                G8RTOS_WaitSemaphore(&sem_SPIA);
//                ST7789_DrawRectangle(pothole3_x_pos, (pothole3_y_pos + 5), pothole_W, 1, 0x0000);  //init to black
//                G8RTOS_SignalSemaphore(&sem_SPIA);
//            }
        }
    }
}


void start_game(void) {
    while(1) {
        score_count = 0;    //reset score
        lose_flag = 0;      //reset lose flag
        start_flag = 1;     //game in progress flag
        new_hs_flag = 0;    //no new high score at start

        speed_rate = 150;   //init speed of potholes
        score_temp = 0;     //reset temp for changing speed of potholes

        //flags for high score reset in eeprom
        reset_flag1 = 0;
        reset_flag2 = 0;
        reset_flag3 = 0;

        ST7789_DrawCar(110);
        old_accel_data = 110;

        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawRectangle(0, 100, 240, 160, ST7789_BLACK);   //clears middle section of screen
        G8RTOS_SignalSemaphore(&sem_SPIA);

        char score[20];
        sprintf(score, " Score: %d  ", 0);
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        char hscore[20];
        sprintf(hscore, "High Score: %d  ", score_count_high);
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)hscore, 106, 265, 0xFC80, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        //add countdown to game start in middle of screen
        //3
        G8RTOS_WaitSemaphore(&sem_SPIA);
//        ST7789_DrawCount3();
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)count3, 104, 130, ST7789_RED, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        sleep(750);
        //2
        G8RTOS_WaitSemaphore(&sem_SPIA);
//        ST7789_DrawCount2();
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)count2, 104, 130, ORN, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        sleep(750);
        //1
        G8RTOS_WaitSemaphore(&sem_SPIA);
//        ST7789_DrawCount1();
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)count1, 104, 130, 0x0500, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        sleep(750);
        //GO!
        G8RTOS_WaitSemaphore(&sem_SPIA);
        ST7789_DrawText(&FontStyle_Emulogic, (const char *)go, 100, 130, ST7789_WHITE, ST7789_BLACK);
        G8RTOS_SignalSemaphore(&sem_SPIA);

        sleep(750);

        //middle of screen cleared with background thread

        G8RTOS_AddThread(background, 5, "background");
        G8RTOS_AddThread(car, 6, "car");
        G8RTOS_AddThread(pothole2, 6, "pothole2");
        G8RTOS_AddThread(pothole3, 6, "pothole3");
        G8RTOS_AddThread(lose_condition, 6, "lose");

        G8RTOS_KillSelf();
    }
}


void lose_condition(void) {
    //check if game is lost
    while(1){
        if(pothole2_y_pos <= (car_y_pos + 30)){   //obstacle within range of vehicle (on ground or less than height)
            if((car_x_pos > pothole2_x_pos - 20) && (car_x_pos < pothole2_x_pos + 40)){   //if x values are overlapping with vehicle
                //fail
                lose_flag = 1;
                start_flag = 0; //ends game session

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(0, 0, 240, 264, ST7789_BLACK); //black out screen
                G8RTOS_SignalSemaphore(&sem_SPIA);

//                G8RTOS_WaitSemaphore(&sem_UART);
//                UARTprintf("Score: %d \n", score_count);   //test
//                G8RTOS_SignalSemaphore(&sem_UART);

                if((uint32_t)score_count > score_count_high) {
                    score_count_high = (uint32_t)score_count;
                    new_hs_flag = 1;
                    G8RTOS_AddThread(newHighScore, 10, "newHighScore");
                }

                char score[20];
                sprintf(score, " Score: %d", score_count);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                char hscore[20];
                sprintf(hscore, "High Score: %d  ", score_count_high);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)hscore, 106, 265, 0xFC80, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);


                if(new_hs_flag == 0) {
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawText(&FontStyle_Emulogic, (const char *)YouLose, 84, 130, ST7789_RED, ST7789_BLACK);
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }

                G8RTOS_KillSelf();
            }
            else {   //vehicle not in range of obstacle
                //pass
                lose_flag = 0;
            }
        }
        else {
            lose_flag = 0;
        }

        if(pothole3_y_pos <= (car_y_pos + 30)){   //obstacle within range of vehicle (on ground or less than height)
            if((car_x_pos > pothole3_x_pos - 20) && (car_x_pos < pothole3_x_pos + 40)){   //if x values are overlapping with vehicle
                //fail
                lose_flag = 1;
                start_flag = 0; //ends game session

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(0, 0, 240, 264, ST7789_BLACK); //black out screen
                G8RTOS_SignalSemaphore(&sem_SPIA);

//                G8RTOS_WaitSemaphore(&sem_UART);
//                UARTprintf("Score: %d \n", score_count);   //test
//                G8RTOS_SignalSemaphore(&sem_UART);

                if((uint32_t)score_count > score_count_high) {
                    score_count_high = (uint32_t)score_count;
                    new_hs_flag = 1;
                    G8RTOS_AddThread(newHighScore, 10, "newHighScore");
                }

                char score[20];
                sprintf(score, " Score: %d", score_count);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                char hscore[20];
                sprintf(hscore, "High Score: %d  ", score_count_high);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)hscore, 106, 265, 0xFC80, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                if(new_hs_flag == 0) {
                    G8RTOS_WaitSemaphore(&sem_SPIA);
                    ST7789_DrawText(&FontStyle_Emulogic, (const char *)YouLose, 84, 130, ST7789_RED, ST7789_BLACK);
                    G8RTOS_SignalSemaphore(&sem_SPIA);
                }

                G8RTOS_KillSelf();
            }
            else {   //vehicle not in range of obstacle
                //pass
                lose_flag = 0;
            }
        }
        else {
            lose_flag = 0;
        }

//        G8RTOS_WaitSemaphore(&sem_UART);
//        UARTprintf("Score: %d \n", score_count);   //test
//        G8RTOS_SignalSemaphore(&sem_UART);

    }
}


//void restart(void) {
//    // Initialize / declare any variables here
//    uint8_t buttons;
//
//    while(1) {
//        // Wait for a signal to read the buttons on the Multimod board.
//        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);
//
//        // Sleep to debounce
//        sleep(100);
//
//        // Read the buttons status on the Multimod board.
//        G8RTOS_WaitSemaphore(&sem_I2CA);
//        buttons = ~(MultimodButtons_Get());
//        G8RTOS_SignalSemaphore(&sem_I2CA);
//
//        // Process the buttons and determine what actions need to be performed.
//        if(buttons & SW1) {
//            if(lose_flag == 1){
//                G8RTOS_AddThread(start_game, 0, "start"); //starts the game over
//
//                //should change start_game to game_options thread for actual gameplay
//
//                lose_flag = 0;
//            }
//        }
//
//        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
//        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
//    }
//}

void read_buttons(void) {
    uint8_t buttons;
    uint8_t color_select = 0;
    uint16_t bodycolor_old = BDY;

    while(1) {
        // Wait for a signal to read the buttons on the Multimod board.
        G8RTOS_WaitSemaphore(&sem_PCA9555_Debounce);

        // Sleep to debounce
        sleep(100);

        // Read the buttons status on the Multimod board.
        G8RTOS_WaitSemaphore(&sem_I2CA);
        buttons = ~(MultimodButtons_Get());
        G8RTOS_SignalSemaphore(&sem_I2CA);

        // Process the buttons and determine what actions need to be performed.
//        if(buttons & SW1) {
//            if(lose_flag == 1){
//                G8RTOS_AddThread(start_game, 0, "start"); //starts the game over
//
//                //should change start_game to game_options thread for actual gameplay
//
//                lose_flag = 0;
//            }
//        }

        if (buttons & SW1) {
            if(start_flag == 0) {
                reset_flag1 = 1;
                if(color_select < 4) {
                color_select = color_select + 1;    //changes car color choice
                }
                else {
                    color_select = 0;   //back to starting color
                }

                //choose color of car
                switch(color_select) {
                    case 0:
                        car_color = 0x24E4;     //dark green
                        break;
                    case 1:
                        car_color = 0xC018;     //pretty purple
                        break;
                    case 2:
                        car_color = 0x31BF;     //light red
                        break;
                    case 3:
                        car_color = 0x9A60;     //pretty blue
                        break;
                    case 4:
                        car_color = 0x049D;     //pretty orange
                        break;
                    default:
                        car_color = 0x24E4;     //dark green
                        break;
                }
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(65, 260, 40, 15, ST7789_BLACK);  //clears score on top of screen
                G8RTOS_SignalSemaphore(&sem_SPIA);

                char score[20];
                sprintf(score, " Score: %d  ", 0);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)score, 10, 265, ST7789_RED, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                char hscore[20];
                sprintf(hscore, "High Score: %d  ", score_count_high);
                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)hscore, 106, 265, 0xFC80, ST7789_BLACK);
                G8RTOS_SignalSemaphore(&sem_SPIA);

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(60, 125, 16, 15, ST7789_BLACK);  //clears new high score message
                G8RTOS_SignalSemaphore(&sem_SPIA);

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawRectangle(167, 125, 16, 15, ST7789_BLACK);  //clears new high score message
                G8RTOS_SignalSemaphore(&sem_SPIA);

                G8RTOS_WaitSemaphore(&sem_SPIA);
                ST7789_DrawText(&FontStyle_Emulogic, (const char *)ChooseColor, 72, 130, car_color, ST7789_BLACK);  //display car color choice with text
                G8RTOS_SignalSemaphore(&sem_SPIA);

                bodycolor_old = alter_CarArray(bodycolor_old, car_color);   //puts chosen car color into array to be drawn

                ST7789_DrawCar(110);    //draws car with chosen color

                old_accel_data = 110;

//                for(int i=0; i<600; i++) {
//                    if (car_colors[i] == bodycolor_old) {
//                        car_colors[i] = car_color;
//                    }
//                }
//
//                bodycolor_old = car_color;
            }
        }

        //press start button to start game
        if (buttons & SW4) {
            if(start_flag == 0) {
                G8RTOS_AddThread(start_game, 230, "start");
            }
        }

        if (buttons & SW2) {
            if(reset_flag1 == 1) {
                reset_flag2 = 1;
            }
        }

        if (buttons & SW3) {        //order should go SW1, SW2, SW3, SW3 to reset high score
            if(reset_flag2 == 1){
                if(reset_flag3 == 1) {
                   score_count_high = 0;
                   high_score[0] = score_count_high;

                   EEPROMProgram(high_score, 0x400, sizeof(high_score));   //reset high score in memory

                   char hscore[20];
                   sprintf(hscore, "High Score: %d  ", score_count_high);
                   G8RTOS_WaitSemaphore(&sem_SPIA);
                   ST7789_DrawText(&FontStyle_Emulogic, (const char *)hscore, 106, 265, ORN, ST7789_BLACK);
                   G8RTOS_SignalSemaphore(&sem_SPIA);

                   G8RTOS_WaitSemaphore(&sem_SPIA);
                   ST7789_DrawText(&FontStyle_Emulogic, (const char *)reset_high_score, 20, 162, ORN, ST7789_BLACK);
                   G8RTOS_SignalSemaphore(&sem_SPIA);

                   //reset flags after reset eeprom
                   reset_flag1 = 0;
                   reset_flag2 = 0;
                   reset_flag3 = 0;
                }
                else {
                    reset_flag3 = 1;
                }
            }
        }

        GPIOIntClear(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
        GPIOIntEnable(BUTTONS_INT_GPIO_BASE, BUTTONS_INT_PIN);
    }
}

/********************************Periodic Threads***********************************/

//void Get_Joystick(void) {
//    uint16_t joy_y_data = 0;
//
//    joy_y_data = JOYSTICK_GetY();   //get data
//    G8RTOS_WriteFIFO(JOYSTICK_FIFO, joy_y_data);    //send data to FIFO
//}
//
/*******************************Aperiodic Threads***********************************/


void GPIOE_Handler() {
    // Disable interrupt
    GPIOIntDisable(GPIO_PORTE_BASE, GPIO_INT_PIN_4);

    // Signal relevant semaphore
    G8RTOS_SignalSemaphore(&sem_PCA9555_Debounce);
}

//void GPIOD_Handler() {
//    // Disable interrupt
//    GPIOIntDisable(GPIO_PORTD_BASE, GPIO_INT_PIN_2);
//
//    // Signal relevant semaphore
//    G8RTOS_SignalSemaphore(&sem_Joystick_Debounce);
//}
