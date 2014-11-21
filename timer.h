#ifndef _TIMER_H_INCLUDED
#define _TIMER_H_INCLUDED

#include "common.h"

#ifdef __cplusplus__
extern "C" {
#endif /* __cplusplus__ */

#define	TIMER_COUNT		(32)

extern volatile int timers[TIMER_COUNT];

void timer_init(void);
int start_timer(int /* delay_ms */);
void stop_timer(int /* timer_id */);
int timer_value(int /* timer_id */);
void delay_ms(int /* delay_ms */);
void timer_suspend(void);
void timer_resume(void);


extern volatile int timer_seconds_total;
extern volatile int timer_mseconds_total;


#ifdef __cplusplus__
}
#endif /* __cplusplus__ */

#endif /* _TIMER_H_INCLUDED */
