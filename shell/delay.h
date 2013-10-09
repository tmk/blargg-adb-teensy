#include <util/delay.h>

#define to_us( cyc ) ((cyc) * 1e6 / F_CPU)

#define delay_usec( us ) \
	__builtin_avr_delay_cycles( (unsigned long) (F_CPU / 1e6 * (us) + 0.5) )

#define delay_loop_usec( us, cyc ) \
	__builtin_avr_delay_cycles( (unsigned long) (F_CPU / 1e6 * (us) + 0.5) - (cyc) )

#define delay_loop_freq( hz, cyc ) \
	__builtin_avr_delay_cycles( (unsigned long) ((double) F_CPU / (hz) + 0.5) - (cyc) )
	
static void delay_msec( unsigned n )
{
	while ( n-- )
		delay_loop_usec( 1000, 6 );
}

static void delay_sec( byte secs )
{
	while ( secs-- )
		delay_msec( 1000 );
}
