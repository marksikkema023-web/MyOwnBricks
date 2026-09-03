/*
 * A library for the emulation of PoweredUp sensors on microcontrollers
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
#include "BaseSensor.h"

/**
 * @brief Default constructor
 */
BaseSensor::BaseSensor(uint8_t rxPin, uint8_t txPin) :
    m_connSerialRX_pin(rxPin),
    m_connSerialTX_pin(txPin),
    
    m_lastAckTick(0),
    m_connected(false),

    m_mode(0),
    m_currentExtMode(0),
    m_lastDataSendTick(0),
    m_defaultComboModesEnabled(false)
{}

/**
 * @brief Get status of connection with the hub.
 * @return bool
 */
bool BaseSensor::isConnected(){
    return m_connected;
}

/**
 * @brief
 * @param pData
 *      TODO:
 *      Continuous values
 */
void BaseSensor::setSensorDataPointer(uint8_t modeId, void* pData) {
    if (modeId < CURRENT_SENSOR_MODE_COUNT) {
        this->m_sensorDataPointers[modeId] = pData;
    }
}

/**
 * @brief Set callback receiving data for a specific mode.
 * @param modeId The mode ID for which the callback is set.
 * @param callback The function to be called when data for the specified mode is received.
 *      The callback function should have the signature: void callbackFunction().
 *      It will be called when data for the specified mode is received.
 *      The callback function can access the updated data through the pointers set with setSensorDataPointer.
 */
void BaseSensor::setSensorCallback(uint8_t modeId, ModeCallback callback) {
    if (modeId < CURRENT_SENSOR_MODE_COUNT) {
        m_sensorCallbacks[modeId] = callback;
    }
}

void BaseSensor::sendHandshakeCommands() {
    uint8_t checksum;

    // 1. MESSAGE_CMD (0x40) -> Type ID Connection
    checksum = 0xFF; // make this a writeChecksum() to save memory?
    streamByte(LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_1 | LUMP_CMD_TYPE, checksum);
    streamByte(static_cast<uint8_t>(g_device.type), checksum); // Dynamically pulls 0x3E
    SerialTTL.write(checksum);
    
    delay(10); 

    // Calculate mode/view metrics from the descriptor data
    uint8_t totalModes = CURRENT_SENSOR_MODE_COUNT;
    uint8_t inputModes = 0;
    
    // Quick bit-popcount on the masks to find input/output count dynamically
    for (uint8_t i = 0; i < 16; i++) 
        if ((g_device.inModesMask >> i) & 1) inputModes++;
    
    // 2. LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_4 | CMD_MODES (0x51)
    checksum = 0xFF;
    streamByte(LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_4 | LUMP_CMD_MODES, checksum);
    streamByte((totalModes - 1 > 7) ? 7 : (totalModes - 1), checksum);
    streamByte((inputModes - 1 > 7) ? 7 : (inputModes - 1), checksum);
    streamByte(totalModes - 1, checksum);
    streamByte(inputModes - 1, checksum);
    SerialTTL.write(checksum);  

    delay(10);

    // 3. LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_4 | CMD_SPEED (0x52)
    checksum = 0xFF;
    streamByte(LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_4 | LUMP_CMD_SPEED, checksum);
    streamByte(0x00, checksum); // Low byte of 115200 (0x00)
    streamByte(0xC2, checksum); // Mid-low byte of 115200 (0xC2)
    streamByte(0x01, checksum); // Mid-high byte of 115200 (0x01) <-- FIXED: Hardcoded to 0x01
    streamByte(0x00, checksum); // High byte of 115200 (0x00)
    SerialTTL.write(checksum);  

    delay(10);

    // 4. LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_8 | CMD_VERSION (0x5F)
    checksum = 0xFF;
    streamByte(LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_8 | LUMP_CMD_VERSION, checksum);
    // Stream FW Version directly from struct
    streamByte(g_device.fwVersion.Build  << 4, checksum); 
    streamByte(g_device.fwVersion.Bugfix << 4, checksum);
    streamByte(g_device.fwVersion.Minor  << 4, checksum);
    streamByte(g_device.fwVersion.Major  << 4, checksum);
    // Stream HW Version directly from struct
    streamByte(g_device.hwVersion.Build  << 4, checksum);
    streamByte(g_device.hwVersion.Bugfix << 4, checksum);
    streamByte(g_device.hwVersion.Minor  << 4, checksum);
    streamByte(g_device.hwVersion.Major  << 4, checksum);

    SerialTTL.write(checksum);  
    SerialTTL.flush();
    delay(10);
}

/**
 * @brief Send modeminformation
 *      TODO: Find out if empty modes should be skipped?
 *            e.g. The force sensor skips mode 3 and 5
*/
void BaseSensor::sendModeInfoCommands(uint8_t modeId) {
    const Mode &mode = g_device.modes[modeId];
    uint8_t checksum;
    uint8_t header;

    // For INFO payload type byte, modes >= 8 use +0x20 (INFO_MODE_PLUS_8).
    uint8_t typeOffset = (modeId > 7) ? 0x20 : 0x00;

/*    auto streamPayloadBytes = [&](const uint8_t* ptr, uint8_t length, uint8_t &chk) {
        for (uint8_t i = 0; i < length; i++) {
            streamByte(ptr[i], chk);
        }
    };*/

    // ==========================================
    // 1. INFO_NAME (Type 0x20) -> 6 chars + 6 flags + 4 reserved bytes
    // ==========================================
    header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_16 | (modeId & LUMP_MSG_CMD_MASK);
    checksum = 0xFF;
    streamByte(header, checksum);
    streamByte(LUMP_INFO_NAME | typeOffset, checksum); 
#if defined(BUILD_LEGACY_SENSOR)
    streamPayloadBytes((const uint8_t*)mode.name, 12, checksum);
#elif defined(BUILD_MODERN_SENSOR)
    streamPayloadBytes((const uint8_t*)mode.name, 6, checksum);
    streamPayloadBytes(mode.flags.data, 6, checksum);
#endif
    streamByte(0x00, checksum);
    streamByte(0x00, checksum);
    streamByte(0x00, checksum);
    streamByte(0x00, checksum);
    SerialTTL.write(checksum);
    delay(10);

    // ==========================================
    // 2. INFO_RAW (Type 0x21) -> 2 Floats (8 Bytes)
    // ==========================================
    header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_8 | (modeId & LUMP_MSG_CMD_MASK);
    checksum = 0xFF;
    streamByte(header, checksum);
    streamByte(LUMP_INFO_RAW | typeOffset, checksum);
    streamPayloadBytes((const uint8_t*)&mode.rangeMin, 4, checksum);
    streamPayloadBytes((const uint8_t*)&mode.rangeMax, 4, checksum);
    SerialTTL.write(checksum);
    delay(10);

    // ==========================================
    // 2. INFO_PCT 
    // ==========================================
    header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_8 | (modeId & LUMP_MSG_CMD_MASK);
    checksum = 0xFF;
    streamByte(header, checksum);
    streamByte(LUMP_INFO_PCT | typeOffset, checksum);
    streamPayloadBytes((const uint8_t*)&mode.pctMin, 4, checksum);
    streamPayloadBytes((const uint8_t*)&mode.pctMax, 4, checksum);
    SerialTTL.write(checksum);
    delay(10);

    // ==========================================
    // 2. INFO_SI 
    // ==========================================
    header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_8 | (modeId & LUMP_MSG_CMD_MASK);
    checksum = 0xFF;
    streamByte(header, checksum);
    streamByte(LUMP_INFO_SI | typeOffset, checksum);
    streamPayloadBytes((const uint8_t*)&mode.siMin, 4, checksum);
    streamPayloadBytes((const uint8_t*)&mode.siMax, 4, checksum);
    SerialTTL.write(checksum);
    delay(10);

    // ==========================================
    // 3. INFO_UNITS (Type 0x24) -> 4 Bytes
    // ==========================================
    header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_4 | (modeId & LUMP_MSG_CMD_MASK);
    checksum = 0xFF;
    streamByte(header, checksum);
    streamByte(LUMP_INFO_UNITS | typeOffset, checksum);
    streamPayloadBytes((const uint8_t*)mode.unit, 4, checksum);
    SerialTTL.write(checksum);
    delay(10);

    // ==========================================
    // 4. INFO_MAPPING (Type 0x25) -> 2 Bytes
    // ==========================================
    header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_2 | (modeId & LUMP_MSG_CMD_MASK);
    checksum = 0xFF;
    streamByte(header, checksum);
    streamByte(LUMP_INFO_MAPPING | typeOffset, checksum);
    streamByte(mode.mappingIn, checksum);
    streamByte(mode.mappingOut, checksum);
    SerialTTL.write(checksum);
    delay(10);

    // ==========================================
    // 5. INFO_FORMAT (Type 0xA0) -> 4 Bytes
    // ==========================================
    header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_4 | (modeId & LUMP_MSG_CMD_MASK);
    checksum = 0xFF;
    streamByte(header, checksum);
    streamByte(LUMP_INFO_FORMAT | typeOffset, checksum);
    streamByte(mode.sampleCount, checksum);
    streamByte(static_cast<uint8_t>(mode.dataType), checksum);
    streamByte(mode.digits, checksum);
    streamByte(mode.decimals, checksum);
    SerialTTL.write(checksum);
    SerialTTL.flush();
    delay(10);

    // ==========================================
    // 6. INFO_MODE_COMBOS (Type 0xFF) -> 2 Bytes, Only when .combos[0] > 0x0000 and modeId == 0?
    // ==========================================
    if (g_device.combos[0] > 0x0000 && modeId == 0) {
        header = LUMP_MSG_TYPE_INFO | LUMP_MSG_SIZE_2 | (modeId & LUMP_MSG_CMD_MASK);
        checksum = 0xFF;
        streamByte(header, checksum);
        streamByte(LUMP_INFO_MODE_COMBOS, checksum);
        streamByte( g_device.combos[0] & 0xFF, checksum);
        streamByte((g_device.combos[0] >> 8) & 0xFF, checksum);
        SerialTTL.write(checksum);
        SerialTTL.flush();
        delay(10);
    }
}

/**
 * @brief Send initialization sequences for the current sensor.
 *      TODO: Find out why regularly the first ACK isn't caught.
 */
void BaseSensor::commSendInitSequence() {
    SerialTTL.begin(2400);
    sendHandshakeCommands();
    const uint8_t totalModes = CURRENT_SENSOR_MODE_COUNT;
    for (int8_t m = static_cast<int8_t>(totalModes) - 1; m >= 0; m--) {
        sendModeInfoCommands(m);
    }
    SerialTTL.write(0x04);   // ACK
    delay(10);
}

/**
 * @brief Handle the protocol queries & responses from/to the hub.
 *      Queries can be read/write according to the requested mode.
 * @warning In the situation where the processing of the responses to the
 *      queries from the hub takes longer than 200ms, a disconnection
 *      will be performed here.
 */
void BaseSensor::handleModes(){
    if (changeMask & (1 << m_mode)) { // Any change in sensor values
        const unsigned long now = millis();
        if (now - m_lastDataSendTick >= 5) {
            m_lastDataSendTick = now;
            this->sendCurrentModeData();
            changeMask &= ~(1 << m_mode);
            DEBUG_PRINTLN("values changed, sending data to hub");
        }
    }
    if (SerialTTL.available() == 0)
        return;
    unsigned char header = SerialTTL.read();
    unsigned char write_mode;
    unsigned char ExtMode;
    if (header == 0x02) { // NACK
        m_lastAckTick = millis();
        // Here we can send mode 0 or mode 8 according to the value of ExtMode
        // And send extendedModeInfoResponse before any data response.
        // Note: In theory the default mode is always the lowest (0).
        // If combos mode is enabled, prefer to send this data
        this->extendedModeInfoResponse();
        this->sendCurrentModeData();
    }
    if (header == 0x43) { // CMD_SELECT, select mode
        // "Get value" commands (3 bytes message: header, mode, checksum)
        size_t ret = SerialTTL.readBytes(m_rxBuf, 2);
        if (ret < 2) {
            // check if all expected bytes are received without timeout
            DEBUG_PRINT(F("incomplete 0x43 message"));
            return;
        }
        m_mode = m_rxBuf[0];
        DEBUG_PRINT(F("<\tAsked mode "));
        DEBUG_PRINTLN(m_mode);
        this->m_currentExtMode = (m_mode < 8) ? LUMP_EXT_MODE_0 : LUMP_EXT_MODE_8;
    } else if (header == 0x46) {
        // "Set value" commands
        // The message has 2 parts (each with header, value and checksum):
        // - The EXT_MODE status as value
        // - The LUMP_MSG_TYPE_DATA itself with its data as value

        // Get data1, checksum1, header2 (header of the next message)
        size_t ret = SerialTTL.readBytes(m_rxBuf, 3);
        if (ret < 3)
            // check if all expected bytes are received without timeout
            return;

        ExtMode = m_rxBuf[0];

        // Get mode and size of the message from the header
        uint8_t msg_size;
        parseHeader(m_rxBuf[2], write_mode, msg_size);
        // TODO: avoid buffer overflow: check msg size <= size rx buffer

        write_mode += ExtMode;

        // Fetch the memory pointer for this mode
        const Mode &mode = g_device.modes[write_mode];
        void* rawPtr = m_sensorDataPointers[write_mode];

        // Calculate how many bytes we need to copy
        uint8_t bytesPerSample = (1 << mode.dataType);
        uint8_t totalDataBytes = mode.sampleCount * bytesPerSample;

        // Read the remaining bytes after the header (cheksum included)
        // Data will be in the indexes [0;msg_size-2]
        if (rawPtr != nullptr && (msg_size - 1) == totalDataBytes && totalDataBytes > 0) {
            // Read directly into the sensor's struct/variable memory address!
            ret = SerialTTL.readBytes(static_cast<uint8_t*>(rawPtr), msg_size - 1);

            // If UART timed out or dropped bytes mid-stream, abort
            if (ret != totalDataBytes) {
                return; 
            }
        } else {
            // Length mismatch or nullptr — clear/flush unread packet bytes from UART buffer
            return;
        }
        DEBUG_PRINT(F("header == 0x46 Callback for mode: "));
        DEBUG_PRINTLN(write_mode);
        DEBUG_PRINT(F("After we go again in mode: "));
        DEBUG_PRINTLN(m_mode);
        if (m_sensorCallbacks[write_mode] != nullptr) {
            m_sensorCallbacks[write_mode](); // callback 
        }
    } else if (header == 0x4C) { // 01001100 Disable combo mode, 0x4C = MESSAGE_CMD | LENGTH_2 | CMD_WRITE
    // 0x4C 0x20 0x00 0x93?
    // MESSAGE_CMD | LENGTH_2 | CMD_WRITE
    // 0x23 (write payload?) | 0x00 (0 pairs?)
    // I think encoder motor is the only sensor using Combo mode in PUP app
        DEBUG_PRINTLN(F("Disable combo mode request received (0x4C message)"));

        // Reset the Combination modes (supposed to)
        // Currently (03/2022) the packet is the following:
        // { 4C 20 00 93 }
        // Note: There is no parsing of the message, we just check the checksum
        // and discard the message if it doesn't match.

        // Get data (4 bytes message)
        size_t ret = SerialTTL.readBytes(m_rxBuf, 3);
        if (ret < 3) {
            // check if all expected bytes are received without timeout
            DEBUG_PRINTLN(F("incomplete 0x4C message"));
            return;
        }

        if (m_rxBuf[2] != 0x93) { // checksum check
            // Structure not expected
            DEBUG_PRINTLN(F("Structure not expected"));
            return;
        }

        this->m_defaultComboModesEnabled = false;
        // Send acknowledgement
        this->ackResetCombosMode();

    } else if (header == 0x5C) { // 01011100 Enable combo mode, 0x5C = MESSAGE_CMD | LENGTH_8 | CMD_WRITE
    // 0x5C 0x23 0x00 0x10 0x20 0x30 0x00 0x00 0x00
    // MESSAGE_CMD | LENGTH_8 | CMD_WRITE
    // 0x23 (write payload?) | 0x03 (3 pairs?)
    // 0x00 (Combo index?) does it refer to mode 0?
    // 0x10, 0x20, 0x30: The 3 separate mode/dataset values being written. Mode 1, 2 & 3?
    // 0x00, 0x00, 0x00: Padding to fill the 8 byte payload
    // 0x2C? checksum
        DEBUG_PRINTLN(F("Combo mode request received (0x5C message)"));

        // Receive a combination modes query to define the default data to send after each NACK
        // Currently (03/2022) the packet is the following:
        // { 5C 23 00 10 20 30 00 00 00 ?? }
        // Note: There is no parsing of the message, we just check the checksum
        // and discard the message if it doesn't match.

        this->m_defaultComboModesEnabled = true;

        SerialTTL.write(header);
        for (uint8_t i = 0; i < 9; i++) {
            uint32_t startTime = millis();
    
            // Wait for next byte, timing out after 100ms
            while (!SerialTTL.available()) {
                if (millis() - startTime > 100) { // 100ms timeout
                    DEBUG_PRINTLN(F("incomplete 0x5C message (timeout)"));
                    return;
                }
            }

            // Read single byte and stream it right back out
            SerialTTL.write(static_cast<uint8_t>(SerialTTL.read()));
        }
        SerialTTL.flush();
    }
}

void BaseSensor::sendCurrentModeData() {
    // 1. Get the current mode metadata
    const Mode &mode = g_device.modes[m_mode];
    
    // 2. Fetch the memory pointer for this mode
    void* rawPtr = m_sensorDataPointers[m_mode];
    if (rawPtr == nullptr) return; // Safety check: pointer not set!

    uint8_t totalDataBytes = 0; // ???

// --- COMBO MODE HARDCODED INTERCEPT FOR ENCODER MOTORS ---
// This is a temporary solution to send a fixed 8-byte data structure for the encoder motor combo mode.
// When other combo enabled sensors come up, we will understand better how to handle this in a more generic way.
// If the powered up app has no other sensors with combo mode enabled
// We will need to test on PyBricks
    if (m_defaultComboModesEnabled) { // && m_mode == 0) {
        // Force header to MESSAGE_DATA (0xC0) | LENGTH_8 (0x18) | Mode 0 (0x00) = 0xD8
        sendPacket(0xD8, static_cast<const uint8_t*>(rawPtr), 8);
    }
    else {
        // 3. Calculate payload sizes
        uint8_t bytesPerSample = (1 << mode.dataType);
        totalDataBytes = mode.sampleCount * bytesPerSample;

        // 4. Construct the dynamic LPF2 header
        // Determine LENGTH_x based on totalDataBytes (1->0x00, 2->0x08, 4->0x10, 8->0x18...)
        uint8_t lengthMask = 0;
        uint8_t size = totalDataBytes;
        while (size > 1) {
            size >>= 1;
            lengthMask += 8;
        }

        // Mask mode by 7 so modes 8+ don't corrupt the length bits
        uint8_t maskedMode = m_mode & 0x07;

        sendPacket(LUMP_MSG_TYPE_DATA | lengthMask | maskedMode, static_cast<const uint8_t*>(rawPtr), totalDataBytes);
    }
}

void BaseSensor::receiveSensorModeData(uint8_t modeId, const uint8_t* incomingDataPayload) {
}

/**
 * @brief Send EXT_MODE status to the hub: extended mode info message
 *      Should be used as a first response after a NACK, and before every responses for
 *      modes >= 8.
 */
void BaseSensor::extendedModeInfoResponse(){
    // extended mode info
    sendPacket(LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_1 | LUMP_CMD_EXT_MODE, &this->m_currentExtMode, 1);
}

/**
 * @brief Response to a reset combo mode query.
 */
void BaseSensor::ackResetCombosMode(){
    const uint8_t payload = 0x00;
    sendPacket(LUMP_MSG_TYPE_CMD | LUMP_MSG_SIZE_1 | LUMP_CMD_WRITE, &payload, 1);
}

/**
 * @brief Response to a combo mode query.
 *      It's the same package as the one received.
 * @todo Replay the received packet instead manually setting it.
 */

/**
 * @brief Get checksum for the given message
 * @param pData Message array: Header + Payload
 * @param length Length of the payload (size WITHOUT header & checksum)
 * @return Checksum byte
 */


/**
 * @brief Wait until the hub is available on the serial RX line.
 *      Then, assert the TX line.
 */
void BaseSensor::commWaitForHubIdle(){
    // Disable uart: manual control TX and RX pins
    SerialTTL.end();

    unsigned long idletick;

    pinMode(m_connSerialTX_pin, OUTPUT);
    digitalWrite(m_connSerialTX_pin, LOW);

    pinMode(m_connSerialRX_pin, INPUT);
    idletick = millis();
    // The hub waits for the device to go low for 400ms
    // The hub will then keep high, once high for 100ms...
    // our device will go high for 100ms and low for 100ms
    // and start init sequence
    while (1) { 
        if (digitalRead(m_connSerialRX_pin) == LOW) {
            idletick = millis();
        }
        if (millis() - idletick > 100) {
            break;
        }
    }

    digitalWrite(m_connSerialTX_pin, HIGH);
    delay(100);
    digitalWrite(m_connSerialTX_pin, LOW);
    delay(100);
}


/**
 * @brief Handle initialization of a connection to the hub.
 *      Workflow:
 *          - Wait RX line deasserted by the hub to LOW
 *          - Wait 100 ms
 *          - Assert TX line briefly
 *          - Start UART connection at 2400 bauds
 *          - Send sensor init sequence
 *          - Send ACK (0x04)
 *          - Wait ACK during 2s
 *          - Start UART connection at 115200 bauds
 */
void BaseSensor::connectToHub() {
    DEBUG_PRINTLN("INIT SENSOR");

    // Wait for HUB to idle it's TX pin (idle = High)
    // TODO: These modifications emit b'\x00\x00' before anything else on the serial line!!
    commWaitForHubIdle();
    DEBUG_PRINTLN("StartingInitSequence");

    // Starting initialization sequence
    commSendInitSequence();
    unsigned long starttime = millis();

    // Check if the hub send a ACK
    unsigned long currenttime = starttime;
    while ((currenttime - starttime) < 2000) {
        if (SerialTTL.available() > 0) {
            // read the incoming byte
            unsigned char dat = SerialTTL.read();
            if (dat == 0x04) { // ACK
                //DEBUG_PRINTLN("Connection established !");
                SerialTTL.begin(115200);
                m_connected   = true;
                m_lastAckTick = millis();
                break;
            }
        }
        currenttime = millis();

        SerialTTL.flush();
//        delay(10); // Regularly, the device doesnt register the first mode select after changing baudrate
    }
}

/**
 * @brief Handle the connection process to the hub.
 * @see Protocol queries & responses are processed by `handleModes()`.
 * @warning Do not forget to check at each iteration if `millis() - m_lastAckTick > 200`.
 *      If true, the LPF2_device must go in reset mode by setting the m_connected
 *      boolean to false.
 */
void BaseSensor::process(){
    if(!m_connected){
        this->connectToHub();
        return;
    }

    // Connection established
    handleModes();

    // Check disconnection from the Hub and go in reset/init mode if needed
    if (millis() - m_lastAckTick > 200) {
        INFO_PRINT(F("Disconnect; Too much time since last NACK - "));
        INFO_PRINTLN(millis() - m_lastAckTick);
        m_connected = false;
    }
}

/**
 * @brief Get mode and message size from the given header.
 *      Currently used to parse 2nd part of write queries
 *      (1st part has the known header 0x46).
 * @param header Header of a received/transmitted message.
 * @param mode Reference See the class enumeration of modes.
 * @param msg_size Reference to message size.
 */
void BaseSensor::parseHeader(const uint8_t& header, uint8_t& mode, uint8_t& msg_size){
    // Type is known to be MESSAGE_DATA because of 0x46 header
    // msg_type = header & TYPE_MASK;
    mode     = header & LUMP_MSG_CMD_MASK;
    msg_size = getMsgSize(header);
}


/**
 * @brief Get size of a message from the given header. Used by parseHeader().
 *
 * @warning /!\ This is a simplified version that works on LUMP_MSG_TYPE_DATA messages.
 *      DO NOT use on MESSAGE_SYS, MESSAGE_INFO messages.
 * @param header
 * @return Expected size
 */
uint8_t BaseSensor::getMsgSize(const uint8_t& header){
    // Simplified version that implicitly asserts that msg_type is LUMP_MSG_TYPE_DATA
    return (uint8_t) ((1 << (((header) >> 3) & 0x7)) + 2);
}


/**
 * @brief Send the TX buffer content to the hub
 *      Also add the checksum of the message.
 * @param msg_size Size of the message WITHOUT header & checksum: Payload size.
 */

void BaseSensor::streamPayloadBytes(const uint8_t* ptr, uint8_t length, uint8_t &chk) {
//    if (ptr == nullptr) return;
    for (uint8_t i = 0; i < length; i++) {
        streamByte(ptr[i], chk);
    }
}

void BaseSensor::sendPacket(uint8_t headerByte, const uint8_t* payloadPtr, uint8_t payloadSize) {
    uint8_t chk = 0xFF; // LPF2 checksum initial seed

    // 1. Stream Header
    streamByte(headerByte, chk);

    // 2. Stream Payload (using our new helper)
    streamPayloadBytes(payloadPtr, payloadSize, chk);

    // 3. Stream Checksum
    SerialTTL.write(chk);
    SerialTTL.flush();
}