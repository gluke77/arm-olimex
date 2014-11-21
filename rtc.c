#include "common.h"
#include "rtc.h"

void rtc_init(void)
{
  PREINT = (int)(peripheral_clock() / 32768) - 1;
  PREFRAC = peripheral_clock() - (PREINT + 1) * 32768;

  SETBIT(CCR, 0);
}

void delay(void)
{
  int i = 10;
  while (i--);
}

void _delay_us(int d)
{
	int count = peripheral_clock() / 1000000;
	count *= d;

	while (count--);
}

void _delay_ms(int d)
{
	int count = peripheral_clock() / 1000;
	count *= d;

	while (count--);
}

//void delay_ms(int ms)
//{
//  int start_tic = CTC & 0xFE;
//  int tics = 32768 * ms / 1000;
//  int tics_elapsed;
//
//  do 
//  {
//    tics_elapsed = CTC & 0xFE - start_tic;
//    if (0 > tics_elapsed)
//      tics_elapsed += 32768;
//  } while (tics_elapsed < tics);
//
//  int i = 1000 * ms;
//  while (i--)
//    delay();
//
//}

