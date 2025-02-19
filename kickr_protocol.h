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
    void init();
    void startScan();
    bool isConnected() const { return deviceConnected; }
    bool connect(BLEAddress address);
    void disconnect();
    void changeGear(int8_t gearIndex);
    void getCurrentGear();
    void sendHello();
    void enableNotifications();
    static void commandCallback(BLERemoteCharacteristic *pCharacteristic, uint8_t *data, size_t length, bool isNotify);
    static void decode_riding_data(BLERemoteCharacteristic *pCharacteristic, uint8_t *data, size_t length, bool isNotify);
    static RidingData getCurrentRidingData() { return currentRidingData; }
    uint8_t getCurrentGearIndex() { return currentGear; }
    void shiftGear(int8_t numGears);
    void handleConnection(unsigned long currentTime);
    void handleGearStatus(unsigned long currentTime);
    void handleTasks(unsigned long currentTime);
    static void setupLed();
    static void setStatusLed(bool on);

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
    bool deviceConnected = false;
    bool doConnect = false;
    bool scan = true;
    bool triggerGearRatiosFetch = false;
    BLEAddress *pServerAddress = nullptr;
    unsigned long lastConnectionAttempt = 0;
    unsigned long lastGearChangeTime = 0;

    class AdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks
    {
        KickrProtocol &protocol;

    public:
        AdvertisedDeviceCallbacks(KickrProtocol &p) : protocol(p) {}
        void onResult(BLEAdvertisedDevice advertisedDevice) override;
    };

    friend class MyClientCallback;
};

class MyClientCallback : public BLEClientCallbacks
{
    KickrProtocol &protocol;

public:
    MyClientCallback(KickrProtocol &p) : protocol(p) {}
    void onConnect(BLEClient *pclient) override;
    void onDisconnect(BLEClient *pclient) override;
};
