#pragma once

#define DEBUG 0

// Bluetooth Service and Characteristic UUIDs
constexpr char KICKR_SERVICE_UUID[] = "00000001-19ca-4651-86e5-fa29dcdd09d1";
constexpr char KICKR_WRITE_CMD_UUID[] = "00000003-19ca-4651-86e5-fa29dcdd09d1";
constexpr char KICKR_READ_CMD_UUID[] = "00000004-19ca-4651-86e5-fa29dcdd09d1";
constexpr char KICKR_READ_DATA_UUID[] = "00000002-19ca-4651-86e5-fa29dcdd09d1";

// Device configuration
constexpr char DEVICE_NAME[] = "KICKR CORE B6F2";
constexpr char CLIENT_NAME[] = "ESP32_KICKR_Virtual_Shifter";

// Protocol constants
constexpr uint8_t MSG_HELLO[] = {0x52, 0x69, 0x64, 0x65, 0x4F, 0x6E, 0x01, 0x02};
constexpr uint8_t MSG_DEVICE_INFO[] = {0x00, 0x08, 0x88, 0x04};

// Timing constants
constexpr unsigned long GEAR_CHANGE_FETCH_INTERVAL = 1000;  // ms
constexpr uint16_t BLE_SCAN_INTERVAL = 1349;
constexpr uint16_t BLE_SCAN_WINDOW = 449;
constexpr unsigned long DEBOUNCE_TIME = 200; // ms
constexpr unsigned long GEAR_UPDATE_DELAY = 1000; // ms
constexpr unsigned long LONG_PRESS_DURATION = 700; // ms This constant defines the minimum duration that a button must be held down to be considered a "long press." If the button is held down for longer than this duration, it triggers a long press action.

constexpr unsigned long LONG_PRESS_INTERVAL = 300; // ms This constant defines the interval at which repeated actions occur during a long press. Once a long press is detected, actions (such as changing gears) will be triggered repeatedly at this interval as long as the button remains pressed.

// Gear configuration
constexpr uint16_t GEAR_RATIOS[] = {
    7500, 8700, 9900, 11100, 12300, 13799, 15300, 16800, 18600,
    20400, 22200, 24000, 26099, 28200, 30299, 32400, 34900, 37400,
    39900, 42399, 45400, 48400, 51400, 54899, 57899
};

constexpr size_t MAX_GEAR = sizeof(GEAR_RATIOS) / sizeof(GEAR_RATIOS[0]);

// Pin configuration
#define UP_READ_PIN 1
#define UP_GND_PIN 3
#define DOWN_READ_PIN 20
#define DOWN_GND_PIN 9
