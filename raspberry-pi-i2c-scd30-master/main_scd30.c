#include "scd30_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"
#include <stdio.h>  // printf

#define sensirion_hal_sleep_us sensirion_i2c_hal_sleep_usec

int main(void) {
    int16_t error = NO_ERROR;
    sensirion_i2c_hal_init();
    init_driver(SCD30_I2C_ADDR_61);

    // make sure the sensor is in a defined state (soft reset does not stop
    // periodic measurement)
    scd30_stop_periodic_measurement();
    scd30_soft_reset();
    sensirion_hal_sleep_us(200);

    uint8_t major = 0;
    uint8_t minor = 0;
    error = scd30_read_firmware_version(&major, &minor);
    if (error != NO_ERROR) {
        return -1;
    }
    error = scd30_start_periodic_measurement(0);
    if (error != NO_ERROR) {
        return -1;
    }

    float co2_concentration = 0.0;
    float temperature = 0.0;
    float humidity = 0.0;
    
    while(1)
    {
        printf("{");
        printf("\"SCD30\":");
        printf("[");
        printf("{");
        error = scd30_blocking_read_measurement_data(&co2_concentration,
                                                        &temperature, &humidity);
       
        printf("\"co2_concentration\":");
        
        if (error != NO_ERROR) 
            printf("%i", error);
            
        else
            printf("%.2f", co2_concentration);

        printf("}");
        printf("]");
        printf("}");
        
        sensirion_hal_sleep_us(90000);
    }
}
