#ifndef __BITWISE_H__
#define __BITWISE_H__

#define BYTE_HIGH( u16 ) ( (uint8_t)( u16 >> 8 ) )
#define BYTE_LOW( u16 ) ( (uint8_t)( u16 & 0x00ff ) )

#define BYTES2WORD( byte_msb, byte_lsb ) ( ( (uint16_t)byte_msb << 8 ) | byte_lsb )

#endif
