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
#ifndef BASESENSOR_H
#define BASESENSOR_H

#include "Arduino.h"
#include <config.h>
#include "global.h"
#include "lego_uart.h"
#include "DeviceDescriptor.h"

extern volatile uint16_t changeMask; // used to track changes in sensor values for each mode

/**
 * @brief Handle basic functions for LegoUART protocol.
 *      Designed to be inherited in specific classes of sensors.
 *
 * @param m_connSerialRX_pin Serial RX pin of the board. (default: 0).
 * @param m_connSerialTX_pin Serial TX pin of the board. (default: 1).
 * @param m_rxBuf Buffer used to store bytes emitted by the hub.
 * @param m_txBug Buffer used to store bytes before being sent to the hub.
 * @param m_lastAckTick Time flag used to detect disconnection from the hub.
 * @param m_connected Connection flag.
 */
class BaseSensor {
public:
    BaseSensor(uint8_t rxPin = 0, uint8_t txPin = 1);
    // Define what a callback looks like: a function that takes a const void* (the updated variable)
    typedef void (*ModeCallback)();

    // virtual ~BaseSensor(){}
    void process();
    bool isConnected();
    void setSensorDataPointer(uint8_t modeId, void* pData);
    void setSensorCallback(uint8_t modeId, ModeCallback callback);

private:
    // Protocol handy functions
//    uint8_t getHeader(const uint8_t& msg_type, const uint8_t& mode, const uint8_t& msg_size);
    void parseHeader(const uint8_t& header, uint8_t& mode, uint8_t& msg_size);
    uint8_t getMsgSize(const uint8_t& header);

    inline void streamByte(uint8_t byte, uint8_t &runningChecksum) {
        runningChecksum ^= byte;
        SerialTTL.write(byte);
    }
    // Stream a block of bytes from any memory pointer
    void streamPayloadBytes(const uint8_t* ptr, uint8_t length, uint8_t &chk);

    // Single unified send function to replace sendUARTBuffer completely
    void sendPacket(uint8_t headerByte, const uint8_t* payloadPtr, uint8_t payloadSize);

    void commWaitForHubIdle();
    void connectToHub();
    void commSendInitSequence();
    void handleModes();

    // Process queries from/to hub
    void sendCurrentModeData();
    void receiveSensorModeData(uint8_t modeId, const uint8_t* incomingDataPayload);
    void sendHandshakeCommands();
    void sendModeInfoCommands(uint8_t modeId);
    void extendedModeInfoResponse();
    void ackResetCombosMode();
    void ackSetCombosMode();

    const uint8_t m_connSerialRX_pin;
    const uint8_t m_connSerialTX_pin;

    unsigned char m_rxBuf[3]; // Officially it should be 32 + 3 bytes, but I have never seen a message bigger than 19 bytes, so I will use this size to save memory
    unsigned long m_lastAckTick;

    bool m_connected;

    // UART protocol
    unsigned char m_mode = 0;
    uint8_t m_currentExtMode = 0;
    unsigned long m_lastDataSendTick = 0;
    bool m_defaultComboModesEnabled = false;

    // Array to hold pointers to data for each mode
    void* m_sensorDataPointers[CURRENT_SENSOR_MODE_COUNT] = {nullptr};
    // Array to hold callbacks for each mode (initialized to nullptr in constructor)
    ModeCallback m_sensorCallbacks[CURRENT_SENSOR_MODE_COUNT] = { nullptr };
};
#endif // BASESENSOR_H
