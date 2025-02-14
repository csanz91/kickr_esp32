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
constexpr unsigned long GEAR_CHANGE_INTERVAL = 5000;  // ms
constexpr uint16_t BLE_SCAN_INTERVAL = 1349;
constexpr uint16_t BLE_SCAN_WINDOW = 449;

// Gear configuration
constexpr uint16_t GEAR_RATIOS[] = {
    7500, 8700, 9900, 11100, 12300, 13799, 15300, 16800, 18600,
    20400, 22200, 24000, 26099, 28200, 30299, 32400, 34900, 37400,
    39900, 42399, 45400, 48400, 51400, 54899, 57899
};
