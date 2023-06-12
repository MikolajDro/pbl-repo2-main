#include <math.h>   // NAN
#include <stdio.h>  // printf

#include "sen5x_i2c.h"
#include "sensirion_common.h"
#include "sensirion_i2c_hal.h"

int main(void) {
    int16_t error = 0;

    sensirion_i2c_hal_init();

    error = sen5x_device_reset();
    if (error) {
        return -1;
    }

    unsigned char serial_number[32];
    uint8_t serial_number_size = 32;
    error = sen5x_get_serial_number(serial_number, serial_number_size);
    if (error) {
        return -1;
    }

    unsigned char product_name[32];
    uint8_t product_name_size = 32;
    error = sen5x_get_product_name(product_name, product_name_size);
    if (error) {
        return -1;
    }

    uint8_t firmware_major;
    uint8_t firmware_minor;
    bool firmware_debug;
    uint8_t hardware_major;
    uint8_t hardware_minor;
    uint8_t protocol_major;
    uint8_t protocol_minor;
    error = sen5x_get_version(&firmware_major, &firmware_minor, &firmware_debug,
                              &hardware_major, &hardware_minor, &protocol_major,
                              &protocol_minor);
    if (error) {
        return -1;
    }

    float temp_offset = 0.0f;
    error = sen5x_set_temperature_offset_simple(temp_offset);
    if (error) {
        return -1;
    }

    // Start Measurement
    error = sen5x_start_measurement();

    if (error) {
        printf("Error executing sen5x_start_measurement(): %i\n", error);
    }

    float mass_concentration_pm1p0;
    float mass_concentration_pm2p5;
    float mass_concentration_pm4p0;
    float mass_concentration_pm10p0;
    float ambient_humidity;
    float ambient_temperature;
    float voc_index;
    float nox_index;

    while(1) {
        sensirion_i2c_hal_sleep_usec(500000);
        error = sen5x_start_measurement();
        sensirion_i2c_hal_sleep_usec(500000);

        error = sen5x_read_measured_values(
            &mass_concentration_pm1p0, &mass_concentration_pm2p5,
            &mass_concentration_pm4p0, &mass_concentration_pm10p0,
            &ambient_humidity, &ambient_temperature, &voc_index, &nox_index);
        if (error) {
            return -1;
        } else {
            printf("\{");
            printf("\"Pm1\": %.1f, ",
                   mass_concentration_pm1p0);
            printf("\"Pm2.5\": %.1f, ",
                   mass_concentration_pm2p5);
            printf("\"Pm4\": %.1f, ",
                   mass_concentration_pm4p0);
            printf("\"Pm10\": %.1f, ",
                   mass_concentration_pm10p0);
            if (isnan(ambient_humidity)) {
                printf("\"Humidity\": \"%.1f\", ", ( float )error);
            } else {
                printf("\"Humidity\": \"%.1f\", ", ambient_humidity);
            }
            if (isnan(ambient_temperature)) {
                printf("\"Temperature\": \"%.1f\", ", ( float )error);
            } else {
                printf("\"Temperature\": \"%.1f\", ", ambient_temperature);
            }
            if (isnan(voc_index)) {
                printf("\"Voc index\": \"%.1f\", ", ( float )error);
            } else {
                printf("\"Voc index\": \"%.1f\", ", voc_index);
            }
            if (isnan(nox_index)) {
                printf("\"Nox index\": \"%.1f\"", ( float )error);
            } else {
                printf("\"Nox index\": \"%.1f\"", nox_index);
            }
            printf("}\n");
        }

        error = sen5x_stop_measurement();
        if (error) {
            printf("\{Error executing sen5x_stop_measurement(): %i}", error);
        }
    }
}
