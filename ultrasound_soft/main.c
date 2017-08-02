#include "iostm8l051f3.h"
#include "intrinsics.h"
#include "stdint.h"
#include "delay.h"

#define MIN_F (80 / 2)
#define MAX_F (510 / 2)
#define BEEPH_F (0x07)
#define BEEPL_F (0xD0)
#define UPDATE (48)

void beep ()
{
  /* init Timer 2 */  
  TIM2_ARRH = BEEPH_F;
  TIM2_ARRL = BEEPL_F;           //tick 2 KHz
  
  /* wait 1 sec */
  __enable_interrupt();
  delay_ms(1000);
  __disable_interrupt();
}

void ultrasound_start(void)
{
  /* Timer 4 reinit */
  TIM4_ARR = UPDATE;             //tick update
  
  /* Timer 2 reinit */
  TIM2_ARRH = 0x00;
  TIM2_ARRL = MIN_F;              //tick 100 KHz
  
  __enable_interrupt();
}

void tim4_init()        //change freq timer
{
  CLK_PCKENR1_bit.PCKEN12 = 1;  //enable clock
  TIM4_PSCR = 0x0F;             //prescaler = 32768
    
  TIM4_IER_bit.UIE = 1;         //enable update interrupt
  TIM4_CR1_bit.ARPE = 1;        //enable interrupt
  TIM4_CR1_bit.CEN = 1;         //run timer
}

void tim2_init()        //sound timer
{
  CLK_PCKENR1_bit.PCKEN10 = 1;  //enable clock
  TIM2_PSCR = 1;                //prescaler = 2  
  
  TIM2_IER_bit.UIE = 1;         //enable update interrupt
  TIM2_CR1_bit.ARPE = 1;        //enable interrupt
  TIM2_CR1_bit.CEN = 1;         //run timer
}

void main( void )
{
  CLK_CKDIVR = 0;       //set freq as 16 MHz
  
  /* pins init */
  PB_ODR &= ~((1<<1) | (1<<2));
  PB_DDR |= (1<<1) | (1<<2);
  PB_CR1 |= (1<<1) | (1<<2);  
  
  tim2_init();
  beep(); 
  tim4_init();
  ultrasound_start();
  
  while(1)
  {    
    __wait_for_interrupt();
  }
}

uint8_t invert = 0;
#pragma vector=TIM2_OVR_UIF_vector
__interrupt void TIM2_UIF_IRQHandler(void)
{
  if(!invert)
  {
    PB_ODR_bit.ODR1 = 0;
    PB_ODR_bit.ODR2 = 1;
    invert = 0xFF;
  }
  else
  {
    PB_ODR_bit.ODR2 = 0;
    PB_ODR_bit.ODR1 = 1;
    invert = 0;
  }
    
  TIM2_SR1_bit.UIF = 0;
}

uint8_t countBack = 0;

#pragma vector=TIM4_UIF_vector
__interrupt void TIM4_UIF_IRQHandler(void)
{
  uint16_t temp = (TIM2_ARRH)<<8;
  temp += (TIM2_ARRL);
    
  if(!countBack)
  {
    /* counting up */
    
    if(temp < MAX_F)
    {
      temp++;
      TIM2_ARRH = (temp>>8);
      TIM2_ARRL = (temp & 0x00FF);
    }
    else
    {
      countBack = 0xFF;
    }
  }
  else
  {
    /* counting down */
    
    if(temp > MIN_F)
    {
      temp--;
      TIM2_ARRH = (temp>>8);
      TIM2_ARRL = (temp & 0x00FF);
    }
    else
    {
      countBack = 0;
    }
  }
  TIM4_SR1_bit.UIF = 0;
}



