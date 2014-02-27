#include "usart.h"
#include "bitwise.h"
#include <avr/io.h>

void usart_init( int baudrate ) {

	// UCSRA |= (1 << U2X);
	UCSRA &= ~(1 << U2X);
	
	UBRRH = BYTE_HIGH( U_BAUD_600 );
	UBRRL = BYTE_LOW( U_BAUD_600 );
	
	// Asynchronous operation, parity disabled, 1 stop bit, 8bit data
	UCSRC = (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);	// Frame format: TODO

}

void usart_start( void ) {
	
	UCSRB = (1 << RXCIE) | ( 1 << TXCIE ) | /*(1 << UDRIE) |*/ (1 << RXEN) | (1 << TXEN); // RX & TX enable
	
}

void usart_stop( void ) {
	
		UCSRB = 0;
	
}

void usart_send_byte_polled( uint8_t byte ) {
	
	while ( !( UCSRA & (1<<UDRE)) );
	usart_send_byte( byte );
	
}


void usart_send_data_polled( uint8_t *data, uint8_t size ) {
	
	uint8_t i = 0;
	for( i = 0; i < size; ++i ) {
		usart_send_byte_polled( data[i] );
	}
	
}

void usart_send_byte( uint8_t byte ) {
	
	UDR = byte;
	
}

uint8_t usart_recv_byte( void ) {
	
	return UDR;
	
}
