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

#include "ESP8266_I2C.h"

//LOCAL LIBRARY VARIABLES////////////////////////////////
//DEBUG RELATED
static uint8_t _esp8266_i2c_debug;
static ESP8266_I2C_STATE _esp8266_i2c_state;

//I2C RELATED
static uint8_t _esp8266_i2c_initialized = 0;
//END LOCAL LIBRARY VARIABLES/////////////////////////////

void ICACHE_FLASH_ATTR ESP8266_I2C_SetDebug(uint8_t debug_on)
{
    //SET DEBUG PRINTF ON(1) OR OFF(0)

    _esp8266_i2c_debug = debug_on;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_Init(void)
{
    //SET THE I2C DEVICE ADDRESS
    //THIS IS THE UPPER 7 BITS OF THE 8 BIT I2C ADDRESS
    //FOR READ  (BIT 0) = 1
    //FOR WRITE (BIT 0) = 0

    //INITIALIZE IF NOT ALREADY INITIALIZED
    if(_esp8266_i2c_initialized == 1)
    {
        //ALREADY INITILIAZED
        return;
    }

    //SET DEBUG = ON BY DEFAULT
    _esp8266_i2c_debug = 1;

    //INITIALIZE ESP8266 SOFT I2C MASTER MODE
    //ALSO INIT THE I2C GPIO PINS
    i2c_master_gpio_init();
    i2c_master_init();

    _esp8266_i2c_initialized = 1;

    _esp8266_i2c_state = ESP8266_I2C_STATE_OK;
}

ESP8266_I2C_STATE ICACHE_FLASH_ATTR ESP8266_I2C_GetStatus(void)
{
    //RETURN THE STATE VALUE OF THE ESP8266 I2C LIBRARY

    return _esp8266_i2c_state;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_WriteByte(uint8_t slave_address, uint32_t write_reg_add, uint8_t write_reg_add_len, uint8_t byte)
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

    if(_esp8266_i2c_debug)
    {
        os_printf("ESP8266 : I2C : Write multiple\n");
    }

    //CHECK FOR VALIDITY OF SLAVE ADDRESS
    if(&slave_address == NULL || slave_address == 0)
    {
        //INVALID I2C ADDRESS
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Invalid i2c slave address. TERMINATING\n");
        }
        _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        return;
    }

    i2c_master_start();
    i2c_master_writeByte(ESP8266_I2C_SLAVE_ADDRESS_WRITE(slave_address));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to I2C write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }

    //SEND WRITE REGISTER ADDRESS (MULTIPLE BYTES IF REQUIRED)
    while(write_reg_add_len > 0)
    {
        i2c_master_writeByte((uint8_t)((write_reg_add & (0xFF << (8 * (write_reg_add_len - 1)))) >> (8 * (write_reg_add_len - 1))));
        if(!i2c_master_checkAck())
        {
            //I2C DEVICE RESPONDED NACK
            if(_esp8266_i2c_debug)
            {
                os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write register address\n");
                _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
            }
        }
        write_reg_add_len--;
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

void ICACHE_FLASH_ATTR ESP8266_I2C_WriteByteMultiple(uint8_t slave_address, uint32_t write_reg_add, uint8_t write_reg_add_len, uint8_t* buf, uint8_t len)
{
    //WRITE MULTIPLE BYTES TO THE I2C SLAVE SPECIFIED WRITE REGISTER

    if(_esp8266_i2c_debug)
    {
        os_printf("ESP8266 : I2C : Write multiple\n");
    }

    //CHECK FOR VALIDITY OF SLAVE ADDRESS
    if(&slave_address == NULL || slave_address == 0)
    {
        //INVALID I2C ADDRESS
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Invalid i2c slave address. TERMINATING\n");
        }
        _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        return;
    }

    i2c_master_start();
    i2c_master_writeByte(ESP8266_I2C_SLAVE_ADDRESS_WRITE(slave_address));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to i2c write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }

    //SEND WRITE REGISTER ADDRESS (MULTIPLE BYTES IF REQUIRED)
    while(write_reg_add_len > 0)
    {
        i2c_master_writeByte((uint8_t)((write_reg_add & (0xFF << (8 * (write_reg_add_len - 1)))) >> (8 * (write_reg_add_len - 1))));
        if(!i2c_master_checkAck())
        {
            //I2C DEVICE RESPONDED NACK
            if(_esp8266_i2c_debug)
            {
                os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to write register address\n");
                _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
            }
        }
        write_reg_add_len--;
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

uint8_t ICACHE_FLASH_ATTR ESP8266_I2C_ReadByte(uint8_t slave_address, uint32_t read_reg_add, uint8_t read_reg_add_len)
{
    //READ 1 BYTE FROM THE SPECIFIED REGISTER ADDRESS OF THE I2C SLAVE

    if(_esp8266_i2c_debug)
    {
        os_printf("ESP8266 : I2C : Read single\n");
    }

    //CHECK FOR VALIDITY OF SLAVE ADDRESS
    if(&slave_address == NULL || slave_address == 0)
    {
        //INVALID I2C ADDRESS
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Invalid i2c slave address. TERMINATING\n");
        }
        _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        return;
    }

    i2c_master_start();
    i2c_master_writeByte(ESP8266_I2C_SLAVE_ADDRESS_WRITE(slave_address));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to I2C write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }

    //SEND READ REGISTER ADDRESS (MULTIPLE BYTES IF REQUIRED)
    while(read_reg_add_len > 0)
    {
        i2c_master_writeByte((uint8_t)((read_reg_add & (0xFF << (8 * (read_reg_add_len - 1)))) >> (8 * (read_reg_add_len - 1))));
        if(!i2c_master_checkAck())
        {
            //I2C DEVICE RESPONDED NACK
            if(_esp8266_i2c_debug)
            {
                os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to read register address\n");
                _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
            }
        }
        read_reg_add_len--;
    }

    i2c_master_start();
    i2c_master_writeByte(ESP8266_I2C_SLAVE_ADDRESS_READ(slave_address));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to I2C read address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }
    uint8_t data = i2c_master_readByte();
    i2c_master_send_nack();
    i2c_master_stop();

    _esp8266_i2c_state = ESP8266_I2C_STATE_OK;

    return data;
}

void ICACHE_FLASH_ATTR ESP8266_I2C_ReadByteMultiple(uint8_t slave_address, uint32_t read_reg_add, uint8_t read_reg_add_len, uint8_t* buf, uint8_t len)
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

    if(_esp8266_i2c_debug)
    {
        os_printf("ESP8266 : I2C : Read multiple\n");
    }

    if(&slave_address == NULL || slave_address == 0)
    {
        //INVALID I2C ADDRESS
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Invalid i2c slave address. TERMINATING\n");
        }
        _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        return;
    }

    i2c_master_start();
    i2c_master_writeByte(ESP8266_I2C_SLAVE_ADDRESS_WRITE(slave_address));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to i2c write address\n");
            _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
        }
    }

    //SEND READ REGISTER ADDRESS (MULTIPLE BYTES IF REQUIRED)
    while(read_reg_add_len > 0)
    {
        i2c_master_writeByte((uint8_t)((read_reg_add & (0xFF << (8 * (read_reg_add_len - 1)))) >> (8 * (read_reg_add_len - 1))));
        if(!i2c_master_checkAck())
        {
            //I2C DEVICE RESPONDED NACK
            if(_esp8266_i2c_debug)
            {
                os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to read register address\n");
                _esp8266_i2c_state = ESP8266_I2C_STATE_ERROR;
            }
        }
        read_reg_add_len--;
    }

    i2c_master_start();
    i2c_master_writeByte(ESP8266_I2C_SLAVE_ADDRESS_READ(slave_address));
    if(!i2c_master_checkAck())
    {
        //I2C DEVICE RESPONDED NACK
        if(_esp8266_i2c_debug)
        {
            os_printf("ESP8266 : I2C : Error. i2c slave responded NACK to i2c read address\n");
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
