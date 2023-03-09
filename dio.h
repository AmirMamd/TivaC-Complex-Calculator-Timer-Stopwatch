void DIO_init(void);
void DIO_WritePort (unsigned long volatile * Port ,unsigned char value);
void DIO_WritePin(unsigned long  volatile* Port , int Pin, unsigned char value);
char ReadPin(unsigned long  volatile* Port, int Pin);
unsigned long ReadPort(unsigned long  volatile* Port);
void delay(int timer);
void toggle(unsigned long  volatile* Port, int Pin);