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
//#include "run.h"
volatile unsigned int state = 0;
void mario(void);
void openIntF()
{
  GPIOIntRegister(GPIO_PORTF_BASE,mario);
}
void runaya()
{
  lcd_cmd(0x80);
  LCD_String("00:00");
  lcd_cmd(0xC0);
  LCD_String("  Stop watch  ");
  lcd_cmd(0x80);
  
  while(1)
  {
    switch(state)
    {
    case 0:
     keyPad_stop_watch();
      break;
    case 1:
      keyPad_timer();
      break;
    case 2:
      calc();
      break;
    default:
      LCD_String("Super Erroraya");
      break;
    }
  }
}

void init_stopWatch()
{
  lcd_cmd(0x01);
  LCD_String("00:00");
  lcd_cmd(0xC0);
  LCD_String("  Stop watch  ");
  lcd_cmd(0x80);
  
}
void init_timer()
{
  lcd_cmd(0x01);
  LCD_String("00:00");
  lcd_cmd(0xC0);
   LCD_String("    Timers  ");
  lcd_cmd(0x80);
  
}
void mario()
{
  delay(1000000);     //Delay 3shan dost el interrupt ttmsk mara wa7da may3odsh mn dosa wa7da yedy kza interrupt 3shan function execution saree3
  if(state == 0)
  {
    state++ ;
    init_timer();
  }
  else if (state == 1)
  {
    state++;
    lcd_cmd(0x01);
    lcd_cmd(0xC0);
    LCD_String("  Calculator");
    lcd_cmd(0x80);
  }
  else if(state == 2)
  {
    state = 0;
    init_stopWatch();
  }
  GPIOIntClear(GPIO_PORTF_BASE,GPIO_INT_PIN_4 );
}
