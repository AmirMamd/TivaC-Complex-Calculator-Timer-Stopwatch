
#include "inc/tm4c123gh6pm.h"
#include "bitwise_operation.h"
//void DIO_init(void)
//{
//    SYSCTL_RCGCGPIO_R |= 0X20;
//    while((SYSCTL_PRGPIO_R & 0X20)==0);
//    GPIO_PORTF_LOCK_R |= 0x4C4F434B;
//    GPIO_PORTF_CR_R|= 0X01;
//    GPIO_PORTF_DIR_R|= 0X0E;
//    GPIO_PORTF_PUR_R |= 0x11;
//    GPIO_PORTF_DEN_R|= 0X1F;
//}

// write in the port the new value.
void DIO_WritePort (unsigned long volatile * Port ,unsigned char value)
{
  *Port = value;
}

// write in the pin in a specific port the new value.
void DIO_WritePin(unsigned long  volatile* Port , int Pin, unsigned char value)
{
  if(value == 0)
  {
    clearBit( *Port,  Pin);
  }
  else if(value == 1)
  {
    setBit( *Port,  Pin);
  }
}

//read a pin in a specific port
char ReadPin(unsigned long  volatile* Port, int Pin)
{
    return getBit(*Port ,Pin);
}

// read a specific port
unsigned long ReadPort(unsigned long  volatile* Port)
{
  return *Port;
}
void delay(int timer)
{
  int count = 0;
  while(count <= timer)
  {
    count++;
  }
}

//toggle a specific pin in a specific port
void toggle(unsigned long  volatile* Port, int Pin)
{
  if(ReadPin(Port, Pin) == 1)
  {
    clearBit( *Port,  Pin);
  }
  else if(ReadPin(Port, Pin) == 0)
  {
    setBit( *Port,  Pin);
  } 
}