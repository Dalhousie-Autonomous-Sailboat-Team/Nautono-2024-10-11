#include "sail_debug.h"
#include "sail_tasksinit.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"

#include "sail_ina.h"
#include "sail_i2c.h"

static uint8_t reg[NUM_REGISTERS] = {
	0x2,
	0x4,
	0x6,
	0x1,
	0x3,
	0x5
};

static enum status_code ReadWord(I2C_DeviceID ina, uint8_t reg, uint16_t *data){
   if(data == NULL) {
      return STATUS_ERR_BAD_DATA;
   }

   if(I2C_WriteBuffer(ina, &reg, 1, I2C_WRITE_NORMAL) != STATUS_OK) {
      return STATUS_ERR_DENIED;
   }

   if(I2C_ReadBuffer(ina, (uint8_t*)data, 2, I2C_READ_NORMAL) != STATUS_OK) {
      return STATUS_ERR_DENIED;
   }

   return STATUS_OK;
}

float ReadVoltage(I2C_DeviceID ina, int channel) {
    float voltage_V = 0;
    uint8_t reggie = 0;
    uint16_t val_raw = 0;

    switch (channel) {
        case 0:
            reggie = reg[CH1_BUSV];
            break;
        case 1:
            reggie = reg[CH2_BUSV];
            break;
        case 2:
            reggie = reg[CH3_BUSV];
            break;
    }
    ReadWord(ina, reggie, &val_raw);
	
	val_raw = SWAP_BYTES_16_BITS(val_raw);//for changing endianness

    voltage_V = (float)val_raw / 1000.0;

    return voltage_V;
}

static int32_t getShuntVoltage(I2C_DeviceID ina, int channel){
    int32_t res;
    uint8_t reggie;
    uint16_t val_raw = 0;

    switch (channel) {
        case 0:
            reggie = reg[CH1_SHUNTV];
            break;
        case 1:
            reggie = reg[CH2_SHUNTV];
            break;
        case 2:
            reggie = reg[CH3_SHUNTV];
            break;
    }
	
    ReadWord(ina, reggie, &val_raw);

	val_raw = SWAP_BYTES_16_BITS(val_raw);
	
    // 1 Least Significant Bit = 40uV
	res = (int32_t)(val_raw >> 3) * 40;

    return res;
}

float ReadCurrent(I2C_DeviceID ina, int channel) {
    int32_t shunt_uV = 0;
    float current_A  = 0;
    int32_t shuntRes = 100;

    shunt_uV  = getShuntVoltage(ina, channel);
    current_A = shunt_uV / (1000.0 * shuntRes); // in the void INA3221::begin(TwoWire *theWire) of the original function they are set to 10
    return current_A;
}

#define TEST_INA_DELAY_MS 1000

void Test_INA(void){
	TickType_t testDelay = pdMS_TO_TICKS(TEST_INA_DELAY_MS);

	while(1){
		taskENTER_CRITICAL();
		watchdog_counter |= 0x20;
		taskEXIT_CRITICAL();
		running_task = eUpdateCourse;
		
		for(int i = 1; i < 2; i++)
		{
			DEBUG_Write("The INA is %d and channel is %d and voltage is %d mV and current is %d mA\r\n", I2C_INA1 - 2, i, (int)(ReadVoltage(I2C_INA1, i)*1000), (int)(ReadCurrent(I2C_INA1, i) * 1000));
			DEBUG_Write("The INA is %d and channel is %d and voltage is %d and current is %d\r\n", I2C_INA2 - 2, i, (int)ReadVoltage(I2C_INA2, i), ReadCurrent((I2C_INA2, i));
			DEBUG_Write("The INA is %d and channel is %d and voltage is %d and current is %d\r\n", I2C_INA3 - 2, i, (int)ReadVoltage(I2C_INA3, i), ReadCurrent((I2C_INA3, i));

		}
		
		vTaskDelay(testDelay);
	}
}
