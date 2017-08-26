/****************************************************************
* ESP8266 I2C LIBRARY
*
* WRAPPER AROUND ESP8266 I2C MASTER DRIVER
* (ESP8266 DOES NOT HAVE HARDWARE I2C. ESPRESSIF MASTER I2C DRIVER
* IS SOFTWARE BITBANGING OF I2C PROTOCOL)
*
* THE I2C ADDRESS IS THE UPPER MOST SIGNIFICANT 7 BITS SHIFTED RIGHT
* 1 PLACE
*
* MAY 25 2017
*
* ESP8266 I2C MASTER USES THE FOLLOWING PINS FOR I2C
*   I2C SCL : GPIO14
*   I2C SDA : GPIO2
*
* ANKIT BHATNAGAR
* ANKIT.BHATNAGARINDIA@GMAIL.COM
*
* REFERENCES
* ------------
*   (1) TEXAS INSTRUMENTS APP NOTE - UNDERSTANDING I2C
*       www.ti.com/lit/an/slva704/slva704.pdf
*
*   (2) https://github.com/BillyWoods/ESP8266-I2C-example/blob/master/main.c
****************************************************************/

#ifndef _ESP8266_I2C_
#define _ESP8266_I2C_

#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "driver/i2c_master.h"

#define ESP8266_I2C_SLAVE_ADDRESS_WRITE(x)	((x << 1))
#define ESP8266_I2C_SLAVE_ADDRESS_READ(x)		((x << 1) + 1)

//CUSTOM VARIABLE STRUCTURES/////////////////////////////
typedef enum
{
	ESP8266_I2C_STATE_OK,
	ESP8266_I2C_STATE_ERROR
} ESP8266_I2C_STATE;
//END CUSTOM VARIABLE STRUCTURES/////////////////////////

//FUNCTION PROTOTYPES/////////////////////////////////////
//CONFIGURATION FUNCTIONS
void ICACHE_FLASH_ATTR ESP8266_I2C_SetDebug(uint8_t debug_on);
void ICACHE_FLASH_ATTR ESP8266_I2C_Init(void);

//GET PARAMETER FUNCTIONS
ESP8266_I2C_STATE ICACHE_FLASH_ATTR ESP8266_I2C_GetStatus(void);

//CONTROL FUNCTIONS
void ICACHE_FLASH_ATTR ESP8266_I2C_WriteByte(uint8_t slave_address, uint32_t write_reg_add, uint8_t write_reg_add_len, uint8_t byte);
void ICACHE_FLASH_ATTR ESP8266_I2C_WriteByteMultiple(uint8_t slave_address, uint32_t write_reg_add, uint8_t write_reg_add_len, uint8_t* buf, uint8_t len);
uint8_t ICACHE_FLASH_ATTR ESP8266_I2C_ReadByte(uint8_t slave_address, uint32_t read_reg_add, uint8_t read_reg_add_len);
void ICACHE_FLASH_ATTR ESP8266_I2C_ReadByteMultiple(uint8_t slave_address, uint32_t read_reg_add, uint8_t read_reg_add_len, uint8_t* buf, uint8_t len);

//I2C ATOMIC CONTROL FUNCTIONS
void ICACHE_FLASH_ATTR ESP8266_I2C_SendStart(void);
void ICACHE_FLASH_ATTR ESP8266_I2C_SendStop(void);
uint8_t ICACHE_FLASH_ATTR ESP8266_I2C_SendByte(uint8_t val);
//END FUNCTION PROTOTYPES/////////////////////////////////
#endif
