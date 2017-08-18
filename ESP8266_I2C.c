/****************************************************************
* ESP8266 I2C LIBRARY
*
* WRAPPER AROUND ESP8266 I2C MASTER DRIVER
* (ESP8266 DOES NOT HAVE HARDWARE I2C. ESPRESSIF MASTER I2C DRIVER
* IS SOFTWARE BITBANGING OF I2C PROTOCOL)
*
* THE I2C ADDRESS IS THE UPPER MOST SIGNIFICANT 7 BITS (WITHOUT THE R/W)
* BIT ADJUSTED AS A 8 BIT VALUE
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

#include "ESP8266_I2C.h"

//LOCAL LIBRARY VARIABLES////////////////////////////////
//DEBUG RELATED
static uint8_t _esp8266_i2c_debug;
static ESP8266_I2C_STATE _esp8266_i2c_state;

//I2C RELATED
static uint8_t _esp8266_i2c_slave_address;
//END LOCAL LIBRARY VARIABLES/////////////////////////////

void ICACHE_FLASH_ATTR ESP8266_I2C_SetDebug(uint8_t debug_on)
{
    //SET DEBUG PRINTF ON(1) OR OFF(0)

    _esp8266_i2c_debug = debug_on;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_Init(uint8_t slave_address)
{
    //SET THE I2C DEVICE ADDRESS
    //THIS IS THE UPPER 7 BITS OF THE 8 BIT I2C ADDRESS
    //FOR READ  (BIT 0) = 1
    //FOR WRITE (BIT 0) = 0

    if(&slave_address == NULL || slave_address == 0)
    {
        //INVALID I2C ADDRESS
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Invalid i2c slave address. TERMINATING\n");
        }
        _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
    }
    else
    {
        //VALID I2C ADDRESS
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : i2c slave address set to 0x%x\n", slave_address);
        }
        _esp8266_i2c_slave_address = slave_address;
    }

    //SET DEBUG = ON BY DEFAULT
    _esp8266_i2c_debug = 1;

    //INITIALIZE ESP8266 SOFT I2C MASTER MODE
    //ALSO INIT THE I2C GPIO PINS
    i2c_master_gpio_init();
    i2c_master_init();

    _esp8266_i2c_state = ESP8266_I2C_STATE_OK;
}

uint8_t ICACHE_FLASH_ATTR ESP8266_I2C_GetSlaveAddress(void)
{
    //RETURN THE SET I2C SLAVE ADDRESS
    //RETURNS THE UPPER 7 BITS OF THE ADDRESS (WITHOUT R/W BIT 0)

    return _esp8266_i2c_slave_address;
}

ESP8266_I2C_STATE ICACHE_FLASH_ATTR ESP8266_I2C_GetStatus(void)
{
    //RETURN THE STATE VALUE OF THE ESP8266 I2C LIBRARY

    return _esp8266_i2c_state;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_WriteByte(uint8_t write_reg, uint8_t byte)
{
    //WRITE 1 BYTE TO THE I2C SLAVE SPECIFIED WRITE REGISTER
    //I2C SEQUENCE:
    //  START BIT (MASTER)
    //  SLAVE WRITE ADDRESS (MASTER)
    //  ACK (SLAVE)
    //  REGISTER ADDRESS TO WRITE TO (MASTER)
    //  ACK (SLAVE)
    //  DATA BYTE (MASTER)
    //  ACK (SLAVE)
    //  STOP (MASTER)

    i2c_master_start();
    i2c_master_writeByte((_esp8266_i2c_slave_address << 1));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_writeByte(write_reg);
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write register address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_writeByte(byte);
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write data\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_stop();

    _esp8266_i2c_state = ESP8266_I2C_STATE_OK;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_WriteByteMultiple(uint8_t write_reg, uint8_t* buf, uint8_t len)
{
    //WRITE MULTIPLE BYTES TO THE I2C SLAVE SPECIFIED WRITE REGISTER

    i2c_master_start();
    i2c_master_writeByte((_esp8266_i2c_slave_address << 1));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_writeByte(write_reg);
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write register address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }

    uint8_t counter;
    for(counter = 0; counter < len; counter++)
    {
        i2c_master_writeByte(buf[counter]);
        if(!i2c_master_checkAck())
        {
            //I2C DEVICE RESPONDED NACK
            if(_esp8266_i2c_debug)
            {
                os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write data index %d\n", counter);
                _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
            }
            //TERMINATE REST OF TRANSFER
            i2c_master_stop();
        }
    }

    //TRANSFER COMPLETED SUCCESSFULLY
    i2c_master_stop();

    _esp8266_i2c_state = ESP8266_I2C_STATE_OK;
}

uint8_t ICACHE_FLASH_ATTR ESP8266_I2C_ReadByte(uint8_t read_reg)
{
    //READ 1 BYTE FROM THE SPECIFIED REGISTER ADDRESS OF THE I2C SLAVE

    i2c_master_start();
    i2c_master_writeByte((_esp8266_i2c_slave_address << 1));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_writeByte(read_reg);
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to read register address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_start();
    i2c_master_writeByte((_esp8266_i2c_slave_address << 1)+ 1);
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to read data\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    uint8_t data = i2c_master_readByte();
    i2c_master_send_nack();
    i2c_master_stop();

    _esp8266_i2c_state = ESP8266_I2C_STATE_OK;

    return data;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_ReadByteMultiple(uint8_t read_reg, uint8_t* buf, uint8_t len)
{
    //READ THE SPECIFIED NUMBER OF BYTES FROM I2C SLAVE SPECIFIED READ REGISTER
    //I2C SEQUENCE:
    //  START BIT (MASTER)
    //  SLAVE WRITE ADDRESS (MASTER)
    //  ACK (SLAVE)
    //  REGISTER ADDRESS TO READ FROM (MASTER)
    //  ACK (SLAVE)
    //  REPEAT START (MASTER)
    //  SLAVE READ ADDRESS (MASTER)
    //  ACK (SLAVE)
    //  DATA BYTE (SLAVE)
    //  NACK (MASTER)
    //  STOP (MASTER)

    i2c_master_start();
    i2c_master_writeByte((_esp8266_i2c_slave_address << 1));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_writeByte(read_reg);
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to read register address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    i2c_master_start();
    i2c_master_writeByte((_esp8266_i2c_slave_address << 1)+ 1);
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to read data\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }

    uint8_t data, counter;
    for(counter = 0; counter < (len - 1); counter++)
    {
        data = i2c_master_readByte();
        i2c_master_send_ack();

        buf[counter] = data;
    }

    //LAST BYTE TO BE READ
    data = i2c_master_readByte();
    i2c_master_send_nack();
    buf[counter] = data;

    i2c_master_stop();

    _esp8266_i2c_state = ESP8266_I2C_STATE_OK;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_SendStart(void)
{
	//SEND I2C START ON I2C BUS

	i2c_master_start();
}

void ICACHE_FLASH_ATTR ESP8266_I2C_SendStop(void)
{
	 //WRITE I2C STOP ON I2C BUS

	 i2c_master_stop();
}

uint8_t ICACHE_FLASH_ATTR ESP8266_I2C_SendByte(uint8_t val)
{
	//PUT SPECIFIED BYTE ON I2C BUS AND RETURN ACK/NACK
	//TRUE (1) : ACK RECEIVED
	//FALSE (0) : NACK RECEIVED

	i2c_master_writeByte(val);
	return i2c_master_checkAck();
 }
