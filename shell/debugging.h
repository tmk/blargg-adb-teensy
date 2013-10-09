#include <avr/pgmspace.h>
#include "uartout.h"

#ifdef NDEBUG
	static inline void debug_flush( void ) { }
	static inline void debug_newline( void ) { }
	static inline void debug_char( char c ) { (void) c; }
	static inline void debug_nibble( byte n ) { (void) n; }
	static inline void debug_hex( byte n ) { (void) n; }
	static inline void debug_byte( byte n ) { (void) n; }
	static inline void debug_word( byte n ) { (void) n; }
	static inline void init_debug( void ) { }
	static inline void debug_led_on( void ) { }
	static inline void debug_led_off( void ) { }
	#define debug_str( str ) ((void) 0)
#else

static inline void debug_char( char c )
{
	uartout_write( c );
}

static inline void debug_flush( void ) { }

static void debug_str_( const char s [] )
{
	for ( ;; )
	{
		char c = pgm_read_byte( s++ );
		if ( !c )
			break;
		debug_char( c );
	}
}

#define debug_str( str ) debug_str_( PSTR( str ) )

static void debug_nibble( byte n )
{
	n &= 0x0F;
	if ( n > 9 )
		n += 'A' - '9' - 1;
	debug_char( n + '0' );
}

static void debug_hex( byte n )
{
	debug_nibble( n >> 4 );
	debug_nibble( n );
}

static void debug_byte( byte n )
{
	debug_hex( n );
	debug_char( ' ' );
}

static void debug_word( unsigned n )
{
	debug_hex( n >> 8 );
	debug_hex( n );
	debug_char( ' ' );
}

static void debug_newline( void )
{
	debug_char( '\n' );
	debug_flush();
}

static inline void debug_led_on( void )
{
	PORTD |= (1<<6);
}

static inline void debug_led_off( void )
{
	PORTD &= ~(1<<6);
}

static inline void init_debug( void )
{
	DDRD |= (1<<6);
	debug_led_off();
	uartout_init();
}
#endif
