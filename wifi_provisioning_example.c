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

#include "pico/binary_info.h"
#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"

#include "hardware/i2c.h"

#include "string_utils.h"
#include "tiny_code_reader.h"

// Speed of the I2C bus. The sensor supports many rates but 400KHz works.
const int32_t I2C_BAUD_RATE = (400 * 1000);

// How long to wait between reading the sensor. The sensor can be read as
// frequently as you like, but the results only change at about 5FPS, so
// waiting for 200ms is reasonable.
const int32_t SAMPLE_DELAY_MS = 200;

static void connect_to_wifi_with_qr_code(void) {
    tiny_code_reader_results_t results = {};
    bool is_connected = false;
    while (!is_connected) {
        // Perform a read action on the I2C address of the sensor to get the
        // current code information detected.
        if (!tiny_code_reader_read(&results)) {
            printf("No code results found on the i2c bus\n");
            sleep_ms(SAMPLE_DELAY_MS);
            continue;
        }

        if (results.content_length > 0) {
            char* content_string = malloc(results.content_length + 1);
            memcpy(content_string, results.content_bytes, results.content_length);
            content_string[results.content_length] = 0;
            // Example wifi provisioning text:
            // WIFI:S:useful_sensors;T:WPA;P:somepassword;H:false;;
            if (string_starts_with(content_string, "WIFI:")) {
                char* settings_string = string_duplicate(content_string + 5);

                char** settings = NULL;
                int settings_length = 0;
                string_split(settings_string, ';', -1, &settings, &settings_length);

                char* wifi_ssid = NULL;
                char* wifi_password = NULL;
                for (int i = 0; i < settings_length; ++i) {
                    char* setting = settings[i];
                    char** setting_parts = NULL;
                    int setting_parts_length = 0;
                    string_split(setting, ':', -1, &setting_parts, &setting_parts_length);
                    if ((setting_parts_length == 2) && (strlen(setting_parts[0]) == 1)) {
                        const char setting_key = setting_parts[0][0];
                        switch (setting_key) {
                            case 'S': {
                                wifi_ssid = string_duplicate(setting_parts[1]);
                            } break;
                            
                            case 'P': {
                                wifi_password = string_duplicate(setting_parts[1]);
                            } break;
                            
                            case 'T': {
                                // Ignore.
                            } break;

                            case 'H': {
                                // Ignore.
                            } break;

                            default: {
                                printf("Unknown key '%c' in %s\n", setting_key, content_string);
                            } break;
                        }
                    }
                    string_list_free(setting_parts, setting_parts_length);
                }
                string_list_free(settings, settings_length);
                free(settings_string);

                printf("Trying to connect to %s:%s\n", wifi_ssid, wifi_password);
                // Sleep to give the preceding log statement a chance to process before connecting.
                sleep_ms(100);
                if (cyw43_arch_wifi_connect_timeout_ms(wifi_ssid, wifi_password, CYW43_AUTH_WPA2_AES_PSK, 10000)) {
                    printf("Failed to connect to '%s'.\n", content_string);
                } else {
                    is_connected = true;
                    printf("Connected to '%s'.\n", content_string);
                }
                free(wifi_ssid);
                free(wifi_password);
            } else {
                printf("Non-wifi QR code found: '%s'\n", content_string);
            }
            free(content_string);
        }
        sleep_ms(SAMPLE_DELAY_MS);
    }
}

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

    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    connect_to_wifi_with_qr_code();

    cyw43_arch_deinit();

    return 0;
}
