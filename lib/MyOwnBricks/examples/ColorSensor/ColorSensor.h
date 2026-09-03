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
#ifndef COLORSENSOR_H
#define COLORSENSOR_H

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
 * @brief Define modes of the custom sensor.
 *
 */
enum {   
        // Combinable modes: 0:Color, 1:Reflection, 5: RGB I, 6:HSV
        MODE_COMBO = 0,  // read 1x int8_t, 1x int8_t, 4x int16_t, 3x int16_t
        MODE_COLOR = 0,  // read 1x int8_t
        MODE_REFLT = 1,  // read 1x int8_t
        MODE_AMBI  = 2,  // read 1x int8_t
        MODE_LIGHT = 3,  // writ 3x int8_t
        MODE_RREFL = 4,  // read 2x int16_t
        MODE_RGB_I = 5,  // read 4x int16_t
        MODE_HSV   = 6,  // read 3x int16_t
        MODE_SHSV  = 7,  // read 4x int16_t
        MODE_DEBUG = 8,  // ??   4x int16_t
        MODE_CALIB = 9,  // ??   7x int16_t
};
#endif
