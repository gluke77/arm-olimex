#ifndef _USART_H_INCLUDED
#define _USART_H_INCLUDED

// dir0 - p0.10
// dir1 - p0.11

#ifdef __cplusplus__
extern "C" {
#endif /* __cplusplus__ */

#define _USART0
#define _USART1

#include "common.h"
#include <stdlib.h>

#include "modbus.h"

#define USART_OK        ( 0 )
#define USART_BUSY      ( 1 )
#define USART_TRUNCATED ( 2 )

#define USART_RS232			(0)
#define USART_RS485_MASTER	(1)
#define USART_RS485_SLAVE	(2)
#define USART_MODE_MASK		(3)

#define USART_PROTOCOL_MODBUS	(0)
#define USART_PROTOCOL_101		(1)
#define USART_PROTOCOL_UZS		(2)
#define USART_PROTOCOL_SEC		(3)

#define USART_INT_DISABLE	(0)
#define USART_INT_ENABLE	(1 << 7)
#define USART_INT_MASK		(1 << 7)

///////////////////////////////////////////

#if defined _USART0

#define USART0_DIR_DIR  IO0DIR
#define USART0_DIR_SET  IO0SET
#define USART0_DIR_CLR  IO0CLR
#define USART0_DIR_PIN  (10)

#define USART0_RS485_READ   (USART0_DIR_CLR = 1 << USART0_DIR_PIN)
#define USART0_RS485_WRITE  (USART0_DIR_SET = 1 << USART0_DIR_PIN)

void usart0_init(int /* mode */, unsigned int /* baud */);
int usart0_getchar(void);
int usart0_putchar(int /* ch */);
void usart0_puts(char *);

#define	USART0_INBUF_SIZE	(64)

extern volatile char	usart0_inbuf[USART0_INBUF_SIZE];
extern volatile char	usart0_inbuf_pos;
extern volatile char	usart0_msg_ready;

#define USART0_RX_INT_ENABLE	SETBIT(U0IER, 0)
#define USART0_RX_INT_DISABLE	CLEARBIT(U0IER, 0)

#define USART0_RX_INT_ENABLED	TESTBIT(U0IER, 0)
#define USART0_RX_INT_DISABLED	(!USART0_RX_INT_ENABLED)

#define USART0_SET_8N1			(U0LCR = 0x03)
#define USART0_SET_7N2			(U0LCR = 0x06)

int	usart0_data_ready(void);
void usart0_init(int /* mode */, unsigned int /* baud */);
void usart0_write(void);
void usart0_read(void);
char usart0_getmode(void);
result_e usart0_cmd(char * /* req */, char * /* ack */,
	size_t /* ack_size */, short int /* delay */);

char	usart0_getprotocol(void);
void usart0_setprotocol_modbus(void);
void usart0_setprotocol_101(void);
void usart0_setprotocol_uzs(void);
void usart0_setprotocol_sec(void);

void usart0_rx_byte_modbus(char /* byte */);
void usart0_rx_byte_101(char /* byte */);
void usart0_rx_byte_uzs(char /* byte */);
void usart0_rx_byte_sec(char /* byte */);

#endif /* _USART0 */

#if defined _USART1

#define USART1_DIR_DIR  IO0DIR
#define USART1_DIR_SET  IO0SET
#define USART1_DIR_CLR  IO0CLR
#define USART1_DIR_PIN  (11)

#define USART1_RS485_READ   (USART1_DIR_CLR = 1 << USART1_DIR_PIN)
#define USART1_RS485_WRITE  (USART1_DIR_SET = 1 << USART1_DIR_PIN)

int usart1_getchar(void);
int usart1_putchar(int /* ch */);
void usart1_puts(char *);

#define	USART1_INBUF_SIZE	(64)

extern volatile char	usart1_inbuf[USART1_INBUF_SIZE];
extern volatile char	usart1_inbuf_pos;
extern volatile char	usart1_msg_ready;

#define USART1_RX_INT_ENABLE	SETBIT(U1IER, 0)
#define USART1_RX_INT_DISABLE	CLEARBIT(U1IER, 0)

#define USART1_RX_INT_ENABLED	TESTBIT(U1IER, 0)
#define USART1_RX_INT_DISABLED	(!USART1_RX_INT_ENABLED)

#define USART1_SET_8N1			(U1LCR = 0x03)
#define USART1_SET_7N2			(U1LCR = 0x06)

#define USART_PROTOCOL_MODBUS	(0)
#define USART_PROTOCOL_101		(1)
#define USART_PROTOCOL_UZS		(2)
#define USART_PROTOCOL_SEC		(3)

int	usart1_data_ready(void);
void usart1_init(int /* mode */, unsigned int /* baud */);
void usart1_write(void);
void usart1_read(void);
char usart1_getmode(void);
result_e usart1_cmd(char * /* req */, char * /* ack */,
	size_t /* ack_size */, short int /* delay */);

char usart1_getprotocol(void);
void usart1_setprotocol_modbus(void);
void usart1_setprotocol_101(void);
void usart1_setprotocol_uzs(void);
void usart1_setprotocol_sec(void);

void usart1_rx_byte_modbus(char /* byte */);
void usart1_rx_byte_101(char /* byte */);
void usart1_rx_byte_uzs(char /* byte */);
void usart1_rx_byte_sec(char /* byte */);

#endif /* _USART1 */

#ifdef __cplusplus__
}
#endif /* __cplusplus__ */

#endif /* _USART_H_INCLUDED */
