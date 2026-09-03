/*
 * A library for the emulation of PoweredUp sensors on microcontrollers
 * Copyright (C) 2021-2023 Ysard - <ysard@users.noreply.github.com>
 *
 * Based on the original work of Ahmed Jouirou - <ahmed.jouirou@gmail.com>
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
#ifndef COLOR_DISTANCESENSOR_H
#define COLOR_DISTANCESENSOR_H

// Colors (detected & LED (except NONE for this last one)) expected values
#define COLOR_NONE      0xFF
#define COLOR_BLACK     0
#define COLOR_PINK      1
#define COLOR_PURPLE    2
#define COLOR_BLUE      3
#define COLOR_LIGHTBLUE 4
#define COLOR_CYAN      5
#define COLOR_GREEN     6
#define COLOR_YELLOW    7
#define COLOR_ORANGE    8
#define COLOR_RED       9
#define COLOR_WHITE     10

/**
 * @brief Handle the LegoUART protocol and define modes of the
 * Color & Distance sensor.
 *
 * @param m_LEDColor Current color of the LED; Available values:
 *      COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_YELLOW, COLOR_RED, COLOR_WHITE.
 * @param m_sensorDistance Distance measured to the the nearest object.
 *      Continuous values 0...10.
 * @param m_detectionCount Detection count; should be incremented each time
 *      the sensor detects a distance < 5cm.
 * @param m_reflectedLight Reflected light (from clear channel value or
 *      calculations based on rgb channels).
 *      Continuous values 0...100.
 * @param m_ambientLight Ambient light based on lux value.
 *      Continuous values 0...100.
 * @param m_sensorRGB Raw values of Red Green Blue channels.
 *      Values should not exceed experimentally observed value of ~440.
 *      Continuous values 0..1023.
 * @param m_sensorColor Detected color; Available values:
 *      COLOR_NONE, COLOR_BLACK, COLOR_BLUE, COLOR_GREEN, COLOR_YELLOW, COLOR_RED, COLOR_WHITE.
 * @param m_IR_code IR code for Power Functions IR devices
 *      (supposed to be transmitted via the Power Functions RC Protocol).
 * @param m_pIRfunc Callback set by user receiving m_IR_code, when it's changed by the hub.
 * @param m_pLEDColorfunc Callback set by user receiving m_LEDColor, when it's changed by the hub.
 *
 * @param m_currentExtMode Extended mode switch for modes >= 8. Available values:
 *      EXT_MODE_0, EXT_MODE_8.
 */
    // LEGO POWERED UP Color and Distance Sensor modes
    // https://github.com/pybricks/pybricks-micropython/blob/master/pybricks/util_pb/pb_device.h
enum {
    MODE_COMBO  =  0, // ???
    MODE_COLOR  =  0, // read 1x int8_t
    MODE_PROX   =  1, // read 1x int8_t
    MODE_COUNT  =  2, // read 1x int32_t
    MODE_REFLT  =  3, // read 1x int8_t
    MODE_AMBI   =  4, // read 1x int8_t
    MODE_COL_O  =  5, // writ 1x int8_t
    MODE_RGB_I  =  6, // read 3x int16_t
    MODE_IR_TX  =  7, // writ 1x int16_t
    MODE_SPEC_1 =  8, // rrwr 4x int8_t
    MODE_DEBUG  =  9, // ?? 2x int16_t
    MODE_CALIB  = 10, // ?? 8x int16_t
};
#endif
