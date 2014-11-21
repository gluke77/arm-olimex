#ifndef _RTC_H_INCLUDED
#define _RTC_H_INCLUDED

#include "common.h"

#ifdef __cplusplus__
extern "C" {
#endif /* __cplusplus__ */

void rtc_init(void);
void delay(void);
void _delay_us(int);
void _delay_ms(int);

#ifdef __cplusplus__
}
#endif /* __cplusplus__ */

#endif /* _RTC_H_INCLUDED */
