#ifndef __MODBUS_RTU_FUCTIONS_H__
#define __MODBUS_RTU_FUCTIONS_H__

#include "types.h"

#define MB_EXCEPTION_ILL_DATA_ADDR 0x02

uint8_t srlze_resp_read_input_regs( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t reg_count,
									volatile uint16_t *input_regs, uint16_t max_reg_no );
uint8_t srlze_resp_read_holding_regs( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t reg_count,
									  volatile uint16_t *holding_regs, uint16_t max_reg_no );

uint8_t srlze_resp_write_single_reg( volatile uint8_t *destbuf, volatile uint8_t *srcbuf, uint8_t size, uint16_t reg_count );
uint8_t srlze_resp_write_multiple_regs( volatile uint8_t *destbuf, volatile uint8_t *srcbuf, uint8_t size, uint16_t reg_count );

uint8_t srlze_req_write_multiple_regs( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t reg_count, volatile uint16_t *holding_regs );
uint8_t srlze_req_write_single_reg( volatile uint8_t *destbuf, uint16_t reg_no, uint16_t new_reg_value );

#endif
