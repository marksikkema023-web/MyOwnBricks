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
#include "ForceSensor.h"
//#include "color_detection_methods.hpp"

/*try implementing if the hub is active before sending init sequence!*/

union ComboData { // The real combo mode needs to be tested in PyBricks
    int8_t sensorCombo[4];      // MODE_COMBO
    struct __attribute__((packed)) {
        int8_t sensorForce;     // MODE_FORCE
        int8_t sensorTouched;   // MODE_TOUCHED
        int16_t sensorRawForce; // MODE_FRAW
    };
};ComboData comboData;
uint8_t sensorTapped;           // MODE_TAPPED

BaseSensor myLPF2_device(PIN_RX, PIN_TX);

bool connection_status;
volatile uint16_t changeMask = 0; // toggle mode bit when value changes, to send data to the hub

void setup() {
//    pinMode(LED_BUILTIN, OUTPUT);
#if (defined(INFO) || defined(DEBUG))
    DbgSerial.begin(115200); // USB CDC
    while (!Serial) {
        // Wait for serial port to connect.
    }
#endif

//    myLPF2_device.setSensorDataPointer(MODE_COMBO, &comboData.sensorCombo[0]);

    myLPF2_device.setSensorDataPointer(MODE_FORCE, &comboData.sensorForce);
    myLPF2_device.setSensorDataPointer(MODE_TOUCHED, &comboData.sensorTouched);
    myLPF2_device.setSensorDataPointer(MODE_FRAW, &comboData.sensorRawForce);

    myLPF2_device.setSensorDataPointer(MODE_TAPPED, &sensorTapped);

    connection_status = false;
}


unsigned long lastSensorSwitchTime = 0;
void loop() {
    // Set arbitrary detected color
     if (millis() - lastSensorSwitchTime >= 1000) {
        lastSensorSwitchTime = millis();

        // 2. ALTERNATE THE VALUE: Switch between states
        if (comboData.sensorForce == 0x03) { 
          comboData.sensorForce = 0x09;
          changeMask |= (1 << MODE_FORCE); // Set bit 0 to indicate a change in mode 8
        } else {
          comboData.sensorForce = 0x03;
          changeMask |= (1 << MODE_FORCE); // Set bit 0 to indicate a change in mode 8
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
