#include "kickr_protocol.h"

uint8_t KickrProtocol::currentGear = 1;
RidingData KickrProtocol::currentRidingData = {0, 0, 0.0f, 0, false};

bool KickrProtocol::connect(BLEAddress address)
{
    Serial.print("Attempting to connect to ");
    Serial.println(address.toString().c_str());

    if (pClient == nullptr)
    {
        Serial.println("Creating client...");
        pClient = BLEDevice::createClient();
        if (pClient == nullptr)
        {
            Serial.println("Failed to create client!");
            return false;
        }
    }

    Serial.println("Client created, attempting connection...");
    bool success = pClient->connect(address, BLE_ADDR_TYPE_RANDOM);
    Serial.println(success ? "Connection successful" : "Connection failed");

    if (!success)
    {
        pClient = nullptr; // Reset client on failure
        Serial.println("Connection failed");
        return false;
    }

    Serial.println("Connected to device");
    BLERemoteService *pRemoteService = pClient->getService(KICKR_SERVICE_UUID);
    if (pRemoteService == nullptr)
    {
        Serial.println("Failed to find KICKR service");
        return false;
    }

    Serial.println("Found KICKR service");
    pWriteCmdCharacteristic = pRemoteService->getCharacteristic(KICKR_WRITE_CMD_UUID);
    pReadCmdCharacteristic = pRemoteService->getCharacteristic(KICKR_READ_CMD_UUID);
    pReadDataCharacteristic = pRemoteService->getCharacteristic(KICKR_READ_DATA_UUID);

    if (pWriteCmdCharacteristic == nullptr || pReadCmdCharacteristic == nullptr || pReadDataCharacteristic == nullptr)
    {
        Serial.println("Failed to find characteristics");
        return false;
    }

    Serial.println("Found characteristics");
    enableNotifications();
    sendHello();
    getCurrentGear();

    extern bool deviceConnected;
    deviceConnected = true;
    return true;
}

void KickrProtocol::disconnect()
{
    if (pClient != nullptr)
    {
        pClient->disconnect();
    }
}

void KickrProtocol::changeGear(uint8_t gearIndex)
{
    if (gearIndex < 1 || gearIndex > MAX_GEAR)
        return;

    uint8_t buffer[32];
    size_t length = 0;
    createPhysicalParamMessage(buffer, length, GEAR_RATIOS[gearIndex - 1]);
    writeCommand(buffer, length);

    currentGear = gearIndex;
    Serial.print("Selected gear: ");
    Serial.println(currentGear);
}

void KickrProtocol::gearUp() {
    uint8_t nextGear = currentGear + 1;
    changeGear(nextGear);
}

void KickrProtocol::gearDown() {
    uint8_t nextGear = currentGear - 1;
    changeGear(nextGear);
}

void KickrProtocol::getCurrentGear()
{
    writeCommand(const_cast<uint8_t *>(MSG_DEVICE_INFO), sizeof(MSG_DEVICE_INFO));
}

void KickrProtocol::sendHello()
{
    writeCommand(const_cast<uint8_t *>(MSG_HELLO), sizeof(MSG_HELLO));
}

void KickrProtocol::writeCommand(uint8_t *data, size_t length)
{
    if (pWriteCmdCharacteristic != nullptr)
    {
#if DEBUG
        Serial.print("Writing command: ");
        for (size_t i = 0; i < length; i++)
        {
            Serial.printf("%02X ", data[i]);
        }
        Serial.println();
#endif
        pWriteCmdCharacteristic->writeValue(data, length, true); // Change to true to wait for response
    }
    else
    {
        Serial.println("Cannot write - characteristic not available");
    }
}

void KickrProtocol::createPhysicalParamMessage(uint8_t *buffer, size_t &length, uint16_t gearRatio)
{
    buffer[0] = 0x04; // HubCommand
    buffer[1] = 0x2A; // Field 5 (Physical), wire type 2
    buffer[3] = 0x10; // Field 2 (GearRatioX10000)

    // Encode gear ratio as varint
    size_t varintPos = 4;
    uint32_t value = gearRatio;
    while (value > 0x7F)
    {
        buffer[varintPos++] = (value & 0x7F) | 0x80;
        value >>= 7;
    }
    buffer[varintPos++] = value & 0x7F;

    // Set submessage length (varint bytes + tag byte)
    buffer[2] = (varintPos - 4) + 1;

    length = varintPos;
}

void KickrProtocol::enableNotifications()
{
    if (pReadCmdCharacteristic != nullptr)
    {
        if (pReadCmdCharacteristic->canIndicate())
        {
            pReadCmdCharacteristic->registerForNotify(commandCallback, false);
            Serial.println("CMD indications registered");
        }
        else
        {
            Serial.println("CMD characteristic doesn't support indications!");
        }
    }

    if (enableDataNotifications && pReadDataCharacteristic != nullptr)
    {
        if (pReadDataCharacteristic->canNotify())
        {
            pReadDataCharacteristic->registerForNotify(decode_riding_data, true);
            Serial.println("Data notifications registered");
        }
        else
        {
            Serial.println("Data characteristic doesn't support notifications!");
        }
    }
}

uint32_t KickrProtocol::decode_varint(const uint8_t *data, size_t length, size_t &pos)
{
    uint32_t result = 0;
    uint8_t shift = 0;

    while (pos < length && (data[pos] & 0x80))
    {
        result |= (data[pos] & 0x7F) << shift;
        shift += 7;
        pos++;
    }
    if (pos < length)
    {
        result |= (data[pos] & 0x7F) << shift;
        pos++;
    }
    return result;
}

void KickrProtocol::updateRidingData(uint8_t tag, uint32_t value)
{
    switch (tag)
    {
    case 0x08: // Power (field 1)
        currentRidingData.power = value;
        break;
    case 0x10: // Cadence (field 2)
        currentRidingData.cadence = value;
        break;
    case 0x18: // Speed (field 3)
        currentRidingData.speed = value / 100.0f;
        break;
    case 0x20: // Heart Rate (field 4)
        currentRidingData.heartRate = value;
        break;
    case 0x28: // Unknown1 (field 5)
        break;
    case 0x30:                            // Unknown2 (field 6)
        currentRidingData.isValid = true; // Set valid when we receive all fields
        break;
    }
}

void KickrProtocol::decode_riding_data(BLERemoteCharacteristic *pCharacteristic, uint8_t *data, size_t length, bool isNotify)
{
    if (length > 0 && data[0] == 0x03)
    {                                      // Check if it's a Riding Data message
        currentRidingData.isValid = false; // Reset validity
        size_t pos = 1;                    // Skip command byte

        while (pos < length)
        {
            uint8_t tag = data[pos++];
            uint32_t value = decode_varint(data, length, pos);
            updateRidingData(tag, value);
        }
    }
}

void KickrProtocol::commandCallback(BLERemoteCharacteristic *pCharacteristic, uint8_t *data, size_t length, bool isNotify)
{
#if DEBUG
    Serial.println("CMD Received data: ");
    for (size_t i = 0; i < length; i++)
    {
        Serial.printf("%02X ", data[i]);
    }
    Serial.println();
#endif

    // Check for hello response
    if (length == 8 &&
        data[0] == MSG_HELLO[0] && data[1] == MSG_HELLO[1] && data[2] == MSG_HELLO[2] && data[3] == MSG_HELLO[3] &&
        data[4] == MSG_HELLO[4] && data[5] == MSG_HELLO[5] && data[6] == 0x02 && data[7] == MSG_HELLO[7])
    {
        digitalWrite(LED_BUILTIN, HIGH); // Turn on LED on GPIO-8
        return;
    }

    if (length > 0 && data[0] == 0x3C)
    { // Device Information message
        uint8_t gear = parseGearFromDeviceInfo(data + 1, length - 1);
        if (gear > 0)
        {
            currentGear = gear; // Update current gear
            Serial.print("Current gear: ");
            Serial.println(gear);
        }
    }
}

uint8_t KickrProtocol::parseGearFromDeviceInfo(const uint8_t *data, size_t length)
{ // Added const
    for (size_t i = 0; i < length - 2; i++)
    {
        if (data[i] == 0x40)
        { // Found field 8
            uint16_t ratio = 0;
            size_t j = i + 1;
            uint8_t shift = 0;

            while (j < length && (data[j] & 0x80))
            { // LEB128 decoding
                ratio |= (data[j] & 0x7F) << shift;
                shift += 7;
                j++;
            }

            if (j < length)
            { // Add final byte
                ratio |= (data[j] & 0x7F) << shift;
            }

            // Find the gear index for this ratio
            for (uint8_t gear = 0; gear < MAX_GEAR; gear++)
            {
                if (GEAR_RATIOS[gear] == ratio)
                {
                    return gear + 1;
                }
            }
        }
    }
    return 0;
}

// Ensure only one definition of onResult exists
void KickrAdvertisedDeviceCallbacks::onResult(BLEAdvertisedDevice advertisedDevice)
{
    Serial.print("Device found: ");
    Serial.println(advertisedDevice.toString().c_str());
    if (advertisedDevice.getName() == DEVICE_NAME)
    {
        extern bool doConnect;
        extern BLEAddress *pServerAddress;
        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        advertisedDevice.getScan()->stop();
        doConnect = true;
    }
}
