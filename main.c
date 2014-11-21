//#include <__armlib.h>

#include "common.h"
#include "usart.h"
#include "rtc.h"
#include "led.h"
#include "timer.h"

#include "iap.h"

#include <string.h>
#include <stdio.h>

char msg[] = "Another one bites the dust!\n";
char buf[1024];
int	i;
char * pchar;

void do_timer(void)
{
}

int main(void)
{
	rtc_init();
	led_init();

	usart0_init(USART_RS232, 115200);
	usart1_init(USART_RS232, 115200);

	timer_init();
		
    led_ms(200);
    _delay_ms(200);
    led_ms(50);
    _delay_ms(200);
    led_ms(50);

	usart0_puts("Hello!\n");

	usart0_puts("Trying to clear flash\n");
	if (IAP_CMD_SUCCESS != (i = iap_clear_flash()))
	{
		sprintf(buf, "iap_clear error %d\n", i);
		usart0_puts(buf);
	}

	usart0_puts("Trying to write flash\n");	
		if (IAP_CMD_SUCCESS != (i = iap_copy_ram_to_flash(msg, 512)))
	{
		sprintf(buf, "iap_copy error %d\n", i);
		usart0_puts(buf);
	}

	if (IAP_CMD_SUCCESS != (i = iap_compare(IAP_FLASH_BEGIN, msg, sizeof(msg) >> 2 << 2)))
	{
		sprintf(buf, "iap_compare error %d\n", i);
		usart0_puts(buf);
	}

	usart0_puts("Trying to read flash\n");
	usart0_puts((char *)IAP_FLASH_BEGIN);

	while (1)
	{
//		_delay_ms(500);
	}
}

