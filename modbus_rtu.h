#ifndef __MODBUS_RTU_H__
#define __MODBUS_RTU_H__

#include "types.h"

void mb_rtu_set_dev_addr( uint8_t new_addr );

void mb_rtu_init_input_regs( volatile uint16_t *addr, int reg_count );
void mb_rtu_init_holding_regs( volatile uint16_t *addr, int reg_count );
void mb_rtu_init_recvbuf( volatile uint8_t *recvbuf );
void mb_rtu_init_sendbuf( volatile uint8_t *sendbuf );
void mb_rtu_init_tmr_function( uint8_t ( *half_chars_passed )( void ) );

uint8_t mb_rtu_recvbuf_feed( uint8_t data_byte, uint8_t copy );
uint8_t mb_rtu_resp_ready( void ); // Return 1 if response is ready.
uint8_t mb_rtu_sendbuf_is_empty( void ); // Return 1 if resp buffer empty, 0 if not.
uint8_t mb_rtu_sendbuf_fetch( void ); // Return next response byte.

void mb_rtu_set_holding_reg( uint16_t reg_no, uint16_t new_value );
uint16_t mb_rtu_read_holding_reg( uint16_t reg_no );

void mb_rtu_set_input_reg( uint16_t reg_no, uint16_t new_value );
uint16_t mb_rtu_read_input_reg( uint16_t reg_no );

uint8_t mb_rtu_get_valid_frames( void );
uint8_t mb_rtu_get_invalid_frames( void );

#endif
