#include "modbus_rtu_functions.h"
#include "crc.h"
#include "bitwise.h"

static uint8_t prepare_exception( volatile uint8_t *destbuf, uint8_t exc_code ) {

		uint8_t size = 2;
		uint16_t crc = 0xFFFF;
		
		crc_feed( &crc, destbuf[0] );
		destbuf[1] |= ( 1 << 7 ); // Set most significant bit in modbus function byte.
		crc_feed( &crc, destbuf[1] );
		destbuf[size++] = exc_code;
		crc_feed( &crc, destbuf[2] );
		destbuf[size++] = BYTE_LOW( crc );
		destbuf[size++] = BYTE_HIGH( crc );
		
		return size;

}

static uint8_t srlze_read_regs( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t reg_count, volatile uint16_t *regs, uint16_t max_reg_no ) {

	uint8_t size = 2;
	uint16_t crc = 0xffff;
	
	if( ( reg_no + reg_count ) > max_reg_no ) { // Requested reg unavailable, send exception.
		return prepare_exception( destbuf, MB_EXCEPTION_ILL_DATA_ADDR );
	}
	
	destbuf[size++] = BYTE_LOW( reg_count ) * 2;
	
	crc_feed( &crc, destbuf[0] );
	crc_feed( &crc, destbuf[1] );
	crc_feed( &crc, destbuf[2] );
	
	int i = reg_no;
	for( i = reg_no; i < reg_count; ++i ) {
		destbuf[size] = BYTE_HIGH( regs[i] );
		crc_feed( &crc, destbuf[size++] );
		destbuf[size] = BYTE_LOW( regs[i] );
		crc_feed( &crc, destbuf[size++] );
	}
	
	destbuf[size++] = BYTE_LOW( crc );
	destbuf[size++] = BYTE_HIGH( crc );
	
	return size;

}

uint8_t srlze_resp_read_input_regs( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t reg_count,
									volatile uint16_t *input_regs, uint16_t max_reg_no ) {

	return srlze_read_regs( destbuf, reg_no, reg_count, input_regs, max_reg_no );

}

uint8_t srlze_resp_read_holding_regs( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t reg_count, 
									  volatile uint16_t *holding_regs, uint16_t max_reg_no ) {

	return srlze_read_regs( destbuf, reg_no, reg_count, holding_regs, max_reg_no );

}

uint8_t srlze_resp_write_single_reg( volatile uint8_t *destbuf, volatile uint8_t *srcbuf, uint8_t size, uint16_t max_reg_no ) {

	if( ( BYTES2WORD( srcbuf[2], srcbuf[3] ) ) > max_reg_no ) { // Requested reg unavailable, send exception.
		return prepare_exception( destbuf, MB_EXCEPTION_ILL_DATA_ADDR );
	}

	int i = size;
	while( i-- ) *(destbuf++) = *(srcbuf++);
	return size;

}

uint8_t srlze_resp_write_multiple_regs( volatile uint8_t *destbuf, volatile uint8_t *srcbuf, uint8_t size, uint16_t max_reg_no ) {

	if( ( BYTES2WORD( srcbuf[2], srcbuf[3] ) + BYTES2WORD( srcbuf[4], srcbuf[5] ) ) > max_reg_no ) { // Requested reg unavailable, send exception.
		return prepare_exception( destbuf, MB_EXCEPTION_ILL_DATA_ADDR );
	}

	uint8_t i = 0;
	uint16_t crc = 0xFFFF;
	for( i = 0; i < 6; ++i ) { // First six bytes the same in request and response
		destbuf[i] = srcbuf[i];
		crc_feed( &crc, destbuf[i] );
	}

	destbuf[i++] = BYTE_LOW( crc );
	destbuf[i++] = BYTE_HIGH( crc );
	
	return i; // i - size of destbuf

}

uint8_t srlze_req_write_multiple_regs( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t reg_count, volatile uint16_t *holding_regs ) {

}

uint8_t srlze_req_write_single_reg( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t new_reg_value ) {

}
