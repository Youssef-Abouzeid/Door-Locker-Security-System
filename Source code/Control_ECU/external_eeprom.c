#include "external_eeprom.h"
#include "twi.h"
#include"util/delay.h"

uint8 EEPROM_writeByte(uint16 u16addr, uint8 *u8data,uint16 len)
{
	uint16 i;
	/*Send the Start Bit*/
	TWI_start();
	if (TWI_getStatus() != TWI_START)
		return ERROR;

	/*Send the device address, we need to get A8 A9 A10 address bits from the
	 * memory location address and R/W=0 (write)*/
	TWI_writeByte((uint8)(0xA0 | ((u16addr & 0x0700)>>7)));
	if (TWI_getStatus() != TWI_MT_SLA_W_ACK)
		return ERROR;

	/* Send the required memory location address*/
	TWI_writeByte((uint8)(u16addr));
	if (TWI_getStatus() != TWI_MT_DATA_ACK)
		return ERROR;

	for(i=0 ;i<len; i++){
		/*write bytes to eeprom*/
		TWI_writeByte( u8data[i] );
		if (TWI_getStatus() != TWI_MT_DATA_ACK)
		{
			return ERROR;
		}
		_delay_ms(10);
	}
	/*Send the Stop Bit*/
	TWI_stop();

	return SUCCESS;
}

uint8 EEPROM_readByte(uint16 u16addr, uint8 *u8data,uint16 len)
{
	uint16 i;
	/*Send the Start Bit*/
	TWI_start();
	if (TWI_getStatus() != TWI_START)
		return ERROR;

	/* Send the device address, we need to get A8 A9 A10 address bits from the
	 * memory location address and R/W=0 (write)*/
	TWI_writeByte((uint8)((0xA0) | ((u16addr & 0x0700)>>7)));
	if (TWI_getStatus() != TWI_MT_SLA_W_ACK)
		return ERROR;

	/* Send the required memory location address*/
	TWI_writeByte((uint8)(u16addr));
	if (TWI_getStatus() != TWI_MT_DATA_ACK)
		return ERROR;

	/*Send the Repeated Start Bit*/
	TWI_start();
	if (TWI_getStatus() != TWI_REP_START)
		return ERROR;

	/* Send the device address, we need to get A8 A9 A10 address bits from the
	 * memory location address and R/W=1 (Read)*/
	TWI_writeByte((uint8)((0xA0) | ((u16addr & 0x0700)>>7) | 1));
	if (TWI_getStatus() != TWI_MT_SLA_R_ACK)
		return ERROR;

	for(i=0 ;i<len; i++){
		/* Read Bytes from Memory with ACK*/
		u8data[i] = TWI_readByteWithACK();
		if (TWI_getStatus() != TWI_MR_DATA_ACK)
		{
			return ERROR;
		}
		_delay_ms(10);
	}

	/*Send the Stop Bit*/
	TWI_stop();

	return SUCCESS;
}


