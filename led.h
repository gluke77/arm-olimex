#ifndef _LED_H_INCLUDED
#define _LED_H_INCLUDED

#include "common.h"

#ifdef __cplusplus__
extern "C" {
#endif /* __cplusplus__ */

#define LEDDIR   (IO0DIR)
#define LEDSET   (IO0SET)
#define LEDCLR   (IO0CLR)
#define LEDPIN   (IO0PIN)
#define LEDBIT   (30)

void led_on(void);
void led_off(void);

#define LED_OFF (LEDSET = 1 << LEDBIT)
#define LED_ON  (LEDCLR = 1 << LEDBIT)

#define TESTLED (!(LEDPIN & (1 << LEDBIT)))

void led_init(void);
void led_ms(int /* ms */);

#ifdef __cplusplus__
}
#endif /* __cplusplus__ */

#endif /* _LED_H_INCLUDED */
