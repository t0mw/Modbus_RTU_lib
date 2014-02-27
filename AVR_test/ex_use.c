#include "modbus_rtu.h"
#include "usart.h"
#include "timer.h"
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>

volatile uint8_t usart_recvbuf[64];
volatile uint8_t recvbuf_index_start = 0;
volatile uint8_t recvbuf_index_stop = 0;
volatile uint8_t usart_sendbuf[64];

volatile uint16_t modbus_holding_regs[16];
volatile uint16_t modbus_input_regs[16];
volatile uint8_t half_chars_passed = 0;

uint8_t usart_mode = U_RECV;

#define DIODE_PORT PORTB
#define DIODE_PIN 0

void tmr_refresh( void ) {

	half_chars_passed = 0;
	
}

ISR( USART_RXC_vect ) {

	if( usart_mode == U_RECV ) {
		
		tmr_refresh();
		
		if( recvbuf_index_stop == 0 ) {
			timer_reset();
			timer_start();
		}

		usart_recvbuf[recvbuf_index_stop++] = usart_recv_byte();
		
	}
	
}

ISR( USART_UDRE_vect ) {

}

ISR( USART_TXC_vect ) {

	if( usart_mode == U_SEND ) {
		
		if( mb_rtu_sendbuf_is_empty() == 0 ) {
			usart_send_byte( mb_rtu_sendbuf_fetch() );
		} else {
			usart_mode = U_RECV;
		}
		
	}
	
}

ISR( TIMER1_COMPA_vect, ISR_NOBLOCK ) {

	++half_chars_passed;
	timer_reset();
	
}

uint8_t half_chars_passed_count( void ) {

	return half_chars_passed;
	
}

/*
	
	modpoll.exe -m rtu -r 1 -c 16 -l 2000 -o 2 -b 600 -t 4:hex -p none COM7
	
	TODO:
		- timers
		- comments
		- move files
		- master
	
*/
int main( void ) {

	wdt_disable();
	
	cli();
	
		DDRB |= ( 1 << DIODE_PIN ); // Enable PC5 as output.
		PORTB |= ( 1 << DIODE_PIN ); // Turn off the diode.
	
		uint16_t i = 0;
		for( i = 0; i < 16; ++i ) {
			modbus_input_regs[i] = i;
			modbus_holding_regs[i] = i+0xa000;
		}
	
		mb_rtu_init_input_regs( modbus_input_regs, 16 );
		mb_rtu_init_holding_regs( modbus_holding_regs, 16 );
		mb_rtu_init_recvbuf( usart_recvbuf );
		mb_rtu_init_sendbuf( usart_sendbuf );
		mb_rtu_init_tmr_function( half_chars_passed_count );

		usart_init(0);
		usart_start();
		
		usart_send_byte_polled( 0xEF );
		
		timer_compa_init();
		
		//usart_init( BAUD_9600 );
		//usart_set_mode( USART_MODE_RX );
		//usart_int_enable( USART_INT_RX | USART_INT_TX );
		
	sei();

	while(1) {

		if( recvbuf_index_stop != 0 && recvbuf_index_start != recvbuf_index_stop ) {
			mb_rtu_recvbuf_feed( usart_recvbuf[recvbuf_index_start++], 0 );
		}

		if( mb_rtu_resp_ready() && usart_mode != U_SEND ) {
			timer_stop();
			recvbuf_index_start = 0;
			recvbuf_index_stop = 0;
			usart_mode = U_SEND;
			usart_send_byte( mb_rtu_sendbuf_fetch() );
		}

	}

	return 0;

}
