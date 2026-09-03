

#define DEBUG
#define INFO

/* Define the pins, used by commWaitForHubIdle for probing the hub state. */
#if defined(__AVR_ATtiny3224__) // defined(ATTINY_BOARD)
    // ATtiny3224 ALT2 UART pins
    #define PIN_RX PIN_PB3
    #define PIN_TX PIN_PB2
#else // ProMicro
    #define PIN_RX 0
    #define PIN_TX 1
#endif
// Enable Serial CDC (USB) for Atmega32u4 / Pro-Micro
#if defined(__AVR_ATmega32U4__)
#define SerialTTL    Serial1
#define DbgSerial    Serial
#elif defined(__AVR_ATtiny3224__) // The 3224 does not like swapping
#define SerialTTL    Serial       // On PB2 & PB3
#define DbgSerial    Serial1      // On PA1 & PA2
#else
#define SerialTTL    Serial
#endif

#define CURRENT_SENSOR_MODE_COUNT 10

//#define BUILD_LEGACY_SENSOR // Can have long mode names, but no mode flags
#define BUILD_MODERN_SENSOR   // Has short mode names, but has mode flags

