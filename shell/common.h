#include <stdbool.h>
#include <limits.h>
#include <stdint.h>

#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/power.h>
#include <avr/io.h>

// Otherwise inline is ignored by compiler
#define inline inline __attribute__((always_inline))

typedef uint8_t byte;

// Reduces size of main
int main(void) __attribute__ ((OS_main)); // OS_main slightly smaller than noreturn

#include "delay.h"

static void init_common( void )
{
	#ifdef CLKPR
		// CPU clock prescaler
		CLKPR = 0x80; // must first enable change of prescaler
		CLKPR = 0;
	#endif
}

static inline void reduce_power( void )
{
	#ifdef power_all_disable
		power_all_disable();
	#endif
	
	// Enable pull-ups to avoid floating inputs
	#ifdef DDRA
		DDRA = 0;
		PORTA = 0xFF;
	#endif
	DDRB = 0;
	DDRC = 0;
	DDRD = 0;
	PORTB = 0xFF;
	PORTC = 0xFF;
	PORTD = 0xFF;
}
