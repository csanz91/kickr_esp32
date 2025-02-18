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
4.  Change the [`GEAR_RATIOS`] as desired from [`kickr_constants.h`](kickr_constants.h).
5.  Ensure the correct board is selected (`Tools` \> `Board` \> `ESP32 Dev Module` or similar).
6.  Upload the code to your ESP32.

## Usage

1.  Power on your KICKR trainer.
2.  Open the Serial Monitor in the Arduino IDE (set baud rate to 115200).
3.  The ESP32 will scan for the KICKR and attempt to connect.
4.  Once connected, use the hardware buttons to change gears.
5.  The Serial Monitor will display connection status, current gear, and other debug information.


## Configuration

-   `DEVICE_NAME` in [`kickr_constants.h`](kickr_constants.h):  Change this if your KICKR has a different name.
-   `GEAR_CHANGE_INTERVAL` in [`kickr_constants.h`](kickr_constants.h): Adjust the interval between gear changes (in milliseconds).
-   `GEAR_RATIOS` in [`kickr_constants.h`](kickr_constants.h):  This array defines the gear ratios that will be sent to the KICKR.  You can customize these values to match your desired virtual gearing.
-   `DEBOUNCE_TIME` in `kickr_constants.h`: Set to 20ms for responsive button handling.
-   `GEAR_LONG_PRESS_NUM_SHIFTS` in `kickr_constants.h`: Number of gears to shift during a long press.

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