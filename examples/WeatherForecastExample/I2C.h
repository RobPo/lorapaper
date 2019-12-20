/*
 * I2C.h
 *
 * Created: 22-9-2017 08:48:47
 *  Author: adri
 */ 


#ifndef I2C_H_
#define I2C_H_

	/******************************************************************************************
										INCLUDE FILES
	******************************************************************************************/
	#include <stdint.h>
	
	
	/******************************************************************************************
									FUNCTION PROTOTYPES
	******************************************************************************************/
	void	I2C_init			(void);
	void	I2C_write_register	(uint8_t slaveAddress, uint8_t Register, uint8_t data);
	void	I2C_write_array		(uint8_t slaveAddress, uint8_t Register, uint8_t *data, uint8_t lenght);
	uint8_t I2C_read_register	(uint8_t slaveAddress, uint8_t Register);
	void	I2C_read_array		(uint8_t slaveAddress, uint8_t Register, uint8_t *data, uint8_t lenght);

#endif /* I2C_H_ */
