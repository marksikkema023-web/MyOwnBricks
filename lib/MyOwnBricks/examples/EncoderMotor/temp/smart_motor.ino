/*
 * MyOwnBricks is a library for the emulation of PoweredUp sensors on microcontrollers
 * Copyright (C) 2021-2023 Ysard - <ysard@users.noreply.github.com>
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
 *c:\Users\gring\Documents\Arduino\MyOwnBricks-master\MyOwnBricks-JorgePe\examples\custom_sensor\CustomSensor.cpp
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#define DEBUG
#define INFO

#if defined(__AVR_ATtiny3224__)
    #pragma message "Compiling for ATtiny3224 - Custom settings applied."
#endif

#if F_CPU >= 10000000UL
    #pragma message "Compiling for 10Mhz or above" // The code is barely keeping up on 10Mhz
#elif F_CPU < 10000000UL
    #pragma message "Clock speed probably too low!"
#endif

#include "SmartMotor.h"
#include "AS5600.h"
#include <Wire.h>

// Try the Constant power on pin 2 flag. Bit 7 in power
// Test the PWM frequency and make filter


// MAX_RPM should be able to handle 125% speed (70RPM on 7.2 volt, 83 on 9v)
// we are not using the scale factor to speed procentage, as the hub's PID is based on the absolute- and accumulated angle
//#define MAX_RPM 83 //Small Angular noload=110/load=85 | blue GeekServo contious noload 90/load=??? Need to test in PUP app on full power
//#define SCALE_FACTOR (110.0 / MAX_RPM)
#define SCALE_FACTOR 0.955 // scale to be liniar with the speed in the app

int8_t  sensorRotation    = 0; // Rotation speed
int32_t sensorAccumulated = 0; // Accumulated angle
int16_t sensorAbsolute    = 0; // Absolute angle
int16_t sensorRaw         = 0; // Raw angle
bool    connection_status;
float   rawRotation       = 0;

// INPUT_PULLUP???
//    pinMode(LED_BUILTIN, OUTPUT);???
//            pinMode(LED_BUILTIN_TX, INPUT);
//            pinMode(LED_BUILTIN_RX, INPUT);
// what is header 0x46? CMD_EXT_MODE
// What is CombosMode? See ColorSensor.cpp
// INFO_MODE_COMBOS              0x06    // INFO command - COMBOS  (mode combinations - LPF2-only)
// \x88\x06\....
// Take care of floats
// Update rate: 100 Hz, need to test this?
// speed = % not working, the hub uses abs angle and acc angle for PID. So speed is based on small angular motor??? Maybe change the init specs?
// The small anglular motor gets a duty cycle of about 2/3 duty, nomatter if on 9v or 7.2v. Have to test more precise with the propper duty cycle measuring tool.
// More testing on getStableRPM, it seems to ugly
// Not working if IDE is not open!
// Would the code work on 115200 baud
// Why no DEBUG in SmartMotor.cpp handling modes
// Updating every 100Hz = 10.000 microseconds
// make hub emulator
CustomSensor myOwnSwitch(& sensorRotation, & sensorAccumulated, & sensorAbsolute, & sensorRaw); // pointers to the needed variables]
AS5600 as5600; 

void setup() {
    pinMode(0, OUTPUT); // debug led
#if defined(__AVR_ATtiny3224__)
    // ATtiny414 specific: Move UART to alternative pins (PA3/PA4) ???
    // to leave PA1/PA2 free for I2C (Wire) ???
    // Attiny3224
    // TX = PA0 ???
    // RX = PA1 ???
    // SDA = PA2 → Arduino pin 2 ???
    // SCL = PA3 → Arduino pin 3 ???
//    Serial.swap(1); // Swapping doesnt seem to work for Serial1
//    Serial1.swap(1); // Swapping doesnt seem to work for Serial1
#endif

#if (defined(INFO) || defined(DEBUG))
    DbgSerial.begin(115200); // USB CDC
        while (!DbgSerial) {
        // Wait for serial port to connect.
    }
#endif

    // Default values
// ...

    // Device config
    Wire.begin();
    // Set I2C to Fast Mode (400kHz)
    Wire.setClock(400000);
    // Initialize without a specific DIR pin to test I2C first
    as5600.begin();

#if (defined(INFO) || defined(DEBUG))
    int b = as5600.isConnected();
    INFO_PRINT("Connect: ");
    INFO_PRINTLN(b);
    // Print the TWBR register value
    INFO_PRINT("I2C TWBR or TWI0.MBAUD Register: "); // 12 = 400kHz or 6 on the 3224
    INFO_PRINTLN(TWBR);
#endif
    connection_status = false;
}

// Loops per second without code on 10Mhz = 144187
//                  with    code on 10Mhz =   2526 98.2%
//                  without code on 16Mhz = 231143
//                  with    code on 16Mhz =   3342

// Variables used for calculating processor utilization
unsigned long loopCount = 0; // 
unsigned long lastReport = 0;

int32_t last_value;
void loop() {
    loopCount++; // Keep track of how fast the loop is spinning

//     if (sensorReady) { // See color_sensor.ino

    // Get data from AS5600
    sensorRaw         = as5600.readAngle();
    rawRotation       = getStableRPM(sensorRaw, false);
    sensorAccumulated = getCumulativeAngle(sensorRaw);
    sensorAbsolute    = sensorRaw * AS5600_RAW_TO_DEGREES;
    // Watch out: AS5600_RAW_TO_DEGREES is a float
    sensorAbsolute    = sensorRaw * AS5600_RAW_TO_DEGREES;
    sensorRotation    = (int8_t)constrain(round(rawRotation*SCALE_FACTOR), -125, 125); // should not be above 125% or under -125%
    if (sensorAbsolute > 179)
        sensorAbsolute = sensorAbsolute - 360;

    // Send data to PoweredUp Hub
    myOwnSwitch.process(); 
    digitalWrite(0, !digitalRead(0));  // change state of the LED by setting the pin to the HIGH voltage level

    if (myOwnSwitch.isConnected()) {
        // Already connected ?
        if (!connection_status) {
            INFO_PRINTLN(F("Connected !"));
            connection_status = true;
        }
    } else {
        INFO_PRINTLN(F("Not Connected !"));
        connection_status = false;
    }
/*
// calculate processor utilization
    if (millis() - lastReport >= 1000) {
        Serial1.print("Loops per second: ");
        Serial1.println(loopCount);
    
        loopCount = 0;
        lastReport = millis();
    }
*/
/*#if (defined(INFO) || defined(DEBUG))
    DbgSerial.flush();
#endif*/
}

/**
 * @brief Get stable RPM, as the library it's function getAngularSpeed() is giving instable results when called too often
 *
 * @note need to re-test this and make it more efficient.
 */
float    _lastStableSpeed = 0;
uint32_t _lastMeasurement = 0;
int16_t  _lastAngle       = 0;
float getStableRPM(int16_t _lastReadAngle, bool update) 
// Only good upto 300RPM, otherwise configur lastspeedcheck to less than 100
{
    static uint32_t lastSpeedCheck = 0;
    uint32_t now = millis();

    // ONLY calculate if 100ms have passed
    if (now - lastSpeedCheck < 100) {
    // Return the last calculated speed if called too early
        return _lastStableSpeed; 
    }

    if (update) {
        _lastReadAngle = as5600.readAngle();
    }

    uint32_t currentMicros = micros();
    int angle = _lastReadAngle;
    uint32_t deltaT = currentMicros - _lastMeasurement; // Still use micros for math accuracy
    int deltaA = angle - _lastAngle;

    // Standard wrap-around logic
    if (deltaA > 2048) deltaA -= 4096;
    else if (deltaA < -2048) deltaA += 4096;

    // Calculate Speed
    float speed = (deltaA * 1e6) / deltaT;
  
    // Update trackers
    _lastMeasurement = currentMicros;
    _lastAngle = angle;
    lastSpeedCheck = now;

    _lastStableSpeed = speed * AS5600_RAW_TO_RPM;
    return _lastStableSpeed;
}


/**
 * @brief Get cumulative angle, as the libraries function getCumulativePosition() is misbehaving around 0
 *
 * @note ...
 */
int16_t lastRawAngle = -1; // Flag to initialize
int32_t getCumulativeAngle(int16_t currentRawAngle) {

    if (lastRawAngle == -1) {
        lastRawAngle = currentRawAngle;
        return 0; // RETURNS NOTHING!!!
    }

    // Calculate the shortest distance between angles
    int16_t delta = currentRawAngle - lastRawAngle;

    // Handle the wrap-around (4095 <-> 0)
    if (delta > 2048)  delta -= 4096;
    if (delta < -2048) delta += 4096;

    // We now have the 'delta' in raw ticks. 
    // Add it to a running raw total.
    static int32_t totalTicks = 0;
    totalTicks += delta;

    lastRawAngle = currentRawAngle;

    // Convert the running total to degrees
    return (totalTicks * 360) / 4096;
}


