// Simple adapter to connect Apple ADB keyboard to USB computer.

#include "shell/shell.h"

#include "adb.h"
#include "keymap.h"
#include "usb_keyboard.h"

extern byte adb_debug_log [0x40];
extern byte adb_debug_pos;

enum { max_keys = 6 };

static void handle_key( byte raw )
{
	byte code = usb_from_adb_code( raw & 0x7F );
	if ( !code )
		return;
	
	if ( KC_LCTRL <= code && code <= KC_RGUI )
	{
		// Modifier; route to mask rather than keys list
		byte mask = 1 << (code - KC_LCTRL);
		keyboard_modifier_keys &= ~mask;
		if ( !(raw & 0x80) )
			keyboard_modifier_keys |= mask;
	}
	else
	{
		// Find code in list
		byte i = 0;
		do
		{
			if ( keyboard_keys [i] == code )
				break;
			i++;
		}
		while ( i < max_keys );
		
		if ( raw & 0x80 )
		{
			// Released
			if ( i >= max_keys )
			{
				debug_str( "released key not in list\n" );
			}
			else
			{
				// Remove from list
				for ( ; i < max_keys - 1; i++ )
					keyboard_keys [i] = keyboard_keys [i + 1];
				
				keyboard_keys [i] = 0;
			}
		}
		else
		{
			// Pressed
			if ( i < max_keys )
			{
				debug_str( "pressed key already in list\n" );
			}
			else
			{
				// Add to list
				i = 0;
				do
				{
					if ( keyboard_keys [i] == 0 )
					{
						keyboard_keys [i] = code;
						break;
					}
					i++;
				}
				while ( i < max_keys );
			
				if ( i >= max_keys )
					debug_str( "too many keys pressed\n" );
			}
		}
	}
}

static inline void sleep( void )
{
	#ifdef SMCR
		SMCR &= ~(1<<SM2 | 1<<SM1 | 1<<SM0);
	#endif
	sleep_enable();
	sleep_cpu();
	sleep_disable();
}

static inline void init( void )
{
	init_shell();
	
	usb_init();
	while ( !usb_configured() )
		{ }
	
	delay_msec( 1000 ); // give OS time to load keyboard driver
	
	adb_host_init();
	
	// Enable separate key codes for left/right shift/control/option keys
	// on Apple Extended Keyboard.
    adb_host_listen( 0x2B, 0x02, 0x03 ); // left/right distinction
}

int main( void )
{
	init();
	
	byte pos = 0;
	byte leds = -1;
	for ( ;; )
	{
		byte new_leds = keyboard_leds;
		if ( leds != new_leds )
		{
			leds = new_leds;
			adb_host_kbd_led( ~leds & 0x07 );
		}
		
		// Get USB interrupt activity out of the way before doing time-sensitive ADB stuff
		delay_msec( 7 );
		sleep();
		
		pos = adb_debug_pos;
		uint16_t keys = adb_host_kbd_recv();
		if ( keys == adb_host_nothing )
			continue;
		
		if ( keys == adb_host_error )
		{
			debug_str( "error\n" );
			while ( pos != adb_debug_pos )
				debug_byte( adb_debug_log [pos++ & 0x3F] );
			debug_newline();
			
			continue;
		}
		
		debug_word( keys );
		debug_flush();
		
		// Split the two key events
		handle_key( keys >> 8 );
		byte key = keys & 0xFF;
		if ( (key & 0x7F) != 0x7F )
			handle_key( key );
		usb_keyboard_send();
	}
}
