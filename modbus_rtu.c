#include "modbus_rtu.h"
#include "modbus_rtu_functions.h"
#include "crc.h"
#include "bitwise.h"

static volatile uint8_t *recv_data_buffer;
static volatile uint8_t recv_data_buffer_size;
static volatile uint8_t recv_data_buffer_start_index;
static volatile uint16_t recv_data_checksum = 0xFFFF;

static volatile uint8_t *send_data_buffer;
static volatile uint8_t send_data_buffer_size;
static volatile uint8_t send_data_buffer_start_index;

static volatile uint16_t *input_registers;
static volatile uint16_t input_registers_count;
static volatile uint16_t *holding_registers;
static volatile uint16_t holding_registers_count;

static volatile uint8_t mb_rtu_valid_frames;
static volatile uint8_t mb_rtu_invalid_frames;

static volatile uint8_t dev_addr;

static uint8_t ( *mb_rtu_halfchars_passed )( void );

void mb_rtu_set_dev_addr( uint8_t new_addr ) {

	dev_addr = new_addr;

}

void mb_rtu_init_input_regs( volatile uint16_t *addr, int reg_count ) {

	input_registers = addr;
	input_registers_count = reg_count;

}

void mb_rtu_init_holding_regs( volatile uint16_t *addr, int reg_count ) {

	holding_registers = addr;
	holding_registers_count = reg_count;

}

void mb_rtu_init_recvbuf( volatile uint8_t *recvbuf ) {

	recv_data_buffer = recvbuf;

}

void mb_rtu_init_sendbuf( volatile uint8_t *sendbuf ) {

	send_data_buffer = sendbuf;

}

void mb_rtu_init_tmr_function( uint8_t ( *half_chars_passed )( void ) ) {

	mb_rtu_halfchars_passed = half_chars_passed;

}

uint8_t mb_rtu_recvbuf_feed( uint8_t data_byte, uint8_t copy ) {

	if( copy == 1 ) {
		recv_data_buffer[recv_data_buffer_size] = data_byte;
	}
	
	if( recv_data_buffer_size < 2 )
		send_data_buffer[ recv_data_buffer_size ] = data_byte;

	crc_feed( (uint16_t *)&recv_data_checksum, data_byte );

	++recv_data_buffer_size;
	
	return recv_data_buffer_size;

}

static void set_multiple_regs( void ) {

	uint16_t reg_offset = BYTES2WORD( recv_data_buffer[2], recv_data_buffer[3] );
	uint16_t reg_count = BYTES2WORD( recv_data_buffer[4], recv_data_buffer[5] );
	
	uint16_t tmp_reg = 0;
	
	uint8_t i = 0;
	uint8_t byte_index = 7;
	for( i = 0; i < reg_count; ++i ) {
		holding_registers[reg_offset + i] = BYTES2WORD( recv_data_buffer[byte_index], recv_data_buffer[byte_index+1] );
		byte_index += 2;
	}

}

uint8_t mb_rtu_resp_ready( void ) {

	if( (*mb_rtu_halfchars_passed)() < 7 || recv_data_buffer_size == 0 ) return 0; // Not ready.
	
	/*if( recv_data_checksum != BYTES2WORD( recv_data_buffer[ recv_data_buffer_size - 2 ],
										  recv_data_buffer[ recv_data_buffer_size - 1 ] ) ) { // Invalid checksum
		++mb_rtu_invalid_frames;
		recv_data_checksum = 0xFFFF;
		recv_data_buffer_start_index = 0;
		recv_data_buffer_size = 0;
		return 0;
	}*/

	++mb_rtu_valid_frames;

	switch( recv_data_buffer[1] ) { // [1]: Modbus RTU function code
		case 1: 
			return 0;
			break;
		case 2:
			return 0;
			break;
		case 3:
			send_data_buffer_size = srlze_resp_read_holding_regs( send_data_buffer,
					BYTES2WORD( recv_data_buffer[2], recv_data_buffer[3] ),
					BYTES2WORD( recv_data_buffer[4], recv_data_buffer[5] ),
					holding_registers, holding_registers_count );
			break;
		case 4:
			send_data_buffer_size = srlze_resp_read_input_regs( send_data_buffer,
					BYTES2WORD( recv_data_buffer[2], recv_data_buffer[3] ),
					BYTES2WORD( recv_data_buffer[4], recv_data_buffer[5] ),
					input_registers, input_registers_count );
			break;
		case 5:
			return 0;
			break;
		case 6:
			mb_rtu_set_holding_reg( BYTES2WORD( recv_data_buffer[2], recv_data_buffer[3] ),
									BYTES2WORD( recv_data_buffer[4], recv_data_buffer[5] ) );
			send_data_buffer_size = srlze_resp_write_single_reg( send_data_buffer, recv_data_buffer, recv_data_buffer_size, 
					holding_registers_count );
			break;
		case 15:
			return 0;
			break;
		case 16:
			set_multiple_regs();
			send_data_buffer_size = srlze_resp_write_multiple_regs( send_data_buffer, recv_data_buffer, recv_data_buffer_size,
					holding_registers_count );
			break;
		default:
			return 0;
			break;
	}
	
	recv_data_checksum = 0xFFFF;
	recv_data_buffer_start_index = 0;
	recv_data_buffer_size = 0;
	
	return 1;
	
}

uint8_t mb_rtu_sendbuf_is_empty( void ) {

	if( send_data_buffer_size == send_data_buffer_start_index ) {

		send_data_buffer_size = 0;
		send_data_buffer_start_index = 0;
		return 1;

	} else {

		return 0;

	}

}

uint8_t mb_rtu_sendbuf_fetch( void ) {

	return send_data_buffer[ send_data_buffer_start_index++ ];
	
}

void mb_rtu_set_holding_reg( uint16_t reg_no, uint16_t new_value ) {

	if( reg_no >= holding_registers_count ) {
		return;
	}

	holding_registers[ reg_no ] = new_value;

}

uint16_t mb_rtu_read_holding_reg( uint16_t reg_no ) {

	return holding_registers[ reg_no ];

}

void mb_rtu_set_input_reg( uint16_t reg_no, uint16_t new_value ) {

	if( reg_no >= input_registers_count ) {
		return;
	}

	input_registers[ reg_no ] = new_value;

}

uint16_t mb_rtu_read_input_reg( uint16_t reg_no ) {

	return input_registers[ reg_no ];

}

uint8_t mb_rtu_get_valid_frames( void ) {

	return mb_rtu_valid_frames;

}
	
uint8_t mb_rtu_get_invalid_frames( void ) {

	return mb_rtu_invalid_frames;
	
}
