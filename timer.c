#include "common.h"
#include "timer.h"

#include "led.h"
#include "rtc.h"

#define TIMER_TICS_PER_SECOND (1002)

volatile int  timers[TIMER_COUNT];

volatile int  timer_seconds_total = 0;
volatile int  timer_mseconds_total = 0;
volatile int  timer_seconds_timeout = TIMER_TICS_PER_SECOND;

void do_timer(void);

void timer0_isr(void) __irq
{
	int idx;
  
	if (T0IR & 0x00000001)  // interrupt on match 0
	{
	    for (idx = 0; idx < TIMER_COUNT; idx++)
    		if (0 < timers[idx])
				timers[idx]--;
			
#ifdef _BEEP_H_INCLUDED
    	if (1 == timers[1])
      		BEEP_OFF;
#endif /* _BEEP_H_INCLUDED */
			
    	do_timer();
	
		if (!timer_seconds_timeout--)
		{
			timer_seconds_total++;
			timer_seconds_timeout = TIMER_TICS_PER_SECOND;
		}
		timer_mseconds_total++;

		T0IR |= 0x00000001;
		VICVectAddr = 0;
	}
	
	VICVectAddr = 0;
}

void timer_init(void)
{
	int i;

	for (i = 0; i < TIMER_COUNT; i++)
		stop_timer(i);

  T0TCR = 0x00000002;
  T0MR0 = peripheral_clock() / 1000;      // match 0
  T0MCR |= 0x00000003; // reset and interrupt on match 0
  T0TCR = 0x00000001; // enable and reset timer;

  VICVectAddr4 = (unsigned int)timer0_isr;
  VICVectCntl4 = 0x00000024;
  VICIntSelect &= ~(0x00000001 << 4);
  VICIntEnable |= 0x00000001 << 4;
}

int timer_value(int timer_id)
{
  return ((0 <= timer_id) && (timer_id < TIMER_COUNT))?timers[timer_id]:0;
}

int start_timer(int delay)
{
  int timer_id;
	
  if (0 > delay)
    return TIMER_COUNT;

  for (timer_id = 2; timer_id < TIMER_COUNT; timer_id++)
    if (-1 == timers[timer_id])
    {
      timers[timer_id] = delay;
      break;
    }
			
  return timer_id;
}

void delay_ms(int delay)
{
  if (0 < delay)
  {
    timers[0] = delay;
    while (timer_value(0));
  }
}

void stop_timer(int timer_id)
{
  if ((1 < timer_id) && (timer_id < TIMER_COUNT))
    timers[timer_id] = -1;
}

void timer_suspend(void)
{
	T0TCR = 0x00000000;
}

void timer_resume(void)
{
	T0TCR = 0x00000001;
}
