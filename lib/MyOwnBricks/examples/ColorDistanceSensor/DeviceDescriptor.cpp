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
        .type = DeviceType::BOOST_COLOR_DISTANCE_SENSOR,
        .inModesMask = 0x017F, // 0000000101111111 Is IrTx or COL O a view?
        .outModesMask = 0x01A0, // 0000000110100000
        .caps = 0x00, //?
        .combos = {0x004F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
        // 0x004F = 0000 0000 0100 1111 = modes 0, 1, 2, 3, 6 enabled (Color, Proximity, Count, Reflectance, RGB I)
        // \x88\x06\x4F\x00\x3E MESSAGE_INFO | LENGTH_2 | MODE_0, INFO_MODE_COMBOS, 0x004F, CHECKSUM
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
                0xC4, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "PROX",
                0.0f, 10.0f,
                0.0f, 100.0f,
                0.0f, 10.0f,
                "DIS",
                0x50, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "COUNT",
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "CNT",
                0x08, 0x00,
                1, DATA32, 4, 0,
                {},
                0x00
            },
            {
                "REFLT",
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "PCT",
                0x10, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "AMBI",
                0.0f, 100.0f,
                0.0f, 100.0f,
                0.0f, 100.0f,
                "PCT",
                0x10, 0x00,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "COL O",
                0.0f, 10.0f,
                0.0f, 100.0f,
                0.0f, 10.0f,
                "IDX",
                0x00, 0x04,
                1, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "RGB I",
                0.0f, 1023.0f,
                0.0f, 100.0f,
                0.0f, 1023.0f,
                "RAW",
                0x10, 0x00,
                3, DATA16, 5, 0,
                {},
                0x00
            },
            {
                "IR Tx",
                0.0f, 65535.0f,
                0.0f, 100.0f,
                0.0f, 65535.0f,
                "N/A",
                0x00, 0x04,
                1, DATA16, 5, 0,
                {},
                0x00
            },
            {
                "SPEC 1",
                0.0f, 255.0f,
                0.0f, 100.0f,
                0.0f, 255.0f,
                "N/A",
                0x00, 0x00,
                4, DATA8, 3, 0,
                {},
                0x00
            },
            {
                "DEBUG",
                0.0f, 1023.0f,
                0.0f, 100.0f,
                0.0f, 10.0f,
                "N/A",
                0x10, 0x00,
                2, DATA16, 5, 0,
                {},
                0x00
            },
            {
                "CALIB",
                0.0f, 65535.0f,
                0.0f, 100.0f,
                0.0f, 65535.0f,
                "N/A",
                0x10, 0x00,
                8, DATA16, 5, 0,
                {},
                0x00
            },
        }
    };
