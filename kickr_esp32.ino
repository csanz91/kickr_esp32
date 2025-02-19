#include "kickr_protocol.h"
#include "ButtonHandler.h"

KickrProtocol kickr;

// Button handler instances
ButtonHandler *upButton;
ButtonHandler *downButton;

void setup()
{
    Serial.begin(115200);
    kickr.init();

    // Initialize button handlers using lambda callbacks for shiftGear
    upButton = new ButtonHandler(UP_READ_PIN, UP_GND_PIN, [&](int8_t numGears)
                                 { kickr.shiftGear(numGears); });
    downButton = new ButtonHandler(DOWN_READ_PIN, DOWN_GND_PIN, [&](int8_t numGears)
                                   { kickr.shiftGear(-numGears); });
}

void loop()
{
    unsigned long currentTime = millis();

    // Handle all periodic tasks (connection and gear status)
    kickr.handleTasks(currentTime);

    // Only process button updates when connected
    if (kickr.isConnected())
    {
        upButton->update();
        downButton->update();
    }
}
