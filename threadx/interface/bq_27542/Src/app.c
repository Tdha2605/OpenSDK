#include "battery.h"
#include <stdio.h>
#include "ql_application.h"
#include "ql_iic.h"

static void battery_test() {
    uint16_t voltage, full_capacity, remaining_capacity, soc;
    float temperature;
    int16_t current;

    if (bq27542_init() != 0) {
        printf("BQ27542 Initialization Failed\n");
        return;
    }

    if (bq27542_check_device_id() == 1)
        printf("BQ27542 Detected\n");
    else {
        printf("BQ27542 Not Found\n");
        return;
    }

    if (bq27542_get_voltage(&voltage) == 0)
        printf("Voltage: %d mV\n", voltage);

    if (bq27542_get_full_capacity(&full_capacity) == 0)
        printf("Full Charge Capacity: %d mAh\n", full_capacity);

    if (bq27542_get_remaining_capacity(&remaining_capacity) == 0)
        printf("Remaining Capacity: %d mAh\n", remaining_capacity);

    if (bq27542_get_soc(&soc) == 0)
        printf("State of Charge: %d %%\n", soc);

    if (bq27542_get_temperature(&temperature) == 0)
        printf("Temperature: %.2f °C\n", temperature);

    if (bq27542_get_current(&current) == 0)
        printf("Current: %d mA\n", current);
}

application_init(battery_test, "Battery Test", 2, 10);

