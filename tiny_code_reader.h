#ifndef INCLUDE_TINY_CODE_READER_H
#define INCLUDE_TINY_CODE_READER_H

// Definitions for the Useful Sensors Tiny Code Reader module.
// Includes the standard I2C address of the sensor, constants for the
// configuration commands, and the data structures used to communicate results
// to the main system.
// See the full developer guide at https://usfl.ink/tcr_dev for more information.

#include <stdint.h>

// The I2C address of the tiny code reader board.
#define TINY_CODE_READER_I2C_ADDRESS (0x0c)

// Configuration commands for the sensor. Write this as a byte to the I2C bus
// followed by a second byte as an argument value.
#define TINY_CODE_READER_REG_LED_STATE        (0x01)

// The following structures represent the data format returned from the code
// reader over the I2C communication protocol. The C standard doesn't
// guarantee the byte-wise layout of a regular struct across different
// platforms, so we add the non-standard (but widely supported) __packed__
// attribute to ensure the layouts are the same as the wire representation.

// The results returned from the module have a 16-bit unsigned integer
// representing the length of the code content, followed by 254 bytes
// containing the content itself, commonly as a UTF-8 string.
typedef struct __attribute__ ((__packed__)) {
    uint16_t content_length;
    uint8_t content_bytes[254];
} tiny_code_reader_results_t;

// Fetch the latest results from the sensor. Returns false if the read didn't
// succeed.
inline bool tiny_code_reader_read(tiny_code_reader_results_t* results) {
    int num_bytes_read = i2c_read_blocking(
        i2c_default,
        TINY_CODE_READER_I2C_ADDRESS, 
        (uint8_t*)(results), 
        sizeof(tiny_code_reader_results_t), 
        false);
    return (num_bytes_read == sizeof(tiny_code_reader_results_t));
}

// Writes the value to the sensor register over the I2C bus.
inline void tiny_code_reader_write_reg(uint8_t reg, uint8_t value) {
    uint8_t write_bytes[2] = {reg, value};
    i2c_write_blocking(
        i2c_default,
        TINY_CODE_READER_I2C_ADDRESS, 
        write_bytes,
        2,
        false);
}

#endif  // INCLUDE_TINY_CODE_READER_H