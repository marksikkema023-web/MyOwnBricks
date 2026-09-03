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
#include "BaseSensor.h"
#include "ColorDistanceSensor.h"
//#include "color_detection_methods.hpp"

/*try implementing if the hub is active before sending init sequence!*/

union ComboData { // The real combo mode needs to be tested in PyBricks
    int8_t sensorSpec1[4];
    struct __attribute__((packed)) {
        int8_t sensorColor;    // Mode 0
        int8_t sensorDistance; // Mode 1
        int8_t LEDColor;       // Mode 5
        int8_t reflectedLight; // Mode 3
    };
};ComboData comboData;
uint32_t sensorDetectioncount; // Mode 2
uint8_t  ambientLight;         // Mode 4
uint16_t sensorRGB[3];         // Mode 6
uint16_t IrTx;                 // Mode 7


BaseSensor myLPF2_device(PIN_RX, PIN_TX);

bool connection_status;
volatile uint16_t changeMask = 0; // toggle mode bit when value changes, to send data to the hub

/**
 * @brief Callback for LED brightness change.
 *
 * @note This sensor has 4 built-in lights: 2 above each eye and 2 below each eye.
 *      Set the brightness of each light.
 */
void LEDColorChanged() {
    // DO stuff here to set brightness of LED segments.
#if (defined(INFO) || defined(DEBUG))
    INFO_PRINT(F("Received LED color code: "));
    INFO_PRINTLN((uint8_t)comboData.LEDColor, HEX);
#endif
}

void sendIrTx() {
    // DO stuff here to set brightness of LED segments.
#if (defined(INFO) || defined(DEBUG))
    INFO_PRINT(F("Received IrTx: "));
    INFO_PRINT((uint8_t)(IrTx & 0x00FF) & 0x0F, HEX); 
    INFO_PRINT(F(", "));
    INFO_PRINT(((uint8_t)(IrTx & 0x00FF) >> 4) & 0x0F, HEX);
    INFO_PRINT(F(", "));
    INFO_PRINT((uint8_t)((IrTx >> 8) & 0x00FF) & 0x0F, HEX);
    INFO_PRINTLN();
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

    myLPF2_device.setSensorDataPointer(MODE_SPEC_1, &comboData.sensorSpec1[0]);

    myLPF2_device.setSensorDataPointer(MODE_COLOR, &comboData.sensorColor);
    myLPF2_device.setSensorDataPointer(MODE_PROX,  &comboData.sensorDistance);
    myLPF2_device.setSensorDataPointer(MODE_REFLT, &comboData.reflectedLight);
    myLPF2_device.setSensorDataPointer(MODE_COL_O, &comboData.LEDColor);

    myLPF2_device.setSensorDataPointer(MODE_COUNT, &sensorDetectioncount);
    myLPF2_device.setSensorDataPointer(MODE_AMBI,  &ambientLight);
    myLPF2_device.setSensorDataPointer(MODE_RGB_I, &sensorRGB[0]);
    myLPF2_device.setSensorDataPointer(MODE_IR_TX, &IrTx);

    myLPF2_device.setSensorCallback(MODE_COL_O, &LEDColorChanged);
    myLPF2_device.setSensorCallback(MODE_SPEC_1, &LEDColorChanged);
    myLPF2_device.setSensorCallback(MODE_IR_TX, &sendIrTx);

    connection_status = false;
}


unsigned long lastSensorSwitchTime = 0;
void loop() {
    // Set arbitrary detected color
     if (millis() - lastSensorSwitchTime >= 1000) {
        lastSensorSwitchTime = millis();

        // 2. ALTERNATE THE VALUE: Switch between states
        if (comboData.sensorColor == 0x03) { 
          comboData.sensorColor = 0x09;
          changeMask |= (1 << MODE_SPEC_1); // Set bit 0 to indicate a change in mode 8
        } else {
          comboData.sensorColor = 0x03;
          changeMask |= (1 << MODE_SPEC_1); // Set bit 0 to indicate a change in mode 8
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
