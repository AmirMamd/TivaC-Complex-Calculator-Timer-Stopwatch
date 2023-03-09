#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "driverlib/systick.h"
#include "driverlib/interrupt.h"


#include "inc/hw_ints.h"
#include "driverlib/pin_map.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "utils/uartstdio.h"

#include "lcd.h"
#include "dio.h"

void keyPad_timer(void);
void displayTimer(void);

extern volatile unsigned int state;     // we defined state as extern to be linked to the main definition.

unsigned int minGlobal = 0;
unsigned int secGlobal = 0;
int numOfSeconds;

char time[4] = {'0','0','0','0'};
int index = 0;
int location = 0;
bool flag = false;

// timer initialization using the Systick timer
void timer_init(int minutes, int seconds)
{
  minGlobal = minutes;
  secGlobal = seconds;
  numOfSeconds = (minutes * 60) + seconds + 1 ;  // calculations to know the number of seconds.
  
  SysTickPeriodSet(16777215);     // setting the periodic set which is approximately equivalent to '1' second
  SysTickIntEnable();            // enable the interrupt of systick in the Systickcontrol register
  SysTickEnable();
  SysCtlDelay(1000000);       // delay to give time for the interrupt to be enabled
}


// function called when the interrupts fires and is referenced in the startup file.
void time_out()
{
  numOfSeconds-- ;                   //every time the interrupts fires it decreases the number of seconds.  
  displayTimer();                   // calls the function to decrement the timer and display it if the program is in the timer mode.
  if(numOfSeconds <= 0)
  {
    GPIOPinWrite(GPIO_PORTF_BASE,(GPIO_PIN_1),(GPIO_PIN_1));          // if the timer ended then it opens portf pin 1 which is the red led and the buzzer
    SysTickIntDisable();                                              // Disable the systick interrupt as the time finished
  }
}

//it decrements the timer and displays the time if the program is in the timer's mode.
// if it is in another mode then it will decrement the time only without displaying.
void displayTimer()
{
  int minLocal = minGlobal;
  int secLocal = secGlobal;
  char minDisplay[2];
  char secDisplay[2];
  
  if(secGlobal == 0 & minGlobal != 0)
  {
    minGlobal--;
    secGlobal = 59;
  }
  else if(secGlobal > 0)
  {
    secGlobal--;
  }

  minDisplay[1] =  (minLocal % 10) + '0';
  minLocal /= 10;
  minDisplay[0] =  minLocal + '0';
  
  secDisplay[1] =  (secLocal % 10) + '0';
  secLocal /= 10;
  secDisplay[0] = secLocal + '0';
  if(state == 1)    // checking if the program's mode is in the timer.
  {
    lcd_cmd(0x80);
    lcd_chr(minDisplay[0]);
    lcd_chr(minDisplay[1]);
    lcd_chr(':');
    lcd_chr(secDisplay[0]);
    lcd_chr(secDisplay[1]);
  }
}

/*
  the keypad for the timer mode
*/
void keyPad_timer(void)
{
  unsigned char portval;
  int min;
  int min2;
  int sec;
  int sec2;
  
  if(flag == false)
  {
    lcd_cmd(0x80);
    flag = true;
  }
   if(location == 2)
    {
        location++;
        lcd_cmd(0x14);
     }
  
  for(int i = 0; i < 4; i++)
  {
/*
        Here we set all the columns with '0'.
      */
      GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 |GPIO_PIN_3),0);
      switch(i)
      {
        /*
          give 1 to the ith column.
        */
        case 0:
          // if i == 0 then give column '0', value '1'
          GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_0),(GPIO_PIN_0));
          break;
        case 1:
          // if i == 0 then give column '1', value '1'
          GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_1),(GPIO_PIN_1));
          break;
        case 2:
          // if i == 0 then give column '2', value '1'
          GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_2),(GPIO_PIN_2));
          break;
        case 3:
          // if i == 0 then give column '3', value '1'
          GPIOPinWrite(GPIO_PORTE_BASE,(GPIO_PIN_3),(GPIO_PIN_3));
          break;
      }
      
      // Reads the values in pin 4,5,6,7 and store it in 'portval'.
      // Portval always have an unknown value in the first 4 pins almost it have value 'd'.
      portval =  GPIOPinRead (GPIO_PORTC_BASE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
      
      portval &= (0xF0); 
      if(portval == 0x00)
          {
            // if no button is pressed then nothing is to be done.
          }
           // check if the row 0 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x10)
          {
            switch(i){
              case 0:
                    time[index] = '1';           //store '1' in the array time[index]
                     lcd_chr('1');               // show '1'
                      if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                      {
                        index++;                 // increase the index by 1.
                        location++;              // increase the location by 1.
                      }
                      else
                      {
                        index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                      }                     
                  SysCtlDelay(2000000);             // making delay using the tiva delay function.
                break;
              case 1:
                lcd_chr('2');                 // show '2'
                time[index] = '2';           //store '2' in the array time[index]
                    if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);            // making delay using the tiva delay function.
               break;
              case 2:
                lcd_chr('3');                 // show '3'
                time[index] = '3';           //store '3' in the array time[index]
                   if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);            // making delay using the tiva delay function.
              break;
              case 3:
                break;
            }
          }
      // check if the row 1 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x20)
          {
           switch(i){
              case 0:
                lcd_chr('4');                 // show '4'
                time[index] = '4';           //store '4' in the array time[index]
                    if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);           // making delay using the tiva delay function.
               break;
              case 1:
                lcd_chr('5');                 // show '5'
                time[index] = '5';           //store '5' in the array time[index]
                    if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
                break;
              case 2:
                lcd_chr('6');                 // show '6'
                time[index] = '6';           //store '6' in the array time[index]
                   if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
               break;
              case 3:
                break;
            }
          }
        // check if the row 2 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x40)
        {
            switch(i){
              case 0:
                lcd_chr('7');                 // show '7'
                time[index] = '7';           //store '7' in the array time[index]
                   if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);             // making delay using the tiva delay function.
                break;
              case 1:
                lcd_chr('8');                 // show '8'
                time[index] = '8';           //store '8' in the array time[index]
                    if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);             // making delay using the tiva delay function.
                break;
              case 2:
                lcd_chr('9');                 // show '9'
                time[index] = '9';           //store '9' in the array time[index]
                    if(index <3)             // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                       index  = 0;             // if it is the last digit in the timer then get 
                       location = 0;           // if it is in the last location then set the location to the first location
                       lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }                      
                SysCtlDelay(2000000);                 // making delay using the tiva delay function.
               break;
              case 3:
                DIO_WritePort(&GPIO_PORTF_DATA_R,0x10);  // it closes the timer when it is done and closes the RED LED and BUZZER.
                minGlobal = 0;                           // reset the minutes of timer
                secGlobal = 0;                           // reset the seconds of the timer
                lcd_cmd(0x80);                           // move the cursor to the beginning of the first row
                break;
            }
          }
          // check if the row 3 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x80)
          {
            switch(i){
              case 0:
                break;
              case 1:
                time[index] = '0';               //store '0' in the array time[index]
                  lcd_chr('0');                 // show '0'
                   if(index <3)               // if it is not the last place in the timer then increment the index by '1'
                    {
                      index++;                 // increase the index by 1.
                      location++;              // increase the location by 1.
                    }
                    else
                    {
                      index  = 0;             // if it is the last digit in the timer then get 
                        location = 0;           // if it is in the last location then set the location to the first location
                        lcd_cmd(0x80);          // move the cursor to the beginning of the first row
                    }
                                          
                SysCtlDelay(2000000);              // making delay using the tiva delay function.
              break;
              case 2:
                lcd_chr('#');                         // show '#'
                SysCtlDelay(2000000);                // making delay using the tiva delay function.
               break;
              case 3:
                min  =  time[0] - '0';
                min2 =  time[1] - '0';
                sec  =  time[2] - '0';
                sec2 =  time[3] - '0';
                
                min *=10;
                min += min2;
                
                sec *=10;
                sec += sec2;
                timer_init(min,sec);                 // it takes the values that the user put it and send it to initialize the timer and start it
                SysCtlDelay(1000000);                // making delay using the tiva delay function.
               break;
            }
          }
  }
}