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
#include "UltrasonicSensor.h"

union SetupData {
    int8_t sensorSpec1[4];
    struct {
            uint16_t sensorDISTL;
            uint8_t  sensorLIGHT[4];
    };
};
SetupData mySensor;
// Sensor device HC-SR04(RCWL-9300). I2C address: 0x57? 3.3v?
//Wire.begin();
// Enable internal pull-ups on the ATtiny3224 I2C pins if needed
//pinMode(PIN_PA1, INPUT_PULLUP); // SCL
//pinMode(PIN_PA2, INPUT_PULLUP); // SDA

BaseSensor myLPF2_device(PIN_RX, PIN_TX);

bool          connection_status;
volatile uint16_t changeMask = 0; // toggle mode bit when vale changes, to send data to the hub

/**
 * @brief Callback for LED brightness change.
 *
 * @note This sensor has 4 built-in lights: 2 above each eye and 2 below each eye.
 *      Set the brightness of each light.
 */
void LEDBrightnessesChanged() {
    // DO stuff here to set brightness of LED segments.
#if (defined(INFO) || defined(DEBUG))
    INFO_PRINT(F("Received LEDBrightnesses: "));
    INFO_PRINT(mySensor.sensorLIGHT[0], HEX);
    INFO_PRINT(F(", "));
    INFO_PRINT(mySensor.sensorLIGHT[1], HEX);
    INFO_PRINT(F(", "));
    INFO_PRINT(mySensor.sensorLIGHT[2], HEX);
    INFO_PRINT(F(", "));
    INFO_PRINTLN(mySensor.sensorLIGHT[3], HEX);
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

    myLPF2_device.setSensorDataPointer(MODE_DISTL, &mySensor.sensorDISTL);
    myLPF2_device.setSensorDataPointer(MODE_LIGHT, &mySensor.sensorLIGHT[0]);

    myLPF2_device.setSensorCallback(MODE_LIGHT, &LEDBrightnessesChanged);
    connection_status = false;
}


unsigned long lastSwitchTime = 0;
void loop() {
    // Set arbitrary detected color
     if (millis() - lastSwitchTime >= 1000) {
        lastSwitchTime = millis();

        // 2. ALTERNATE THE VALUE: Switch between states
        if (mySensor.sensorDISTL == 0xff) { 
          mySensor.sensorDISTL = 2500;
          changeMask |= (1 << MODE_DISTL); // Set bit 0 to indicate a change in mode 0
        } else {
          mySensor.sensorDISTL = 0xff;
          changeMask |= (1 << MODE_DISTL); // Set bit 0 to indicate a change in mode 0
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
