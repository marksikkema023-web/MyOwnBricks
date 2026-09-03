/*
 * MyOwnBricks is a library for the emulation of PoweredUp sensors on microcontrollers
 * Copyright (C) 2022 Ysard - <ysard@users.noreply.github.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include <Arduino.h>
#include "config.h"
#include <BaseSensor.h>
#include "ColorSensor.h"

// Use the as5600 encoder

/*try implementing if the hub is active before sending init sequence!*/

union ComboData {
    uint8_t combo[16];              // Offset  0 (Bytes 0-15)  MODE_COMBO
    struct __attribute__((packed)) {
        uint8_t  color;             // Offset  0 (Byte 0)      MODE_COLOR
        uint8_t  reflection;        // Offset  1 (Byte 1)      MODE_REFLT
        uint16_t rgb_i[4];          // Offset  2 (Bytes 2-9)   MODE_RGB_I
        uint16_t hsv[3];            // Offset 10 (Bytes 10-15) MODE_HSV
    };
    uint8_t  ambient;               // MODE_AMBI
    uint8_t  light[3];              // MODE_LIGHT
    uint16_t reflection_raw[2];     // MODE_RREFL
    uint16_t SHSV[4];               // MODE_SHSV
}; ComboData mySensor{};

BaseSensor myLPF2_device(PIN_RX, PIN_TX);

bool connection_status;
volatile uint16_t changeMask = 0; // toggle mode bit when value changes, to send data to the hub

/**
 * @brief Callback for LED brightness change.
 *
 * @note This sensor has 4 built-in lights: 2 above each eye and 2 below each eye.
 *      Set the brightness of each light.
 */
void LEDBrightnessesChanged() { // MODE_LIGHT, Not implemented in the PoweredUp app.
    // DO stuff here to set brightness of LED segments.
#if (defined(INFO) || defined(DEBUG))
    INFO_PRINT(F("Received LEDBrightnesses: "));
    INFO_PRINT(mySensor.light[0], HEX);
    INFO_PRINT(F(", "));
    INFO_PRINT(mySensor.light[1], HEX);
    INFO_PRINT(F(", "));
    INFO_PRINTLN(mySensor.light[2], HEX);
#endif
}

void setup() {
//    pinMode(LED_BUILTIN, OUTPUT);
#if (defined(INFO) || defined(DEBUG))
    DbgSerial.begin(115200); // USB CDC
    while (!Serial) {
        // Wait for serial port to connect.
    }
#endif

//    myLPF2_device.setSensorDataPointer(MODE_COMBO, mySensor.combo);
// Only MODE_COLOR gets used for the Powered Up app
    myLPF2_device.setSensorDataPointer(MODE_COLOR, &mySensor.color);

    myLPF2_device.setSensorCallback(MODE_LIGHT, &LEDBrightnessesChanged);
    connection_status = false;
}


unsigned long lastColorSwitchTime = 0;
void loop() {
    // Set arbitrary detected color
     if (millis() - lastColorSwitchTime >= 1000) {
        lastColorSwitchTime = millis();

        // 2. ALTERNATE THE VALUE: Switch between states
        if (mySensor.color == 0x03) { 
          mySensor.color = 0x09;
          changeMask |= (1 << MODE_COLOR); // Mark the mode as changed to notify the hub
        } else {
          mySensor.color = 0x03;
          changeMask |= (1 << MODE_COLOR); // Mark the mode as changed to notify the hub
        }
    }
    // Send data to PoweredUp Hub
    myLPF2_device.process();

    if (myLPF2_device.isConnected()) {
        // Already connected ?
        if (!connection_status) {
            INFO_PRINTLN(F("Connected !"));
//            pinMode(LED_BUILTIN_TX, INPUT);
//            pinMode(LED_BUILTIN_RX, INPUT);
            connection_status = true;
        }
    } else {
        INFO_PRINTLN(F("Not Connected !"));
//        pinMode(LED_BUILTIN_TX, OUTPUT);
//        pinMode(LED_BUILTIN_RX, OUTPUT);
        connection_status = false;
    }

#if (defined(INFO) || defined(DEBUG))
    DbgSerial.flush();
#endif
}

/* The folowing modes where tested on PyBricks and implemented for the old library.

 **
 * @brief Mode 4 response (read): Send CALIB values, 4 bytes, raw angle 0-1023 angle degrees 0-3599.
 * @note This mode is used by Pybricks
 *
void CustomSensor::sensorSwitchMode_4(){
    // Mode 4
    *m_SensorAccAngle = (*m_SensorRawAngle * 3600L) / 4096L; // needs to be a dedicated int16_t
    m_txBuf[0] = 0xD4;
    m_txBuf[1] = (m_SensorRawAngle[0] >> 2) & 0xFF;                  // Send LSB of raw angle
    m_txBuf[2] = (m_SensorRawAngle[0] >> 10) & 0xFF;                 // Send MSB of raw angle
    m_txBuf[3] = m_SensorAccAngle[0] & 0xFF;                         // Send LSB of angle degrees
    m_txBuf[4] = (m_SensorAccAngle[0] >> 8) & 0xFF;                  // Send MSB of angle degrees
    sendUARTBuffer(4);
}

 **
 * @brief Mode 5 response (read): Send STATS values, 32 bytes
 *
 * @note Most values seem to be static, only 2 values drift.
 *      They Seem to be some Raw thermistor value (unscaled) vs Processed temperature or voltage reference.
 *      Found values from 273 -> 401 and 391 -> 519, with always a 118 difference
 *
void CustomSensor::sensorSwitchMode_5(){
    // Mode 5
    m_txBuf[0] = 0xED;                                               // Header
    m_txBuf[1] = 0x03;                                               // LSB Static value
    m_txBuf[2] = 0x00;                                               // MSB
    m_txBuf[3] = 0x00;
    m_txBuf[4] = 0x00;
    m_txBuf[5] = 0x02;                                               // LSB Static value
    m_txBuf[6] = 0x00;                                               // MSB
    m_txBuf[7] = 0x00;
    m_txBuf[8] = 0x00;
    m_txBuf[9] = 0x03;                                               // LSB Static value
    m_txBuf[10] = 0x00;                                              // MSB
    m_txBuf[11] = 0x08;                                              // LSB Static value
    m_txBuf[12] = 0x00;                                              // MSB
    m_txBuf[13] = 0x11;                                              // LSB 0x0111=273
    m_txBuf[14] = 0x01;                                              // MSB
    m_txBuf[15] = 0x20;                                              // LSB Static value
    m_txBuf[16] = 0x00;                                              // MSB
    m_txBuf[17] = 0x09;                                              // LSB Static value
    m_txBuf[18] = 0x00;                                              // MSB
    m_txBuf[19] = 0x0D;                                              // LSB Static value
    m_txBuf[20] = 0x00;                                              // MSB
    m_txBuf[21] = 0x05;                                              // LSB Static value
    m_txBuf[22] = 0x00;                                              // MSB
    m_txBuf[23] = 0x05;                                              // LSB Static value
    m_txBuf[24] = 0x00;                                              // MSB
    m_txBuf[25] = 0x03;                                              // LSB Static value
    m_txBuf[26] = 0x00;                                              // MSB
    m_txBuf[27] = 0x87;                                              // LSB 0x0187=391
    m_txBuf[28] = 0x01;                                              // MSB
    m_txBuf[29] = 0x00;
    m_txBuf[30] = 0x00;
    m_txBuf[31] = 0x00;
    m_txBuf[32] = 0x00;
    sendUARTBuffer(32);
}
*/