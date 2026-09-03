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
#include "TiltSensor.h"

/*try implementing if the hub is active before sending init sequence!*/

uint8_t sensorAngle[2]; // MODE_LPF2_ANGLE
uint8_t sensorTilt;  // MODE_LPF2_TILT


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

    myLPF2_device.setSensorDataPointer(MODE_LPF2_ANGLE, &sensorAngle[0]);
    myLPF2_device.setSensorDataPointer(MODE_LPF2_TILT,  &sensorTilt);

    connection_status = false;
}


unsigned long lastValueSwitchTime = 0;
void loop() {
    // Set arbitrary detected color
     if (millis() - lastValueSwitchTime >= 1000) {
        lastValueSwitchTime = millis();

        // 2. ALTERNATE THE VALUE: Switch between states
        if (sensorAngle[0] == 0x03) { 
          sensorAngle[0] = 0x09;
          sensorAngle[1] = -45;
          changeMask |= (1 << MODE_LPF2_ANGLE); // Set bit 0 to indicate a change in mode 8
        } else {
          sensorAngle[0] = 0x03;
          sensorAngle[1] = 30;
          changeMask |= (1 << MODE_LPF2_ANGLE); // Set bit 0 to indicate a change in mode 8
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
