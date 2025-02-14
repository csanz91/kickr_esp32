#pragma once
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include "kickr_constants.h"

struct RidingData
{
    uint32_t power;     // Watts
    uint32_t cadence;   // RPM
    float speed;        // km/h
    uint32_t heartRate; // BPM
    bool isValid;       // Indicates if the data is valid
};

class KickrProtocol
{
public:
    bool enableDataNotifications = false;
    bool connect(BLEAddress address);
    void disconnect();
    void changeGear(uint8_t gearIndex);
    void getCurrentGear();
    void sendHello();
    void enableNotifications();
    static void commandCallback(BLERemoteCharacteristic *pCharacteristic, uint8_t *data, size_t length, bool isNotify);
    static void decode_riding_data(BLERemoteCharacteristic *pCharacteristic, uint8_t *data, size_t length, bool isNotify);
    static RidingData getCurrentRidingData() { return currentRidingData; }
    uint8_t getCurrentGearIndex() { return currentGear; }
    void gearUp();
    void gearDown();

private:
    BLEClient *pClient = nullptr;
    BLERemoteCharacteristic *pWriteCmdCharacteristic = nullptr;
    BLERemoteCharacteristic *pReadCmdCharacteristic = nullptr;
    BLERemoteCharacteristic *pReadDataCharacteristic = nullptr;
    void writeCommand(uint8_t *data, size_t length);
    void createPhysicalParamMessage(uint8_t *buffer, size_t &length, uint16_t gearRatio);
    static uint8_t parseGearFromDeviceInfo(const uint8_t *data, size_t length);
    static uint32_t decode_varint(const uint8_t *data, size_t length, size_t &pos);
    static void updateRidingData(uint8_t tag, uint32_t value);
    static RidingData currentRidingData;
    static uint8_t currentGear;
};

class KickrAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
{
    void onResult(BLEAdvertisedDevice advertisedDevice) override;
};
