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

    // Initialize button handlers with signed integers for gear changes
    upButton = new ButtonHandler(UP_READ_PIN, UP_GND_PIN, true, std::bind(&KickrProtocol::shiftGear, &kickr, std::placeholders::_1));
    downButton = new ButtonHandler(DOWN_READ_PIN, DOWN_GND_PIN, false, std::bind(&KickrProtocol::shiftGear, &kickr, std::placeholders::_1));
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
