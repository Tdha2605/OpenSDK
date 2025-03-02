#ifndef BQ27542_H
#define BQ27542_H

#include <stdint.h>

int bq27542_init();
int bq27542_read_register(uint8_t reg, uint16_t *value);
int bq27542_write_register(uint8_t reg, uint16_t value);
int bq27542_get_full_capacity(uint16_t *capacity);
int bq27542_get_remaining_capacity(uint16_t *capacity);
int bq27542_get_voltage(uint16_t *voltage);
int bq27542_get_capacity(uint16_t *capacity);
int bq27542_get_soc(uint16_t *soc);
int bq27542_get_temperature(float *temperature);
int bq27542_get_current(int16_t *current);
int bq27542_check_device_id();

#endif
