#ifndef CONFIG_H
#define CONFIG_H

#define DEBUG
#define INFO

/* Define the pins per development board also here!*/
#if defined(__AVR_ATtiny3224__) // defined(ATTINY_BOARD)
    // ATtiny3224 ALT2 UART pins
    #define PIN_RX PIN_PB3
    #define PIN_TX PIN_PB2
#else // ProMicro
    #define PIN_RX 0
    #define PIN_TX 1
#endif

#define CURRENT_SENSOR_MODE_COUNT 7 //

#define BUILD_LEGACY_SENSOR // Can have long mode names, but no mode flags
//#define BUILD_MODERN_SENSOR   // Has short mode names, but has mode flags

#endif // CONFIG_H