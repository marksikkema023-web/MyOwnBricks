/*
 * MyOwnBricks is a library for the emulation of PoweredUp sensors on microcontrollers
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
#ifndef CUSTOMSENSOR_H
#define CUSTOMSENSOR_H

/**
 * @brief Define modes of the custom sensor.
 *
 */
enum {   
    MODE_COMBO = 0, // read int8_t, uint32_t, uint16_t
    MODE_POWER = 0, // read int8_t
    MODE_SPEED = 1, // read int8_t
    MODE_POS   = 2, // read int32_t
    MODE_APOS  = 3, // read int16_t
    MODE_CALIB = 4, // read int16_t x2
    MODE_STATS = 5, // read int16_t x14
};
#endif
