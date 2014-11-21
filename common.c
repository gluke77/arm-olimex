#include "common.h"

unsigned int processor_clock(void)
{
  return OSCILLATOR_CLOCK_FREQUENCY * (PLLCON & 1 ? (PLLCFG & 0xF) + 1 : 1);
}

unsigned int peripheral_clock(void)
{
  unsigned int divider;
  switch (VPBDIV & 3)
    {
      case 0:
        divider = 4;
        break;
      case 1:
        divider = 1;
        break;
      case 2:
        divider = 2;
        break;
    }
  return processor_clock() / divider;
}
