#include <BLEDevice.h>
#include "kickr_protocol.h"
#include "kickr_constants.h"

KickrProtocol kickr;
bool deviceConnected = false;
bool doConnect = false;
bool getGear = false;
BLEAddress *pServerAddress = nullptr;

class ClientCallback : public BLEClientCallbacks
{
    void onConnect(BLEClient *pclient) override
    {
        deviceConnected = true;
        Serial.println("Connected");
    }

    void onDisconnect(BLEClient *pclient) override
    {
        deviceConnected = false;
        Serial.println("Disconnected");
    }
};

void setupBLEDevice()
{
    BLEDevice::init(CLIENT_NAME);
    auto *pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new KickrAdvertisedDeviceCallbacks());
    pBLEScan->setInterval(BLE_SCAN_INTERVAL);
    pBLEScan->setWindow(BLE_SCAN_WINDOW);
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30, false);
}

void setupGearPins()
{
    // Setup UP switch pins
    pinMode(UP_GND_PIN, OUTPUT);
    pinMode(UP_READ_PIN, INPUT_PULLUP);
    digitalWrite(UP_GND_PIN, LOW);

    // Setup DOWN switch pins
    pinMode(DOWN_GND_PIN, OUTPUT);
    pinMode(DOWN_READ_PIN, INPUT_PULLUP);
    digitalWrite(DOWN_GND_PIN, LOW);
}

void setup()
{
    Serial.begin(115200);
    setupBLEDevice();

    auto *pClient = BLEDevice::createClient();
    pClient->setClientCallbacks(new ClientCallback());
    setupGearPins();

    pinMode(8, OUTPUT);
    digitalWrite(8, HIGH);
}

void loop()
{
    static unsigned long lastChangeTime = 0;
    static unsigned long lastUpPress = 0;
    static unsigned long lastDownPress = 0;
    static bool upLongPress = false;
    static bool downLongPress = false;

    // Attempt to connect if requested
    if (doConnect)
    {
        // kickr.enableDataNotifications = true;
        if (kickr.connect(*pServerAddress))
        {
            Serial.println("Successfully connected to KICKR.");
        }
        else
        {
            Serial.println("Failed to connect to KICKR.");
        }
        doConnect = false;
    }

    // Control the KICKR if connected
    if (deviceConnected)
    {
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

        unsigned long currentTime = millis();
        // Check UP switch - note we look for LOW because of INPUT_PULLUP
        if (digitalRead(UP_READ_PIN) == LOW)
        {
            if (currentTime - lastUpPress > DEBOUNCE_TIME)
            {
                if (!upLongPress && currentTime - lastUpPress > LONG_PRESS_DURATION)
                {
                    upLongPress = true;
                }
                if (upLongPress && currentTime - lastUpPress > LONG_PRESS_INTERVAL)
                {
                    kickr.gearUp();
                    lastUpPress = currentTime;
                }
                else if (!upLongPress)
                {
                    kickr.gearUp();
                    lastUpPress = currentTime;
                }
            }
        }
        else
        {
            if (upLongPress)
            {
                upLongPress = false;
            }
        }

        // Check DOWN switch - note we look for LOW because of INPUT_PULLUP
        if (digitalRead(DOWN_READ_PIN) == LOW)
        {
            if (currentTime - lastDownPress > DEBOUNCE_TIME)
            {
                if (!downLongPress && currentTime - lastDownPress > LONG_PRESS_DURATION)
                {
                    downLongPress = true;
                }
                if (downLongPress && currentTime - lastDownPress > LONG_PRESS_INTERVAL)
                {
                    kickr.gearDown();
                    lastDownPress = currentTime;
                }
                else if (!downLongPress)
                {
                    kickr.gearDown();
                    lastDownPress = currentTime;
                }
            }
        }
        else
        {
            if (downLongPress)
            {
                downLongPress = false;
            }
        }

        const unsigned long timeSinceLastGearChange = min(currentTime - lastUpPress, currentTime - lastDownPress);
        if (timeSinceLastGearChange < GEAR_CHANGE_FETCH_INTERVAL)
        {
            getGear = true;
        }
        if (getGear && timeSinceLastGearChange >= GEAR_CHANGE_FETCH_INTERVAL)
        {
            kickr.getCurrentGear();
            getGear = false;
        }
    }
}
