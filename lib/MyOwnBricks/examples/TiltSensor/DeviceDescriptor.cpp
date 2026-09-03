/**
 *  Copyright (C) 2026 - Rbel12b
 * 
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *  */

#include <Arduino.h>
#include <DeviceDescriptor.h>

    const LPF2_DeviceProfile<CURRENT_SENSOR_MODE_COUNT> g_device = // PROGMEM for ProMicro
    // Device 0x25
    {
        .type = DeviceType::WEDO2_TILT_SENSOR,
        .inModesMask  = 0b0000'0000'0000'1111, // ???
        .outModesMask = 0b0000'0000'0000'0111, // ???
        .caps = 0x00,
        .combos = {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        .fwVersion = Version({
            .Major = 1,
            .Minor = 0,
            .Bugfix = 0,
            .Build = 0,
        }),
        .hwVersion = Version({
            .Major = 1,
            .Minor = 0,
            .Bugfix = 0,
            .Build = 0,
        }),
        .modes =
        {
            {
                "LPF2-ANGLE",
                -45.0f, 45.0f,
                -100.0f, 100.0f,
                -45.0f, 45.0f,
                "DEG",
                0x10, 0x00,
                2, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "LPF2-TILT",
                0.0f, 10.0f,
                0.0f, 100.0f,
                0.0f, 10.0f,
                "DIR",
                0x04, 0x00,
                1, DATA8, 2, 0,
                {},
                0x00
            },
            {
                "LPF2-CRASH",
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "CNT",
                0x10, 0x00,
                3, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "LPF2-CAL",
                -45.0f, 45.0f,
                -100.0f, 100.0f,
                -45.0f, 45.0f,
                "CAL",
                0x10, 0x00,
                3, DATA8, 3, 0,
                {},
                0x00
            }
        }
    };
