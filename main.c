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
#include "calculator.h"
#include "time.h"
#include "stopWatch.h"
#include "run.h"



int main()
{
  
  /*
  Initialize the Ports (E - C) for keyPad 
  (F) for the interrupts and red led.
  */
  initialization();
  
  /*
   Enable the interrupt register and have the name the function that the interrupt will go to it. 
  */
  openIntF();
  
  /*
  Initialize the lCD.
  */
  lcd_init();
  
  /*
  Erase all data on the screen. (Reset the LCD)
  */
  lcd_cmd(0x01);
  
  /*
  Go to the initial state of the program and be in the switch case to change between states.
  */
  runaya(); 
}
