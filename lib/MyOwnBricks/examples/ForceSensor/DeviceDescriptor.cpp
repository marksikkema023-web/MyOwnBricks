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
    // Device 0x3F
    {
        .type = DeviceType::TECHNIC_FORCE_SENSOR,
        .inModesMask = 0b0000'0000'0011'1111,
        .outModesMask = 0b0000'0000'0000'0000,
        .caps = 0x00,
        .combos = {0x0013, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        // 0x0013 = 0000 0000 0001 0011 = modes 0, 1, 4, enabled (FORCE, TOUCHED, FRAW)
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
                "FORCE", // Mode 0
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "FOR",
                0x50, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "TOUCHED", // Mode 1
                0.0f, 1.0f,
                0.0f, 100.0f,
                0.0f, 1.0f,
                "IDX",
                0x04, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "TAPPED", // Mode 2
                0.0f, 3.0f,
                0.0f, 100.0f,
                0.0f, 3.0f,
                "IDX",
                0x04, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "", // Mode 3 Unknown?
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f,
                "",
                0x00, 0x00,
                0, DATA8, 0, 0,
                {},
                0x00
            },
            {
                "FRAW", // Mode 4
                0.0f, 1000.0f,
                0.0f, 100.0f,
                0.0f, 1000.0f,
                "RAW",
                0x50, 0x00, // Func mapping 2.0+?
                1, DATA16, 4, 0,
                {},
                0x00
            },
            {
                "", // Mode 5 Unknown?
                0.0f, 0.0f,
                0.0f, 0.0f,
                0.0f, 0.0f,
                "",
                0x00, 0x00,
                0, DATA8, 0, 0,
                {},
                0x00
            },
            {
                "CALIB", // Mode 6
                0.0f, 65535.0f,
                0.0f, 100.0f,
                0.0f, 65535.0f,
                "N/A",
                0x00, 0x00,
                8, DATA16, 4, 0,
                {},
                0x00
            }
        }
    };
