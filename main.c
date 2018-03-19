#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"// include other header files here
#include <stdio.h>

#define BUF_SIZE 200

int main() 
{
  char buffer[BUF_SIZE];
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;        
  __builtin_disable_interrupts();
  encoder_init(); // in future, initialize modules or peripherals here
  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {	
      case 'c':                      // dummy command for demonstration purposes
      {
        sprintf(buffer,"%d\r\n",encoder_counts());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'd':                      // dummy command for demonstration purposes
      { 

      	float degrees = (float)(360*(encoder_counts()-32768)/1792);
        sprintf(buffer,"%f\r\n",degrees);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'e':                      // dummy command for demonstration purposes
      {
        sprintf(buffer,"%d\r\n",encoder_reset());
        NU32_WriteUART3(buffer);
        break;
      }

      case 'x':                      // dummy command for demonstration purposes
      {
        int a = 0;
        int b = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &a);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &b);

        sprintf(buffer,"%d\r\n", a + b); // return the number + 1
        NU32_WriteUART3(buffer);
        break;
      }
      case 'q':
      {
        // handle q for quit. Later you may want to return to IDLE mode here. 
        break;
      }
      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
