#include <BLEDevice.h>
#include "kickr_protocol.h"
#include "kickr_constants.h"

KickrProtocol kickr;
bool deviceConnected = false;
bool doConnect = false;
BLEAddress* pServerAddress = nullptr;

class ClientCallback : public BLEClientCallbacks {
    void onConnect(BLEClient* pclient) override {
        deviceConnected = true;
        Serial.println("Connected");
    }

    void onDisconnect(BLEClient* pclient) override {
        deviceConnected = false;
        Serial.println("Disconnected");
    }
};

void setupBLEDevice() {
    BLEDevice::init(CLIENT_NAME);
    auto* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new KickrAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30, false);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(115200);
    setupBLEDevice();
    
    auto* pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new ClientCallback());
}

void loop() {
    static unsigned long lastChangeTime = 0;
    
    // Attempt to connect if requested
    if (doConnect) {
        // kickr.enableDataNotifications = true;
        if (kickr.connect(*pServerAddress)) {
            Serial.println("Successfully connected to KICKR.");
        } else {
            Serial.println("Failed to connect to KICKR.");
        }
        doConnect = false;
    }

    // Control the KICKR if connected
    if (deviceConnected) {
        // Change gear every GEAR_CHANGE_INTERVAL milliseconds
        unsigned long currentTime = millis();
        if (currentTime - lastChangeTime >= GEAR_CHANGE_INTERVAL) {
            uint8_t nextGear = (kickr.getCurrentGearIndex() % 24) + 1;
            kickr.changeGear(nextGear);
            kickr.getCurrentGear();
            lastChangeTime = currentTime;

        //     // Example of accessing riding data
        //     RidingData data = kickr.getCurrentRidingData();
        //     if (data.isValid)
        //     {
        //         if (data.power > 250)
        //         {
        //             Serial.println("High power output!");
        //         }
        //         if (data.cadence < 60)
        //         {
        //             Serial.println("Cadence too low!");
        //         }
        //     }
        }

    }
}
