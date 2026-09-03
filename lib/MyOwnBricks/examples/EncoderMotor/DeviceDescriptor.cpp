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
    // Device 0x30
    {
        .type = DeviceType::TECHNIC_MEDIUM_ANGULAR_MOTOR,
        .inModesMask = 0x001E, // 00011110
        .outModesMask = 0x001F, // 00011111
        .caps = 0x0F,
        .combos = {0x000E, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        // 0x000E = 0000 0000 0000 1110 = Modes 1, 2, 3 are input modes (Power, Speed, Position)
        // \x88\x06\x0E\x00\x7F  MESSAGE_INFO | LENGTH_2 | MODE_0, INFO_MODE_COMBOS, 0x000E, CHECKSUM
        .fwVersion = Version({
            .Major = 0,
            .Minor = 0,
            .Bugfix = 0,
            .Build = 4,
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
                "POWER", // COMBO?
                -100.0f, 100.0f,
                -100.0f, 100.0f,
                -100.0f, 100.0f,
                "PCT",
                0x00, 0x50,
                1, DATA8, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x30, 0x00, 0x00, 0x00, 0x05, 0x04 }}
            },
            {
                "SPEED",
                -100.0f, 100.0f,
                -100.0f, 100.0f,
                -100.0f, 100.0f,
                "PCT",
                0x30, 0x70,
                1, DATA8, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x21, 0x00, 0x00, 0x00, 0x05, 0x04 }}
            },
            {
                "POS",
                -360.0f, 360.0f,
                -100.0f, 100.0f,
                -360.0f, 360.0f,
                "DEG",
                0x28, 0x68,
                1, DATA32, 11, 0,
                {},
                0x00,
                Mode::Flags{{ 0x24, 0x00, 0x00, 0x00, 0x05, 0x04 }}
            },
            {
                "APOS",
                -180.0f, 179.0f,
                -200.0f, 200.0f,
                -180.0f, 179.0f,
                "DEG",
                0x32, 0x72,
                1, DATA16, 3, 0,
                {},
                0x00,
                Mode::Flags{{ 0x22, 0x00, 0x00, 0x00, 0x05, 0x04 }}
            },
            {
                "CALIB",
                0.0f, 3600.0f,
                0.0f, 100.0f,
                0.0f, 3600.0f,
                "CAL",
                0x00, 0x00,
                2, DATA16, 5, 0,
                {},
                0x00,
                Mode::Flags{{ 0x22, 0x40, 0x00, 0x00, 0x05, 0x04 }}
            },
            {
                "STATS",
                0.0f, 65535.0f,
                0.0f, 100.0f,
                0.0f, 65535.0f,
                "MIN",
                0x00, 0x00,
                14, DATA16, 5, 0,
                {},
                0x00,
                Mode::Flags{{ 0x00, 0x00, 0x00, 0x00, 0x05, 0x04 }}
            },
        }
    };
