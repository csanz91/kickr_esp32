# ESP32 KICKR Virtual Shifter

This project allows an ESP32 to connect to a Wahoo KICKR CORE smart trainer and control the virtual gears using the builtin feature instead of simulating the wheel diamater or the resistance. It uses Bluetooth Low Energy (BLE) to communicate with the KICKR.

## Features

-   Connects to a KICKR CORE trainer via BLE.
-   Controls and retrieves the gears.
-   Includes basic riding data retrieval (power, cadence, speed, heart rate).
-   Supports hardware buttons for gear control.

## Hardware Required

-   ESP32 development board. It was tested with an ESP32 C3.
-   Wahoo KICKR CORE smart trainer.
-   Two momentary push buttons for gear control.

## Software Required

-   Arduino IDE with ESP32 board support.
-   BLE library for ESP32 (included with ESP32 board support).

## Installation

1.  Install the ESP32 board support in the Arduino IDE.
    -   Go to `File` \> `Preferences`.
    -   Add `https://dl.espressif.com/dl/package_esp32_index.json` to the "Additional Boards Manager URLs" field.
    -   Go to `Tools` \> `Board` \> `Boards Manager`.
    -   Search for "ESP32" and install the "esp32 by Espressif Systems" package.
2.  Clone this repository to your local machine.
3.  Open the `kickr_esp32.ino` file in the Arduino IDE.
4.  Change the [`DEVICE_NAME`] to match your device and [`GEAR_RATIOS`] as desired from [`kickr_constants.h`](kickr_constants.h).
5.  Ensure the correct board is selected (`Tools` \> `Board` \> `ESP32 Dev Module` or similar).
6.  Upload the code to your ESP32.

## Usage

1.  Power on your KICKR trainer.
2.  Open the Serial Monitor in the Arduino IDE (set baud rate to 115200).
3.  The ESP32 will scan for the KICKR and attempt to connect.
4.  Once connected, use the hardware buttons to change gears.
5.  The Serial Monitor will display connection status, current gear, and other debug information.

## Code Overview

### `kickr_esp32.ino`

-   Main sketch file.
-   Initializes BLE, sets up the KICKR protocol, and handles the main program loop.
-   Scans for the KICKR device using `BLEDevice::getScan()`.
-   Connects to the KICKR using the [`KickrProtocol::connect`](kickr_protocol.cpp) method.
-   Changes gears using hardware buttons.
-   The `loop()` function contains the main logic for connecting and controlling the KICKR.

### `kickr_protocol.h` and `kickr_protocol.cpp`

-   Defines the [`KickrProtocol`](kickr_protocol.h) class, which encapsulates the BLE communication with the KICKR.
-   Includes methods for connecting, disconnecting, changing gears, sending hello messages, and enabling notifications.
-   Implements the BLE communication using the ESP32 BLE library.
-   The [`KickrProtocol::commandCallback`](kickr_protocol.cpp) method handles incoming data from the KICKR, including gear information and hello responses.
-   The [`KickrProtocol::decode_riding_data`](kickr_protocol.cpp) method decodes riding data.

### `kickr_constants.h`

-   Defines constants such as BLE service and characteristic UUIDs, device names, protocol constants, timing constants, and gear ratios.
-   The `GEAR_RATIOS` array defines the available gear ratios for the KICKR.

## Configuration

-   `DEVICE_NAME` in [`kickr_constants.h`](kickr_constants.h):  Change this if your KICKR has a different name.
-   `GEAR_CHANGE_INTERVAL` in [`kickr_constants.h`](kickr_constants.h): Adjust the interval between gear changes (in milliseconds).
-   `GEAR_RATIOS` in [`kickr_constants.h`](kickr_constants.h):  This array defines the gear ratios that will be sent to the KICKR.  You can customize these values to match your desired virtual gearing.

## Troubleshooting

-   **Failed to connect:**
    -   Make sure your KICKR is powered on and not connected to any other devices.
    -   Verify that the `DEVICE_NAME` in [`kickr_constants.h`](kickr_constants.h) matches the name of your KICKR.
-   **No data received:**
    -   Double-check the BLE service and characteristic UUIDs in [`kickr_constants.h`](kickr_constants.h).
    -   Ensure that notifications are enabled for the data characteristic.

## Future Enhancements

-   Implement a user interface (e.g., web interface or mobile app) to control the KICKR.
-   Support external bluetooth buttons.

## License

This project is licensed under the [MIT License](LICENSE).