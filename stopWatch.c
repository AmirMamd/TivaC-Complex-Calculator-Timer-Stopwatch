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

void keyPad_stop_watch(void);
void display_stop_watch();

volatile int minGlobalStopWatch = 0;
volatile int secGlobalStopWatch = 0;

extern volatile unsigned int state;     // we defined state as extern to be linked to the main definition.

char time1[4] = {'0','0','0','0'};
int index1 = 0;
int location1 = 0;

/*
  Initialize the stopwatch using Timer0, TimerA
*/
void stop_watch_init(void)
{
  /*
    
  */
  SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN |SYSCTL_XTAL_16MHZ);
  
  /*
    open Timer0
  */
  SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
  
  /*
    check that timer0 is opened
  */
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));
  
  /*
    Enable the master Enable which is 90% of the time is enabled but for 
  */
  IntMasterEnable();
  
  /*
    configure timer0 to be periodic.
  */
  TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
  
  /*
    set the number of the ticks needed to make an interrupt.
    16777215 is equivalent to approximately 1 second in real time.
  */
  TimerLoadSet(TIMER0_BASE, TIMER_A, 16777215);
  
  /*
    interrupt is done when having the positive edge
  */
  TimerControlEvent(TIMER0_BASE, TIMER_A, TIMER_EVENT_POS_EDGE);
  
  /*
    enable Timer0 interrupts
  */
  TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  
  /*
    open Timer0 and timerA
  */
  TimerEnable(TIMER0_BASE, TIMER_A);
  
  /*
    enable the interrupt for timer0A
  */
  IntEnable(INT_TIMER0A);
}

/*
  this is the function called when the interrupt is interrupted
  and it is written in the start up code file.
*/
void stop_watch_display()
{
  TimerIntClear(TIMER0_BASE, TIMER_A);           // close the interrupt flag.
  display_stop_watch();                          // calling the function to increase the timer and display the new time
}

/*
  In this function we increment the time and displays it  when every interrupt is fired.
  If program is in another state it only increments time but doesn't display.
*/ 
void display_stop_watch()
{
  int minLocal;
  int secLocal;
  char minDisplay[2];
  char secDisplay[2];
  
  if(secGlobalStopWatch < 59)              // if seconds are less than 59 then we increment it.
  {
    secGlobalStopWatch++;
  }
  else if (secGlobalStopWatch == 59)      //if the seconds are 59 then we increment the minutes and begin a new minute.
  {
    secGlobalStopWatch = 0;
    minGlobalStopWatch++;
  }

  minLocal = minGlobalStopWatch;
  secLocal = secGlobalStopWatch;
  
  minDisplay[1] =  (minLocal % 10) + '0';
  minLocal /= 10;
  minDisplay[0] =  minLocal + '0';
  
  secDisplay[1] =  (secLocal % 10) + '0';
  secLocal /= 10;
  secDisplay[0] = secLocal + '0';
  if(state == 0)   // check if the state is the "Stopwatch's state"  then display the time. 
  {
      lcd_cmd(0x80);
      lcd_chr(minDisplay[0]);    // display first digit of minutes
      lcd_chr(minDisplay[1]);    // display second digit of minutes
      lcd_chr(':');              // display the ':'
      lcd_chr(secDisplay[0]);    // display first digit of seconds
      lcd_chr(secDisplay[1]);    // display second digit of seconds
  } 
    
}



void keyPad_stop_watch(void)
{
  unsigned char portval;               //variable that holds the value of column that have '1' in the keypad.
  
    if(location1 == 2)    // if location is the third it e skip it as it is the ':'.
    {
        location1++;
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
      
      // it stores the values of the pins 4,5,6,7 and ignore the 4 four pins 0,1,2,3.
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
                break;
              case 1:
               break;
              case 2:
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
               break;
              case 1:
                break;
              case 2:       
               break;
              case 3:
                /*
                    The reset button
                    reset the variables
                    stop the stopwatch
                    disable the interrupt
                 */
                minGlobalStopWatch = 0;    // reset the mins variable with zero 
                secGlobalStopWatch = 0;    // reset the seconds with zero.
                lcd_cmd(0x80);             // make the cursor at the beginning of the first row
                LCD_String("00:00");       // show the stopwatch view at the initial state
                TimerIntDisable(TIMER0_BASE, TIMER_A); // close the Interrupt to stop the stopwatch.
                break;
                
            }
          }
         // check if the row 2 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x40)
        {
            switch(i){
              case 0:
                break;
              case 1:
                break;
              case 2:             
               break;
              case 3:
                /*
                   Stop button which stops the stopwatch and maintain the current state. so when the start button is pressed afterwards it continues counting.
                 */
                TimerIntDisable(TIMER0_BASE, TIMER_A);
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
              break;
              case 2:
               break;
              case 3:
                /*
                   The start button
                 */
                stop_watch_init();           //initialize the stopwatch
                SysCtlDelay(2000000);        // making delay using the tiva delay function.
               break;
            }
          }
  }
}

