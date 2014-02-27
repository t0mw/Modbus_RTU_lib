#ifndef __USART_H__
#define __USART_H__

#include <stdint.h>

#define U_BAUD_9600 9600
#define U_BAUD_600 103

enum usart_states { U_IDLE, U_DISABLED, U_SEND, U_RECV };

void usart_init( int baudrate );
void usart_start( void );

void usart_send_byte_polled( uint8_t byte );
void usart_send_data_polled( uint8_t *data, uint8_t size );

void usart_send_byte( uint8_t byte );
uint8_t usart_recv_byte( void );

#endif
