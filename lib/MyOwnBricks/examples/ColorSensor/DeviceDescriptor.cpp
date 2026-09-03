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
#include "DeviceDescriptor.h"

    const LPF2_DeviceProfile<CURRENT_SENSOR_MODE_COUNT> g_device = // PROGMEM for ProMicro
    // Device 0x3D
    {
        .type = DeviceType::TECHNIC_COLOR_SENSOR,
        .inModesMask  = 0b0000'0011'1111'1111,
        .outModesMask = 0b0000'0000'0000'1000,
        .caps = 0x00,
        .combos = {0x0063, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
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
                "COLOR",
                0.0f, 10.0f,
                0.0f, 100.0f,
                0.0f, 10.0f,
                "IDX",
                0xE4, 0x00,
                1, DATA8, 2, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "REFLT",
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "PCT",
                0x30, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "AMBI",
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "PCT",
                0x30, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00,
                Mode::Flags{{ 0x00, 0x40, 0x00, 0x00, 0x00, 0x04 }}
            },
            {
                "LIGHT",
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "PCT",
                0x00, 0x10,
                3, DATA8, 3, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x05, 0x04 }}
            },
            {
                "RREFL",
                0.0f, 1024.0f,
                0.0f, 100.0f,
                0.0f, 1024.0f,
                "RAW",
                0x10, 0x00,
                2, DATA16, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "RGB I",
                0.0f, 1024.0f,
                0.0f, 100.0f,
                0.0f, 1024.0f,
                "RAW",
                0x10, 0x00,
                4, DATA16, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "HSV",
                0.0f, 360.0f,
                0.0f, 100.0f,
                0.0f, 360.0f,
                "RAW",
                0x10, 0x00,
                3, DATA16, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x00, 0x00, 0x40, 0x00, 0x00, 0x00 }}
            },
            {
                "SHSV",
                0.0f, 360.0f,
                0.0f, 100.0f,
                0.0f, 360.0f,
                "RAW",
                0x10, 0x00,
                4, DATA16, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x00, 0x40, 0x00, 0x00, 0x00, 0x04 }}
            },
            {
                "DEBUG",
                0.0f, 65535.0f,
                0.0f, 100.0f,
                0.0f, 65535.0f,
                "RAW",
                0x10, 0x00,
                4, DATA16, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "CALIB",
                0.0f, 65535.0f,
                0.0f, 100.0f,
                0.0f, 65535.0f,
                "",
                0x00, 0x00,
                7, DATA16, 5, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x40, 0x00, 0x00, 0x04, 0x84 }}
            },
        }
    };
