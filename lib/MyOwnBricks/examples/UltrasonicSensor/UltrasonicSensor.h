/*
 * A library for the emulation of PoweredUp sensors on microcontrollers
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
#ifndef ULTRASONIC_SENSOR_H
#define ULTRASONIC_SENSOR_H

/**
 * @brief Handle the LegoUART protocol and define modes of the
 *      Spike/Technic Ultrasonic Sensor.
 */
    enum {
        MODE_DISTL = 0,  // read 1x int16_t Long distance measurements
        MODE_DISTS = 1,  // read 1x int16_t Short distance measurements
        MODE_SINGL = 2,  // read 1x int16_t Single object detection
        MODE_LISTN = 3,  // writ? 1x int8_t Listen for IR beacons/remotes
        MODE_TRAW  = 4,  // read 1x int32_t Raw sensor values
        MODE_LIGHT = 5,  // writ 4x int8_t
        MODE_PING  = 6,  // read 1x int8_t
        MODE_ADRAW = 7,  // read 1x int16_t
        MODE_CALIB = 8,  // ??   7x int8_t
    };

#endif // ULTRASONIC_SENSOR_H