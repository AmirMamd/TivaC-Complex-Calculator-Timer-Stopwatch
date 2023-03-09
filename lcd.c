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

#include "DIO.h"
#include "lcd.h"

#define LCD_LED GPIO_PORTB_DATA_R

// initialize the LCD
void lcd_init()
{
  // open Port B
  SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
  
  //check port b is opened.
  while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOB));
  //
    // open pins 0,1,2,4,5,6,7 in port B for the LCD
   GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
   //
   delay(50000);
   //
   
   lcd_cmd(0x33);
   lcd_cmd(0x32); // 4 bits mode
   lcd_cmd(0x28); 
   lcd_cmd(0x0c);   //Display on, cursor off
   lcd_cmd(0x06);   //Increment cursor (shift cursor to right)
   lcd_cmd(0x01);  // Clear display screen 
   lcd_cmd(0x80);  // Force cursor to the beginning ( 1st line)
   lcd_cmd(0x0f);  //Display on, cursor blinking
   lcd_cmd(0xC0);  //Force cursor to the beginning ( 2nd line)
   LCD_String(" Super  Project ");
   lcd_cmd(0x80);
}

void lcd_chr(unsigned char data)
{
  LCD_LED  = (LCD_LED & 0x0F) | (data & 0xF0);
  DIO_WritePin(&LCD_LED, 1, 0);    // R/W = 0 write mode
  DIO_WritePin(&LCD_LED, 0, 1);    // rs = 1 data register
  DIO_WritePin(&LCD_LED, 2, 1);    //// enable = 1
  delay(50000);
  
  DIO_WritePin(&LCD_LED, 2, 0);   // close enable
  
  delay(50000);
  LCD_LED = (LCD_LED & 0x0F) | (data << 4);
  DIO_WritePin(&LCD_LED, 2, 1);    //// enable = 1
  delay(50000);
  
  DIO_WritePin(&LCD_LED, 2, 0);   // close enable
  
  delay(50000);
  
}
// this function is applied to send commands to the LCD.
void lcd_cmd(unsigned char cmd)
{
  LCD_LED  = (LCD_LED & 0x0F) | (cmd & 0xF0);
  DIO_WritePin(&LCD_LED, 1, 0);    // R/W = 0 write mode
  DIO_WritePin(&LCD_LED, 0, 0);    // RS = 0 command Register
  DIO_WritePin(&LCD_LED, 2, 1);    //// enable = 1
  delay(50000);
  
  DIO_WritePin(&LCD_LED, 2, 0);   // close enable
  delay(50000);
  
  LCD_LED = (LCD_LED & 0x0F) | (cmd << 4);
  
  DIO_WritePin(&LCD_LED, 2, 1);    //// enable = 1
  delay(50000);
  
  DIO_WritePin(&LCD_LED, 2, 0);   // close enable
  
  delay(50000);
  
}
// it takes a string and calls the lcd_chr() and send to it character at once.
void LCD_String(unsigned char str[] )
{
  unsigned char counter =0;
  
  while (str[counter]!='\0')
  {
    
    lcd_chr( str[counter] );
    counter++;
  }
    
}