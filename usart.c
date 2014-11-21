#include "common.h"
#include "usart.h"

#include "rtc.h"
#include "timer.h"
#include "modbus.h"

#include <string.h>

#if defined _USART0

volatile char	usart0_inbuf[USART0_INBUF_SIZE];
volatile char	usart0_msg_ready = 0;
volatile char	usart0_inbuf_pos = 0;

static char	usart0_mode;
static char	usart0_int_mode;
static char	usart0_protocol;

void usart0_recv(void) __irq;

void usart0_init(int mode, unsigned int baud)
{
	unsigned int divisor = peripheral_clock() / (16 * baud);
	SETBIT(U0LCR, 7); /* Enable DLAB */
	U0DLL = divisor & 0xFF;
	U0DLM = (divisor >> 8) & 0xFF;
	CLEARBIT(U0LCR, 7); /* Disable DLAB */
	PINSEL0 |= 1 << 0;
	PINSEL0 |= 1 << 2;
	PINSEL0 &= ~(1 << 1);
	PINSEL0 &= ~(1 << 3);
	U0FCR = 1;

	usart0_mode = mode & USART_MODE_MASK;
	usart0_int_mode = (mode & USART_INT_MASK);
	if (USART_INT_ENABLE == usart0_int_mode)
	{
		VICVectAddr5 = (unsigned int)usart0_recv;
		VICVectCntl5 = 0x00000026;
		CLEARBIT(VICIntSelect, 6);
		SETBIT(VICIntEnable, 6);
	}

	if (usart0_mode != USART_RS232)
	{
		USART0_DIR_DIR |= 1 << USART0_DIR_PIN;
		PINSEL0 &= ~(1 << 20);
		PINSEL0 &= ~(1 << 21);
	}
		
	usart0_setprotocol_modbus();
	
	if (USART_RS485_MASTER == usart0_mode)
	{
	//	CLEARBIT(UCSR0B, RXEN0);
		USART0_RS485_WRITE;
		USART0_RX_INT_DISABLE;
	}
	else if (USART_RS485_SLAVE == usart0_mode)
	{
	//	SETBIT(UCSR0B, RXEN0);
		USART0_RS485_READ; 
		USART0_RX_INT_ENABLE;
	}
	else if (USART_RS232 == usart0_mode)
	{
		USART0_RX_INT_ENABLE;
	}
}

int usart0_data_ready(void)
{
	return (U0LSR & 0x01);
}

int usart0_getchar(void)
{
 	while (!(U0LSR & 0x01));
	return U0RBR;
}

int usart0_putchar(int ch)
{
	while (!(U0LSR & 0x20));
	return (U0THR = ch);
}

void usart0_puts(char * buf)
{
	while (*buf)
		usart0_putchar(*buf++);
}

char usart0_getmode(void)
{
	return usart0_mode;
}

char usart0_getprotocol(void)
{
	return usart0_protocol;
}

void usart0_setprotocol_modbus(void)
{
	USART0_SET_7N2;
	usart0_protocol = USART_PROTOCOL_MODBUS;
}

void usart0_setprotocol_101(void)
{
	USART0_SET_8N1;
	usart0_protocol = USART_PROTOCOL_101;
}

void usart0_setprotocol_uzs(void)
{
	USART0_SET_8N1;
	usart0_protocol = USART_PROTOCOL_UZS;
}

void usart0_setprotocol_sec(void)
{
	USART0_SET_8N1;
	usart0_protocol = USART_PROTOCOL_SEC;
}

void usart0_write(void)
{
	if (usart0_mode != USART_RS232)
	{
		USART0_RS485_WRITE; 
		//CLEARBIT(UCSR0B, RXEN0);
		_delay_ms(5);
		USART0_RX_INT_DISABLE;
	}
}

void usart0_read(void)
{
	if (usart0_mode != USART_RS232)
	{
		while (!(U0LSR & 0x20));
		_delay_ms(2);
		//SETBIT(UCSR0B, RXEN0);
		USART0_RS485_READ; 
		_delay_ms(5);
		USART0_RX_INT_ENABLE;
	}
}

void usart0_recv(void) __irq
{
	char	byte;
	
	byte = U0IIR;
	
	if ((byte & 0x0F) == 0x04)
	{
		byte = U0RBR;

		switch (usart0_getprotocol())
		{
		case USART_PROTOCOL_MODBUS:
			usart0_rx_byte_modbus(byte);
			break;
		case USART_PROTOCOL_101:
			usart0_rx_byte_101(byte);
			break;
		case USART_PROTOCOL_UZS:
			usart0_rx_byte_uzs(byte);
			break;
		case USART_PROTOCOL_SEC:
			usart0_rx_byte_sec(byte);
			break;
		}
	}

	VICVectAddr = 0;
}

result_e usart0_cmd(char * req, char * ack, size_t ack_size, short int delay)
{
	result_e		res;
	char			timer_id;
	char			idx;
	
	if (USART_RS485_SLAVE == usart0_getmode())
		usart0_write();

	if (USART_PROTOCOL_MODBUS == usart0_getprotocol())
	{
		for (idx = 0; req[idx] != '\n'; idx++)
			usart0_putchar(req[idx]);
		usart0_putchar('\n');
	}
/*	else if (USART_PROTOCOL_101 == usart0_getprotocol())
	{
		for (idx = 0; req[idx] != 0x0d; idx++)
			usart0_putchar(req[idx]);
		usart0_putchar(0x0d);
	}
	else if (USART_PROTOCOL_SEC == usart0_getprotocol())
	{
		for (idx = 0; idx < 4; idx++)
			usart0_putchar(req[idx]);
	}
*/	
	if (NULL == ack)
	{
		if (USART_RS485_SLAVE == usart0_getmode())
			usart0_read();
		return RESULT_OK;
	}

	return RESULT_BAD_ACK;
/*	
	usart0_read();
	
	res = RESULT_TIMEOUT;
		
	timer_id = start_timer(delay);
		
	while (timer_value(timer_id))
	{
		USART0_RX_INT_DISABLE;
		if (usart0_msg_ready)
		{
			res = RESULT_OK;
			usart0_msg_ready = 0;
			memcpy(ack, usart0_inbuf, (ack_size < USART0_INBUF_SIZE)?ack_size:USART0_INBUF_SIZE);
			stop_timer(timer_id);
			break;
		}
		USART0_RX_INT_ENABLE;	
	}
	stop_timer(timer_id);

	if (USART_RS485_MASTER == usart0_getmode())
		usart0_write();	

	return res;
*/
}


void usart0_rx_byte_modbus(char byte)
{
	if (':' == byte)
	{
		usart0_inbuf[0] = ':';
		usart0_inbuf_pos = 1;
		usart0_msg_ready = 0;
	}
	else if (usart0_inbuf_pos)
		usart0_inbuf[usart0_inbuf_pos++] = byte;
		
	if (':' == usart0_inbuf[0] &&
		'\r' == usart0_inbuf[usart0_inbuf_pos-2] &&
		'\n' == usart0_inbuf[usart0_inbuf_pos-1])
	{
		usart0_msg_ready = 1;
		usart0_inbuf_pos = 0;
	}
	else if (USART0_INBUF_SIZE == usart0_inbuf_pos)
	{
		usart0_inbuf_pos = 0;
		usart0_msg_ready = 0;
	}
}

void usart0_rx_byte_101(char byte)
{
	if (0x23 == byte)
	{
		usart0_inbuf[0] = 0x23;
		usart0_inbuf_pos = 1;
		usart0_msg_ready = 0;
	}
	else if (usart0_inbuf_pos)
		usart0_inbuf[usart0_inbuf_pos++] = byte;
		
	if (0x23 == usart0_inbuf[0] &&
		0x0d == usart0_inbuf[usart0_inbuf_pos-1])
	{
		usart0_msg_ready = 1;
		usart0_inbuf_pos = 0;
	}
	else if (USART0_INBUF_SIZE == usart0_inbuf_pos)
	{
		usart0_inbuf_pos = 0;
		usart0_msg_ready = 0;
	}
}

void usart0_rx_byte_uzs(char byte)
{
	if (('>' != byte) && ('\n' != byte))
	{
		usart0_inbuf[usart0_inbuf_pos++] = byte;
		usart0_msg_ready = 0;
	}
		
	if ('\r' == usart0_inbuf[usart0_inbuf_pos-1])
	{
		usart0_msg_ready = 1;
		if (usart0_inbuf_pos < USART0_INBUF_SIZE)
			usart0_inbuf[usart0_inbuf_pos++] = 0x00;
		usart0_inbuf_pos = 0;
	}
	else if (USART0_INBUF_SIZE == usart0_inbuf_pos)
	{
		memset(usart0_inbuf, 0x00, USART0_INBUF_SIZE);
		usart0_inbuf_pos = 0;
		usart0_msg_ready = 0;
	}
}

void usart0_rx_byte_sec(char byte)
{
	if (('~' == byte) && (0 == usart0_inbuf_pos))
	{
		usart0_msg_ready = 0;
		usart0_inbuf[usart0_inbuf_pos++] = byte;
	}
	else if ((usart0_inbuf_pos > 0) && (usart0_inbuf_pos < 4))
	{
		usart0_inbuf[usart0_inbuf_pos++] = byte;
		usart0_msg_ready = 0;
	}
	
	if (('~' == usart0_inbuf[0]) && (4 == usart0_inbuf_pos))
	{
		usart0_inbuf_pos = 0;
		usart0_msg_ready = 1;
	}
	else if (USART0_INBUF_SIZE == usart0_inbuf_pos)
	{
		memset(usart0_inbuf, 0x00, USART0_INBUF_SIZE);
		usart0_inbuf_pos = 0;
		usart0_msg_ready = 0;
	}
}

#endif /* _USART0 */

#if defined _USART1

volatile char	usart1_inbuf[USART1_INBUF_SIZE];
volatile char	usart1_msg_ready = 0;
volatile char	usart1_inbuf_pos = 0;

static char	usart1_mode;
static char	usart1_int_mode;
static char	usart1_protocol;

void usart1_recv(void) __irq;

void usart1_init(int mode, unsigned int baud)
{
	unsigned int divisor = peripheral_clock() / (16 * baud);
	SETBIT(U1LCR, 7); /* Enable DLAB */
	U1DLL = divisor & 0xFF;
	U1DLM = (divisor >> 8) & 0xFF;
	CLEARBIT(U1LCR, 7); /* Disable DLAB */
	PINSEL0 |= 1 << 16;
	PINSEL0 |= 1 << 18;
	PINSEL0 &= ~(1 << 17);
	PINSEL0 &= ~(1 << 19);
	U1FCR = 1;

	usart1_mode = mode & USART_MODE_MASK;
	usart1_int_mode = mode & USART_INT_MASK;

	if (USART_INT_ENABLE == usart1_int_mode)
	{
		VICVectAddr6 = (unsigned int)usart1_recv;
		VICVectCntl6 = 0x00000027;
		CLEARBIT(VICIntSelect, 7);
		SETBIT(VICIntEnable, 7);
	}

	if (usart1_mode != USART_RS232)
	{
		USART1_DIR_DIR |= 1 << USART1_DIR_PIN;
		PINSEL0 &= ~(1 << 22);
		PINSEL0 &= ~(1 << 23);
  	}
		
	usart1_setprotocol_modbus();
	
	if (USART_RS485_MASTER == usart1_mode)
	{
	//	CLEARBIT(UCSR0B, RXEN0);
		USART1_RS485_WRITE;
		USART1_RX_INT_DISABLE;
	}
	else if (USART_RS485_SLAVE == usart1_mode)
	{
	//	SETBIT(UCSR0B, RXEN0);
		USART1_RS485_READ; 
		USART1_RX_INT_ENABLE;
	}
	else if (USART_RS232 == usart1_mode)
	{
		USART1_RX_INT_ENABLE;
	}
}

int usart1_data_ready(void)
{
	return (U1LSR & 0x01);
}

int usart1_getchar(void)
{
 	while (!(U1LSR & 0x01));
	return U1RBR;
}

int usart1_putchar(int ch)
{
	while (!(U1LSR & 0x20));
	return (U1THR = ch);
}

void usart1_puts(char * buf)
{
	while (*buf)
		usart1_putchar(*buf++);
}

char usart1_getmode(void)
{
	return usart1_mode;
}

char usart1_getprotocol(void)
{
	return usart1_protocol;
}

void usart1_setprotocol_modbus(void)
{
	USART1_SET_7N2;
	usart1_protocol = USART_PROTOCOL_MODBUS;
}

void usart1_setprotocol_101(void)
{
	USART1_SET_8N1;
	usart1_protocol = USART_PROTOCOL_101;
}

void usart1_setprotocol_uzs(void)
{
	USART1_SET_8N1;
	usart1_protocol = USART_PROTOCOL_UZS;
}

void usart1_setprotocol_sec(void)
{
	USART1_SET_8N1;
	usart1_protocol = USART_PROTOCOL_SEC;
}

void usart1_write(void)
{
	if (usart1_mode != USART_RS232)
	{
		USART1_RS485_WRITE; 
		//CLEARBIT(UCSR0B, RXEN0);
		_delay_ms(5);
		USART1_RX_INT_DISABLE;
	}
}

void usart1_read(void)
{
	if (usart1_mode != USART_RS232)
	{
		while (!(U1LSR & 0x20));
		_delay_ms(2);
		//SETBIT(UCSR0B, RXEN0);
		USART1_RS485_READ; 
		_delay_ms(5);
		USART1_RX_INT_ENABLE;
	}
}

void usart1_recv(void) __irq
{
	char	byte;
	
	byte = U1IIR;
	
	if ((byte & 0x0F) == 0x04)
	{
		byte = U1RBR;

		switch (usart1_getprotocol())
		{
		case USART_PROTOCOL_MODBUS:
			usart1_rx_byte_modbus(byte);
			break;
		case USART_PROTOCOL_101:
			usart1_rx_byte_101(byte);
			break;
		case USART_PROTOCOL_UZS:
			usart1_rx_byte_uzs(byte);
			break;
		case USART_PROTOCOL_SEC:
			usart1_rx_byte_sec(byte);
			break;
		}
	}

	VICVectAddr = 0;
}


result_e usart1_cmd(char * req, char * ack, size_t ack_size, short int delay)
{
	result_e		res;
	char			timer_id;
	char			idx;
	
	if (USART_RS485_SLAVE == usart1_getmode())
		usart1_write();

	if (USART_PROTOCOL_MODBUS == usart1_getprotocol())
	{
		for (idx = 0; req[idx] != '\n'; idx++)
			usart1_putchar(req[idx]);
		usart1_putchar('\n');
	}
/*	else if (USART_PROTOCOL_101 == usart1_getprotocol())
	{
		for (idx = 0; req[idx] != 0x0d; idx++)
			usart1_putchar(req[idx]);
		usart1_putchar(0x0d);
	}
	else if (USART_PROTOCOL_SEC == usart1_getprotocol())
	{
		for (idx = 0; idx < 4; idx++)
			usart1_putchar(req[idx]);
	}
*/	
	if (NULL == ack)
	{
		if (USART_RS485_SLAVE == usart1_getmode())
			usart1_read();
		return RESULT_OK;
	}

	//return RESULT_BAD_ACK;
	
	usart1_read();
	
	res = RESULT_TIMEOUT;
		
	timer_id = start_timer(delay);
		
	while (timer_value(timer_id))
	{
		if (USART_INT_ENABLE == usart1_int_mode)
			USART1_RX_INT_DISABLE;
		
		if (usart1_msg_ready)
		{
			res = RESULT_OK;
			usart1_msg_ready = 0;
			memcpy(ack, (const void *)usart1_inbuf, (ack_size < USART1_INBUF_SIZE)?ack_size:USART1_INBUF_SIZE);
			stop_timer(timer_id);
			break;
		}
		
		if (USART_INT_ENABLE == usart1_int_mode)
			USART1_RX_INT_ENABLE;	
	}
	stop_timer(timer_id);

	if (USART_RS485_MASTER == usart1_getmode())
		usart1_write();	

	return res;
}


void usart1_rx_byte_modbus(char byte)
{
	if (':' == byte)
	{
		usart1_inbuf[0] = ':';
		usart1_inbuf_pos = 1;
		usart1_msg_ready = 0;
	}
	else if (usart1_inbuf_pos)
		usart1_inbuf[usart1_inbuf_pos++] = byte;
		
	if (':' == usart1_inbuf[0] &&
		'\r' == usart1_inbuf[usart1_inbuf_pos-2] &&
		'\n' == usart1_inbuf[usart1_inbuf_pos-1])
	{
		usart1_msg_ready = 1;
		usart1_inbuf_pos = 0;
	}
	else if (USART1_INBUF_SIZE == usart1_inbuf_pos)
	{
		usart1_inbuf_pos = 0;
		usart1_msg_ready = 0;
	}
}

void usart1_rx_byte_101(char byte)
{
	if (0x23 == byte)
	{
		usart1_inbuf[0] = 0x23;
		usart1_inbuf_pos = 1;
		usart1_msg_ready = 0;
	}
	else if (usart1_inbuf_pos)
		usart1_inbuf[usart1_inbuf_pos++] = byte;
		
	if (0x23 == usart1_inbuf[0] &&
		0x0d == usart1_inbuf[usart1_inbuf_pos-1])
	{
		usart1_msg_ready = 1;
		usart1_inbuf_pos = 0;
	}
	else if (USART1_INBUF_SIZE == usart1_inbuf_pos)
	{
		usart1_inbuf_pos = 0;
		usart1_msg_ready = 0;
	}
}

void usart1_rx_byte_uzs(char byte)
{
	if (('>' != byte) && ('\n' != byte))
	{
		usart1_inbuf[usart1_inbuf_pos++] = byte;
		usart1_msg_ready = 0;
	}
		
	if ('\r' == usart1_inbuf[usart1_inbuf_pos-1])
	{
		usart1_msg_ready = 1;
		if (usart1_inbuf_pos < USART1_INBUF_SIZE)
			usart1_inbuf[usart1_inbuf_pos++] = 0x00;
		usart1_inbuf_pos = 0;
	}
	else if (USART1_INBUF_SIZE == usart1_inbuf_pos)
	{
		memset(usart1_inbuf, 0x00, USART1_INBUF_SIZE);
		usart1_inbuf_pos = 0;
		usart1_msg_ready = 0;
	}
}

void usart1_rx_byte_sec(char byte)
{
	if (('~' == byte) && (0 == usart1_inbuf_pos))
	{
		usart1_msg_ready = 0;
		usart1_inbuf[usart1_inbuf_pos++] = byte;
	}
	else if ((usart1_inbuf_pos > 0) && (usart1_inbuf_pos < 4))
	{
		usart1_inbuf[usart1_inbuf_pos++] = byte;
		usart1_msg_ready = 0;
	}
	
	if (('~' == usart1_inbuf[0]) && (4 == usart1_inbuf_pos))
	{
		usart1_inbuf_pos = 0;
		usart1_msg_ready = 1;
	}
	else if (USART1_INBUF_SIZE == usart1_inbuf_pos)
	{
		memset(usart1_inbuf, 0x00, USART1_INBUF_SIZE);
		usart1_inbuf_pos = 0;
		usart1_msg_ready = 0;
	}
}

#endif /* _USART1 */
