#include "common.h"
#include "rtc.h"
#include "led.h"

void led_init(void)
{
  LEDDIR |= _BV(LEDBIT);

  CLEARBIT(PINSEL1, 28);
  CLEARBIT(PINSEL1, 29);

  led_off();
}

void led_on(void)
{
  LED_ON;
}

void led_off(void)
{
  LED_OFF;
}

void led_ms(int ms)
{
  led_on();
  _delay_ms(ms);
  led_off();
}
