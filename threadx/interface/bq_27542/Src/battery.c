#include "battery.h"
#include "ql_iic.h"
#include <stdint.h>

#define BQ27542_I2C_NO 0
#define BQ27542_I2C_ADDR 0x55
#define BQ27542_REG_DEVICE_ID 0x00
#define BQ27542_REG_VOLTAGE 0x04
#define BQ27542_REG_CAPACITY 0x2C
#define BQ27542_REG_SOC 0x2E
#define BQ27542_REG_TEMP 0x06
#define BQ27542_REG_CURRENT 0x10
#define BQ27542_REG_NOM_CAPACITY 0x0C
#define BQ27542_REG_AVAIL_CAPACITY 0x0E
#define BQ27542_REG_REM_CAPACITY 0x10
#define BQ27542_REG_FULL_CAPACITY 0x12
#define BQ27542_REG_FULL_CAP_FIL 0x18
#define BQ27542_REG_FULL_CAP_UNFL 0x1C
#define BQ27542_REG_REM_CAP_UNFL 0x20
#define BQ27542_REG_REM_CAP_FIL 0x22

int bq27542_init() {
    return ql_i2c_init(BQ27542_I2C_NO, FAST_MODE);
}

int bq27542_read_register(uint8_t reg, uint16_t *value) {
    uint8_t data[2] = {0};
    int ret = ql_i2c_read(BQ27542_I2C_NO, BQ27542_I2C_ADDR, reg, data, 2);
    if (ret == 0) {
        *value = (data[1] << 8) | data[0];
        return 0;
    }
    return -1;
}

int bq27542_write_register(uint8_t reg, uint16_t value) {
    uint8_t data[2] = {value & 0xFF, (value >> 8) & 0xFF};
    return ql_i2c_write(BQ27542_I2C_NO, BQ27542_I2C_ADDR, reg, data, 2);
}

int bq27542_get_full_capacity(uint16_t *capacity) {
    return bq27542_read_register(BQ27542_REG_FULL_CAPACITY, capacity);
}

int bq27542_get_remaining_capacity(uint16_t *capacity) {
    return bq27542_read_register(BQ27542_REG_REM_CAPACITY, capacity);
}

int bq27542_get_voltage(uint16_t *voltage) {
    return bq27542_read_register(BQ27542_REG_VOLTAGE, voltage);
}

int bq27542_get_capacity(uint16_t *capacity) {
    return bq27542_read_register(BQ27542_REG_CAPACITY, capacity);
}

int bq27542_get_soc(uint16_t *soc) {
    return bq27542_read_register(BQ27542_REG_SOC, soc);
}

int bq27542_get_temperature(float *temperature) {
    uint16_t raw_temp;
    if (bq27542_read_register(BQ27542_REG_TEMP, &raw_temp) == 0) {
        *temperature = (raw_temp * 0.1) - 273.15;
        return 0;
    }
    return -1;
}

int bq27542_get_current(int16_t *current) {
    return bq27542_read_register(BQ27542_REG_CURRENT, (uint16_t*)current);
}

int bq27542_check_device_id() {
    uint16_t device_id;
    if (bq27542_read_register(BQ27542_REG_DEVICE_ID, &device_id) == 0) {
        return device_id == 0x542;
    }
    return -1;
}
