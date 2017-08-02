#include "stdint.h"

#ifndef __DELAY_H__
#define __DELAY_H__

void delay_ms_proc(uint16_t, uint16_t);

/* To string macro */
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

/* if clock not set, set standard clock 2MHz */
#ifndef SYSFREQ
#define SYSFREQ 16000000
#endif

/* Define const cycles value */
#define delay_ms_const 24

/* Calculate clocks for delay ms
   In other words, convert time to clocks needed for delay */
#define _ms_clks(time) ((time) * ((SYSFREQ) / 1000))

/* Calculating loops */
#define _loop_l(clocks) ((((clocks)-delay_ms_const)+1 - ((0*4)-1+4))/(65536*4-1+4)+1)
#define _loop_s(clocks,loop1) (((((clocks)-delay_ms_const)+1-4+1)-((loop1)-1)*(65536*4-1+4))/4)

#define param2(time) (_loop_l(_ms_clks(time)))
#define param1(time) (_loop_s(_ms_clks(time),_loop_l(_ms_clks(time))))
#define delay_ms_mod(time) ((_ms_clks(time)-delay_ms_const)-(((param2(time)-1)*(65536*4-1+4))-1+((param1(time)*4)-1+4)))

/* delay_ms macro */
#define delay_ms(time) asm(                                            \
   "pushw X" "\n"                                                               \
   "pushw Y" "\n"                                                               \
   "push CC" "\n"                                                               \
   "clrw X" "\n"                                                                \
   "clrw Y" "\n");                                                              \
   delay_ms_proc(param1(time),param2(time));                                    \
   switch(delay_ms_mod(time))                                                   \
   {                                                                            \
       case 1: { asm("nop"); break;}                                            \
       case 2: { asm(                                                           \
                     "nop" "\n"                                                 \
                     "nop" "\n"                                                 \
                    ); break;}                                                  \
       case 3: { asm(                                                           \
                     "nop" "\n"                                                 \
                     "nop" "\n"                                                 \
                     "nop" "\n"                                                 \
                    ); break;}                                                  \
       default: {break;}                                                        \
   }                                                                            \
                       asm(                                                     \
   "pop CC" "\n"                                                                \
   "popw Y" "\n"                                                                \
   "popw X" "\n"                                                                \
                          );

                          
/* Calculate clocks for delay us */
#define _us_clocks(time) ((time) * (SYSFREQ) / 1000000)

#define delay_us_const 8
                          
/* Calculate loop */
#define _us_loop(time) ((((_us_clocks(time)-delay_us_const)-3)/4)+1)
                          
/* Calculate MOD */
#define delay_us_mod(time) ((_us_clocks(time)-((_us_loop(time)-1)*4+3))%4)        
                          
/* delay_us macro */
#define delay_us(time) switch(_us_clocks(time))                                 \
                       {                                                        \
                           case 1: {asm("nop"); break;}                         \
                           case 2: {asm("nop" "\n"                              \
                                        "nop" "\n"); break;}                    \
                           case 3: {asm("nop" "\n"                              \
                                        "nop" "\n"                              \
                                        "nop" "\n"); break;}                    \
                           case 4: {asm("pushw X" "\n"                          \
                                        "popw X" "\n"); break;}                 \
                           case 5: {asm("pushw X" "\n"                          \
                                        "nop" "\n"                              \
                                        "popw X" "\n"); break;}                 \
                           case 6: {asm("pushw X" "\n"                          \
                                        "nop" "\n"                              \
                                        "nop" "\n"                              \
                                        "popw X" "\n"); break;}                 \
                           case 7: {asm("pushw X" "\n"                          \
                                        "ldw X, #0" "\n"                        \
                                        "nop" "\n"                              \
                                        "popw X" "\n"); break;}                 \
                           case 8: {asm("pushw X" "\n"                          \
                                        "ldw X, #0" "\n"                        \
                                        "ldw X, #0" "\n"                        \
                                        "popw X" "\n"); break;}                 \
                           case 9: {asm("pushw X" "\n"                          \
                                        "ldw X, #0" "\n"                        \
                                        "ldw X, #0" "\n"                        \
                                        "nop" "\n"                              \
                                        "popw X" "\n"); break;}                 \
                           case 10: {asm("pushw X" "\n"                         \
                                        "ldw X, #0" "\n"                        \
                                        "ldw X, #0" "\n"                        \
                                        "ldw X, #0" "\n"                        \
                                        "popw X" "\n"); break;}                 \
                       default: {                                               \
                       asm(                                                     \
              "pushw X" "\n"                                                    \
              "push CC" "\n"                                                    \
              "ldw X, #" TOSTRING(_us_loop(time)) "\n"                          \
              "decw X" "\n"                                                     \
              "jrne $-1" "\n"                                                   \
                          );                                                    \
              switch(delay_us_mod(time))                                        \
              {                                                                 \
                  case 1: { asm("nop"); break;}                                 \
                  case 2: { asm(                                                \
                                "nop" "\n"                                      \
                                "nop" "\n"                                      \
                               ); break;}                                       \
                  case 3: { asm(                                                \
                                "nop" "\n"                                      \
                                "nop" "\n"                                      \
                                "nop" "\n"                                      \
                               ); break;}                                       \
                  default: {break;}                                             \
              }                                                                 \
                           asm(                                                 \
              "pop CC" "\n"                                                     \
              "popw X" "\n"                                                     \
                  );}}
                          
#endif
