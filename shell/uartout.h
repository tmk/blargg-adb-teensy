#ifndef UARTOUT_BAUD
	#define UARTOUT_BAUD 57600
#endif
	
#include <stdio.h>
#include <avr/io.h>

// Allow code to work regardless of what the UART registers are named
#if defined(UCSRA)
	#define UN(name,...) name##__VA_ARGS__
#elif defined(UCSR0A)
	#define UN(name,...) name##0##__VA_ARGS__
#elif defined(UCSR1A)
	#define UN(name,...) name##1##__VA_ARGS__
#endif

// Write character to UART, waiting if it's busy sending one already
static void uartout_write( char c )
{
	loop_until_bit_is_set( UN(UCSR,A), UN(UDRE) );
	UN(UDR) = c;
}

// FILE output device for use with stdio code
static int uartout_putchar( char c, FILE* s )
{
	(void) s;
	uartout_write( c );
	return 0;
}

// Trivial overhead if you aren't using stdio
static FILE uartout = FDEV_SETUP_STREAM( uartout_putchar, NULL, _FDEV_SETUP_WRITE );

// Sets baud rate. Returns &uartout for convenient assignment to stdout.
static FILE* uartout_init( void )
{
	#ifdef U2X
		UN(UCSR,A) = 1<<U2X;
		int const ubrr = F_CPU /  8.0 / UARTOUT_BAUD + 0.5 - 1;
	#else
		int const ubrr = F_CPU / 16.0 / UARTOUT_BAUD + 0.5 - 1;
	#endif
	UN(UBRR,H) = ubrr >> 8 & 0xFF;
	UN(UBRR,L) = ubrr >> 0 & 0xFF;
	UN(UCSR,B) = 1<<UN(TXEN); // transmit-only
	#ifdef URSEL
		UN(UCSR,C) = 1<<URSEL | 1<<UN(UCSZ,1) | 1<<UN(UCSZ,0);
	#else
		UN(UCSR,C) =            1<<UN(UCSZ,1) | 1<<UN(UCSZ,0);
	#endif
	return &uartout;
}

#undef UN
