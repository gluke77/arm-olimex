#ifndef _COMMON_H_INCLUDED
#define _COMMON_H_INCLUDED

#include <LPC22xx.H>
#include <inttypes.h>

#ifdef __cplusplus__
extern "C" {
#endif /* __cplusplus__ */

#define OSCILLATOR_CLOCK_FREQUENCY  (14745600)

unsigned int processor_clock(void);
unsigned int peripheral_clock(void);

#define _BV(bit)                (1UL << bit)

#define TESTBIT(port, bit)	(port & _BV(bit))
#define SETBIT(port, bit)	(port |= _BV(bit))
#define CLEARBIT(port, bit)	(port &= ~(_BV(bit)))

#define SETBITS(port, bits)		((port) |= (bits))
#define CLEARBITS(port, mask)	((port) &= ~(mask))

#define STARTUP_DELAY {int i = 60000000; while (i--);}

#define LO_BYTE(word)	(word & 0xFF)
#define HI_BYTE(word)	((word >> 8) & 0xFF)

#ifdef __cplusplus__
}
#endif /* __cplusplus__ */

#endif /* _COMMON_H_INCLUDED */
