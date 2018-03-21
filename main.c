#include "NU32.h"          // config bits, constants, funcs for startup and UART
#include "encoder.h"// include other header files here
#include "current_controller.h"
#include "utilities.h"
#include <stdio.h>

#define VOLTS_PER_COUNT (3.3/1024)
#define CORE_TICK_TIME 25    // nanoseconds between core ticks
#define SAMPLE_TIME 10       // 10 core timer ticks = 250 ns
#define DELAY_TICKS 20000 // delay 1/2 sec, 20 M core ticks, between messages

#define PLOTPTS 200
#define DECIMATION 10

#define BUF_SIZE 200


static volatile int pwm = 0;

void adc_intialize(void);


void __ISR(_TIMER_2_VECTOR, IPL5SOFT) Controller(void)
{

  static int counter=0;
  static int plotind=0;
  static int decctr=0;
  static int adcval=0;
  static float u = 0;
  static float unew = 0;


  switch (get_mode()){
    case PWM:
    {
      OC1RS = abs(pwm) * 20;
      if(pwm<0){
        LATDbits.LATD8 = 1; // setting motor direction, phase
      }
      else {
        LATDbits.LATD8 = 0;
      }
      break;


    case IDLE:
    {

      pwm = 0;
      OC1RS = 0;            // 0 duty cycle => H-bridge in brake mode
      break;
}
}
  }


  IFS0bits.T2IF = 0;
}



int adc_sample_convert(int pin) { // sample & convert the value on the given
                                           // adc pin the pin should be configured as an
                                           // analog input in AD1PCFG

    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) {
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}



int main()
{
  unsigned int sample = 0,elapsed=0;
  float adc_current = 0.0;
  char buffer[BUF_SIZE];
  adc_intialize();
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;
  __builtin_disable_interrupts();
  current_init();
  encoder_init(); // in future, initialize modules or peripherals here
  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a':
      {
        _CP0_SET_COUNT(0);                    // set the core timer count to zero
        sample = adc_sample_convert(0);
        sprintf(buffer,"%d\r\n",sample);

        NU32_WriteUART3(buffer);
        break;
      }

      case 'b':
      {
        _CP0_SET_COUNT(0);                    // set the core timer count to zero
        sample = adc_sample_convert(0);
        adc_current = (float) (1.98*sample - 1020);
        sprintf(buffer,"%f\r\n",adc_current);

        NU32_WriteUART3(buffer);
        break;
      }



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

      case 'f':                      // dummy command for demonstration purposes
      {
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer,"%d", &pwm);
        set_mode(PWM);
        sprintf(buffer,"%d\r\n",pwm);
        NU32_WriteUART3(buffer);
break;
      }

      case 'p':
      {
        set_mode(IDLE);
        sprintf(buffer,"%d\r\n",0);
        NU32_WriteUART3(buffer);
        break;
}

      case 'r':                      // get mode
            {
              sprintf(buffer, "%d\r\n", get_mode());
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
  IFS0bits.T1IF = 0;     //reset the interrupt flag
  return 0;
}


void adc_intialize(void){
  AD1PCFGbits.PCFG0 = 0;
  AD1CON3bits.ADCS = 2;
  AD1CON1bits.ADON = 1;



}
