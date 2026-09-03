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
#include "config.h"

    const LPF2_DeviceProfile<CURRENT_SENSOR_MODE_COUNT> g_device = // PROGMEM for ProMicro
    // Device 0x3E
    {
        .type = DeviceType::TECHNIC_DISTANCE_SENSOR,
        .inModesMask  = 0b0000'0000'1011'1111, // 7 input modes, Mode 5 is view?
        .outModesMask = 0b0000'0000'0110'0000, // 2 output modes
        .caps = 0x03, // capability flags: 0x01 = supports extended mode, 0x02 = supports combo modes???
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
                "DISTL",
                0.0f, 2500.0f,
                0.0f, 100.0f,
                0.0f, 250.0f,
                "CM",
                0x91, 0x00,
                1, DATA16, 5, 1,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x40, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "DISTS",
                0.0f, 320.0f,
                0.0f, 100.0f,
                0.0f, 32.0f,
                "CM",
                0xF1, 0x00,
                1, DATA16, 4, 1,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "SINGL",
                0.0f, 2500.0f,
                0.0f, 100.0f,
                0.0f, 250.0f,
                "CM",
                0x90, 0x00,
                1, DATA16, 5, 1,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "LISTN",
                0.0f, 1.0f,
                0.0f, 100.0f,
                0.0f, 1.0f,
                "ST",
                0x10, 0x00,
                1, DATA8, 1, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "TRAW",
                0.0f, 14577.0f,
                0.0f, 100.0f,
                0.0f, 14577.0f,
                "uS",
                0x90, 0x00,
                1, DATA32, 5, 0,
                {},
                0x00,
                Mode::Flags{{ 0x00, 0x40, 0x00, 0x00, 0x00, 0x04 }}
            },
            {
                "LIGHT",
                0.0f, 100.0f, // I have tried changing the values to 255
                0.0f, 100.0f, // But PUP app still sends max 100 (0x64)
                0.0f, 100.0f,
                "PCT",
                0x00, 0x10,
                4, DATA8, 3, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x20, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "PING",
                0.0f, 1.0f,
                0.0f, 100.0f,
                0.0f, 1.0f,
                "PCT",
                0x00, 0x90,
                1, DATA8, 1, 0,
                {},
                0x00,
                Mode::Flags{{ 0x00, 0x40, 0x80, 0x00, 0x00, 0x04 }}
            },
            {
                "ADRAW",
                0.0f, 1024.0f,
                0.0f, 100.0f,
                0.0f, 1024.0f,
                "PCT",
                0x90, 0x00,
                1, DATA16, 4, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x00, 0x00, 0x00, 0x04, 0x84 }}
            },
            {
                "CALIB",
                0.0f, 255.0f,
                0.0f, 100.0f,
                0.0f, 255.0f,
                "PCT",
                0x00, 0x00,
                7, DATA8, 3, 0,
                {},
                0x00,
                Mode::Flags{{ 0x40, 0x40, 0x00, 0x00, 0x04, 0x84 }}
            },
        }
    };
