#include "iostm8l051f3.h"
#include "stdint.h"

void delay_ms_proc(uint16_t loop1, uint16_t loop2)
{
  asm(
        "decw X" "\n"
        "jrne delay_ms_proc" "\n"
        "decw Y" "\n"
        "jrne delay_ms_proc" "\n"        
      ); 
}