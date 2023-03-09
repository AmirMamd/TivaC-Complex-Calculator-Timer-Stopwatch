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

/*
  Include the Header files for the files that contain the needed functions.
*/
#include "lcd.h"
#include "dio.h"

/*
  Prototypes for the functions.
*/
char* convertToString(int num);
void clean(char *var) ;
int convertToNum(const char* str);
void tostring(char str[], int num);
void getResult(char op);

void initialization(void)
{
    /*
      Enable the Ports (C-E-F).
    */
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

    /*
      Wait for the ports (C-E-F) to be ready.
    */
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOC));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOE));
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));

    /*
      Initialize the GPIOF pin configuration. 
      Set pins 4 as input, SW controlled.
    */
     GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
     
     /*
       Set pin 4 as a pull up resistance that is always having '1' but when pressed it takes the value '0'.
     */
     GPIO_PORTF_PUR_R |= 0x10;
     
      /*
       Open the lock for PORT(F)
     */
     GPIO_PORTF_LOCK_R |= 0x4C4F434B;

     /*
      Initialize the GPIO (C) pin configuration. 
      Set pins 4,5,6,7 as input for the keypad.
    */
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE,GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    /*
      Initialize the GPIO (E) pin configuration. 
      Set pins 0,1,2,3 as Output for the keypad.
    */
    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    
    /*
       Set pin (4,5,6,7) as a pull down resistance that is always having '0' but when pressed it takes the value '1'.
    */
    GPIO_PORTC_PDR_R |= 0xf0;//////////////////////////////////
    
    
    /*
      Initialize the GPIOF pin configuration. 
      Set pins 1 as output.(Red Led)
    */
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1);
    
    /*
      Set the interrupt in GPIO(F)
      Set pin 4 that makes the interrupt
      set the interrupt at the falling edge of pin 4 ( when it changes from '1' to '0'). 
    */
    GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_4, GPIO_FALLING_EDGE);
    GPIOIntEnable(GPIO_PORTF_BASE, GPIO_PIN_4);
}


/*
  calculations
  x operator y  = total
*/
char x [50];                                // Array for holding the first number in the calculations.
char y [50];                                // Array for holding the second number in the calculations.
char total[100];                            // Array for holding the total number of the calculations.
bool isOperatorPressed = false;             // boolean holding the flag to check if an operation is selected or not.
int xindex = 0;                             // xindex to access the elements in array x[50].
int yindex = 0;                             // yindex to access the elements in array y[50]. 
char operator ;                             // character for holding the operator.
  
void calc(void)
{

  unsigned char portval;                     //variable that holds the value of column that have '1' in the keypad.
  
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

          // check if the row 0 has the value '1' then enter it to check if any button is pressed
          if(portval == 0x10)
          {
            switch(i){
              case 0:
                // check if no operator is chosen, then store the value '1' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '1'; 
                    xindex++;         // increase the xindex by '1'.         
                }
                // if the operator is chosen, then store the value '1' in array y[yindex].
                else
                {
                    y[yindex] = '1';
                    yindex++;       // increase the yindex by '1'.
                }
                lcd_chr('1');       // print '1'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);   // making delay using the tiva delay function.  
                break;
                
              case 1:
                // check if no operator is chosen, then store the value '2' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '2';
                    xindex++;           // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '2' in array y[yindex].
                else
                {
                    y[yindex] = '2';
                    yindex++;         // increase the yindex by '1'.
                }
                lcd_chr('2');       // print '2'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
               break;
              case 2:
                // check if no operator is chosen, then store the value '3' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '3';
                    xindex++;          // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '3' in array y[yindex].
                else
                {
                    y[yindex] = '3';
                    yindex++;                    // increase the yindex by '1'.
                }
                lcd_chr('3');                   // print '3'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
              break;
              
              case 3:
                isOperatorPressed = true;
                lcd_chr('+');                   // print '+'  which is the button pressed in the keypad.
                operator = '+';
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
                break;
            }
          }
          // check if the row 1 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x20)
          {
           switch(i){
              case 0:
                // check if no operator is chosen, then store the value '4' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '4';
                    xindex++;                    // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '4' in array y[yindex].
                else
                {
                    y[yindex] = '4';
                    yindex++;                   // increase the yindex by '1'.
                }
                lcd_chr('4');                  // print '4'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
               break;
              case 1:
                // check if no operator is chosen, then store the value '5' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '5';
                    xindex++;        // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '5' in array y[yindex].
                else
                {
                    y[yindex] = '5';
                    yindex++;                     // increase the yindex by '1'.
                }
                lcd_chr('5');                    // print '5'  which is the button pressed in the keypad.
                 SysCtlDelay(2000000);          // making delay using the tiva delay function.
                break;
              case 2:
                // check if no operator is chosen, then store the value '6' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '6';
                    xindex++;                     // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '6' in array y[yindex].
                else
                {
                    y[yindex] = '6';
                    yindex++;                    // increase the yindex by '1'.
                }
                lcd_chr('6');                   // print '6'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
               break;
              case 3:
                isOperatorPressed = true;
                lcd_chr('-');                   // print '-'  which is the button pressed in the keypad.
                operator = '-';
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
                break;
            }
          }
         // check if the row 2 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x40)
        {
            switch(i){
              case 0:
                // check if no operator is chosen, then store the value '7' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '7';
                    xindex++;                    // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '7' in array y[yindex].
                else
                {
                    y[yindex] = '7';
                    yindex++;                    // increase the yindex by '1'.
                }
                lcd_chr('7');                   // print '7'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
                break;
              case 1:
                // check if no operator is chosen, then store the value '8' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '8';
                    xindex++;                 // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '8' in array y[yindex].
                else
                {
                    y[yindex] = '8';
                    yindex++;                // increase the yindex by '1'.
                }
                lcd_chr('8');                // print '8'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);        // making delay using the tiva delay function.
                break;
              case 2:
                // check if no operator is chosen, then store the value '9' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '9';
                    xindex++;        // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '9' in array y[yindex].
                else
                {
                    y[yindex] = '9';
                    yindex++;       // increase the yindex by '1'.
                }
                lcd_chr('9');       // print '9'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
               break;
               
              case 3:
                isOperatorPressed = true;
                lcd_chr('/');                   // print '/'  which is the button pressed in the keypad.
                operator = '/';
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
                break;
            }
          }
         // check if the row 3 has the value '1' then enter it to check if any button is pressed
          else if(portval == 0x80)
          {
            switch(i){
              case 0:
                isOperatorPressed = true;
                lcd_chr('*');                   // print '*'  which is the button pressed in the keypad.
                operator = '*';
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
                break;
              case 1:
                // check if no operator is chosen, then store the value '0' in array x[xindex].
                if(isOperatorPressed == false)
                {
                    x[xindex] = '0';
                    xindex++;        // increase the xindex by '1'.
                }
                // if the operator is chosen, then store the value '0' in array y[yindex].
                else
                {
                    y[yindex] = '0';
                    yindex++;       // increase the yindex by '1'.
                }
                lcd_chr('0');       // print '0'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
              break;
              case 2:
                lcd_chr('#');                   // print '#'  which is the button pressed in the keypad.
                SysCtlDelay(2000000);          // making delay using the tiva delay function.
               break;
              case 3:
                lcd_chr('=');             // print '='  which is the button pressed in the keypad.
                delay(1000000);          // making delay using the tiva delay function.
                
                if(isOperatorPressed == true)         // if the '=' is pressed and an operation is chosen then calls a function to calculate the result
                {
                   getResult(operator);    // calling the function to get the result.
                  
                }
                else
                {
                  LCD_String("No Operation");      // if no operation is chosen then show this message.
                }
               break;
            }
          }
  }
}
/*
  function to get the result of the calculations by getting both arrays x and y
  and convert them to numbers and perform the operation on it then store it in a new array called tetal.
*/
void getResult(char op)
{
                  char tetal[100];
                  int xnum = 0;
                  xnum = convertToNum(x);      // convert the array of character x to numbers to perform calculations
                  int ynum = 0;
                  ynum = convertToNum(y);     // convert the array of character y to numbers to perform calculations
                  int totalNum = 0;
                  // if the operator is '+' then do an addition operation
                  if(op == '+')
                  {
                    totalNum = xnum + ynum;
                  }
                  // if the operator is '-' then do an subtraction operation
                  else if(op == '-')
                  {
                    totalNum = xnum - ynum;                   
                  }
                  // if the operator is '/' then do an division operation
                  else if(op == '/')
                  {
                    totalNum = xnum / ynum;                  
                  }
                  // if the operator is '*' then do an multiplication operation
                  else if(op == '*')
                  {
                    totalNum = xnum * ynum;                   
                  }
                  tostring(tetal, totalNum);         // convert the number totalNum to array of characters called tetal
                  LCD_String(tetal);                 // print tetal ( the result)
                  SysCtlDelay(8000000);              // making delay using the tiva delay function.          
                  
                  /* after the delay 
                     reset the calculator mode so it can perform new operation
                  */                  
                  lcd_cmd(0x01);                 // erase all the data shown on the LCD.                  
                  lcd_cmd(0x80);                 // get the cursor to the beginning of the first row
                  isOperatorPressed = false;     // reset the isOperatorPressed variable
                  clean(x);                      //clean array x[50]
                  clean(y);                      //clean array y[50]
                  xindex = 0;                    //reset xindex
                  yindex = 0;                    //reset yindex
                  lcd_cmd(0xC0);                 // goes to the second line in LCD
                  LCD_String("  Calculator");    // show the mode name
                  lcd_cmd(0x80);                 // get the cursor to the beginning of the first row 
}

/*
  convert the given number in the paramters and convert it to an array of characters.
*/
char* convertToString(int num)
{
  char finalTotalArray[100] = '0';
  char totalArray[100] = '0';
  long number = num;
  int counter = 0;
  int counter1 = 0;
  while(number != 0)
  {
    int numbaya = number % 10;
    number/=10;
    char temp = numbaya + '0';
    totalArray[counter] = temp;
    counter++;
  }
  /*the resutled array is inversed so we make another loop and array to reverse them again to be in the right order*/
  while(counter - 1 >= 0)
  {
    finalTotalArray[counter1] = totalArray[counter - 1];
    counter1++;
    counter--;
  }
  return (char*)finalTotalArray;
}

/*
  function to clean up the given array by giving each index by '\0' NULL.
*/
void clean(char *var) {
    int i = 0;
    while(var[i] != '\0') {
        var[i] = '\0';
        i++;
    }
}

/*
  convert the given array to a number
*/
int convertToNum(const char* str){
    int num = 0;
    int i = 0;
    bool isNegetive = false;
    if(str[i] == '-'){
        isNegetive = true;
        i++;
    }
    while (str[i] && (str[i] >= '0' && str[i] <= '9')){
        num = num * 10 + (str[i] - '0');
        i++;
    }
    if(isNegetive) num = -1 * num;
    return num;
}

/*
  convert the given number in the paramters and convert it to an array of characters.
*/
void tostring(char str[], int num)
{
    int i, rem, len = 0, n;
 
    n = num;
    while (n != 0)
    {
        len++;
        n /= 10;
    }
    for (i = 0; i < len; i++)
    {
        rem = num % 10;
        num = num / 10;
        str[len - (i + 1)] = rem + '0';
    }
    str[len] = '\0';
}