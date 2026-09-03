// ====== DeviceDescriptor.h ======
#pragma once
#include "lego_uart.h"
#include <config.h>


using DataType = uint8_t;

// Simple Version struct
struct Version {
    uint8_t Major;
    uint8_t Minor;
    uint8_t Bugfix;
    uint8_t Build;
};

struct Mode {
#if defined(BUILD_LEGACY_SENSOR)
    const char name[12];   // Legacy: allows long names up to 11 chars + \0
#elif defined(BUILD_MODERN_SENSOR)
    struct Flags {
        uint8_t data[6];
    };
    const char name[6];
#endif
    float rangeMin;
    float rangeMax;
    float pctMin;
    float pctMax;
    float siMin;
    float siMax;
    const char unit[4];
    uint8_t mappingIn;
    uint8_t mappingOut;
    uint8_t sampleCount;
    DataType dataType;
    uint8_t digits;
    uint8_t decimals;
    uint8_t dummy[2]; 
    uint8_t unknownByte;
#if defined(BUILD_MODERN_SENSOR)
    Flags flags; // Re-instated as a structured type
#endif
};

enum class DeviceType : uint8_t {
    UNKNOWNDEVICE                     = 0x00,
    WEDO2_TILT_SENSOR                 = 0x22,
    WEDO2_MOTION_SENSOR               = 0x23,
    BOOST_COLOR_DISTANCE_SENSOR       = 0x25,
    MEDIUM_LINEAR_MOTOR               = 0x26,              // BOOST Interactive Motor?
    TECHNIC_LARGE_LINEAR_MOTOR        = 0x2E,              // Technic Control+
    TECHNIC_XLARGE_LINEAR_MOTOR       = 0x2F,              // Technic Control+
    TECHNIC_MEDIUM_ANGULAR_MOTOR      = 0x30,
    TECHNIC_COLOR_SENSOR              = 0x3D,
    TECHNIC_DISTANCE_SENSOR           = 0x3E,
    TECHNIC_FORCE_SENSOR              = 0x3F,
    TECHNIC_COLOR_LIGHT_MATRIX        = 0x40,              // Not 100% sure about this one
    TECHNIC_SMALL_ANGULAR_MOTOR       = 0x41,              // Spike Prime, 6 volt reduced by PWM
    TECHNIC_MEDIUM_ANGULAR_MOTOR_GREY = 0x4B,              // Mindstorms
    TECHNIC_LARGE_ANGULAR_MOTOR_GREY  = 0x4C,              // Mindstorms
};

template <uint8_t ModeCount>
struct LPF2_DeviceProfile
{
    DeviceType type = DeviceType::UNKNOWNDEVICE;
    uint16_t inModesMask = 0;
    uint16_t outModesMask = 0;
    uint8_t caps = 0;
    uint16_t combos[16] = {0};
        
    Version fwVersion = {};
    Version hwVersion = {};
        
    // Sized by the template argument.
    Mode modes[CURRENT_SENSOR_MODE_COUNT] = {};
};
extern const LPF2_DeviceProfile<CURRENT_SENSOR_MODE_COUNT> g_device;
