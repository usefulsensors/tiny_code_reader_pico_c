// Copyright (c) 2022 Useful Sensors Inc.
// SPDX-License-Identifier: Apache-2.0
//
// Example code to communicate with a Useful Sensors' Tiny Code Reader.
// See the full developer guide at https://usfl.ink/tcr_dev for more information.
//   
// Demonstrates how to set up and receive information from a Tiny Code Reader. 
// Canonical home for this code is at 
// https://github.com/usefulsensors/tiny_code_reader_pico_c

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"
#include "pico/binary_info.h"

#include "hardware/i2c.h"

#include "tiny_code_reader.h"

// Speed of the I2C bus. The sensor supports many rates but 400KHz works.
const int32_t I2C_BAUD_RATE = (400 * 1000);

// How long to wait between reading the sensor. The sensor can be read as
// frequently as you like, but the results only change at about 5FPS, so
// waiting for 200ms is reasonable.
const int32_t SAMPLE_DELAY_MS = 200;

int main() {
    stdio_init_all();

    printf("Setting up i2c\n");

    // Use I2C0 on the default SDA and SCL pins (6, 7 on a Pico).
    i2c_init(i2c_default, I2C_BAUD_RATE);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    // Make the I2C pins available to picotool.
    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, 
      PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));

    printf("Setup done for i2c\n");

    tiny_code_reader_results_t results = {};
    while (1) {
        // Perform a read action on the I2C address of the sensor to get the
        // current code information detected.
        if (!tiny_code_reader_read(&results)) {
            printf("No code results found on the i2c bus\n");
            sleep_ms(SAMPLE_DELAY_MS);
            continue;
        }

        if (results.content_length == 0) {
            printf("No code found\n");
        } else {
            printf("Found '%s'\n", results.content_bytes);
        }

        sleep_ms(SAMPLE_DELAY_MS);
    }
    return 0;
}
