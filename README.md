# MEMO — Desktop Reminder Display

A compact, Wi-Fi desktop reminder and deadline tracker built with a custom PCB, Seeed Studio XIAO ESP32-C3, and a 128 × 64 OLED display.

<img width="289" height="284" alt="image" src="https://github.com/user-attachments/assets/277a6ad3-cd80-407b-b1a2-ee0b13fc86ea" />


## Overview

MEMO is a custom-built desktop reminder and information display designed to make tracking upcoming deadlines easier. Powered by the Seeed Studio XIAO ESP32-C3, it uses an OLED screen to display reminders, their due dates, and the time remaining until each deadline.

The device connects to Wi-Fi to retrieve the current time through Network Time Protocol (NTP), allowing it to calculate deadlines automatically. Three physical push buttons provide simple navigation and reminder controls, while an RGB LED communicates the urgency of each reminder through colour-coded notifications.

## Features

- Reminder Display: Displays individual reminders, their due dates, and completion status.
- Real-Time Clock: Connects to Wi-Fi and synchronizes the current time using NTP.
- Deadline Countdown: Calculates the time remaining until each reminder is due.
- OLED Interface: Displays the current date, time, selected reminder, and its status.
- Three-Button Navigation: Browse between reminders and mark them as completed.
- RGB Notifications: Uses different LED colours to indicate the urgency of reminders.
- Custom PCB: Designed in KiCad around the Seeed Studio XIAO ESP32-C3.

## Hardware

## Hardware

MEMO is built around the Seeed Studio XIAO ESP32-C3, which handles reminder calculations, Wi-Fi connectivity, display communication, and physical inputs.

### Components & Bill of Materials

All prices are in CAD and reflect the estimated component costs at the time of purchase.

| Component                          | Qty Used | Qty Purchased | Purpose                            | Supplier  | Cost       | Total  |
| ---------------------------------- | -------- | ------------- | ---------------------------------- | --------- | ---------- | ------ |
| Seeed Studio XIAO ESP32-C3         | 1        | 1             | Main microcontroller               | RobotShop | $8.50      | $8.50  |
| 0.96" Grove OLED Display (SSD1315) | 1        | 1             | Displays reminders and information | RobotShop | $7.86      | $7.86  |
| Triple Output RGB LED              | 1        | 1             | Visual status notifications        | RobotShop | $3.29      | $3.29  |
| 330 Ω Resistors                    | 3        | 1 (20-pack)   | Limit RGB LED current              | RobotShop | $1.79/pack | $1.79  |
| Mini Push Button Switches          | 3        | 3             | Reminder navigation and selection  | RobotShop | $0.71      | $2.13  |
| Custom PCB                         | 1        | 1             | Connects all components            | JLCPCB    | $5.58      | $5.58  |
| Total                              | 10       |               |                                    |           |            | $29.15 |

The resistor purchase includes a 20-piece pack, of which three resistors are used on the PCB. Shipping is excluded from the total.

### Purchase Links

- [PCB Board — JLCPCB](https://cart.jlcpcb.com/quote?stencilLayer=2\&stencilWidth=100\&stencilLength=100\&stencilCounts=5\&plateType=1\&utm_source=chatgpt.com)
- [RGB LED — RobotShop](https://ca.robotshop.com/products/sfe-triple-output-led-rgb?utm_source=chatgpt.com)
- [330 Ω Resistors — RobotShop](https://ca.robotshop.com/products/resistor-330-ohm-1-4-watt-pth-20pk?utm_source=chatgpt.com)
- [Mini Push Button Switches — RobotShop](https://ca.robotshop.com/products/sfe-mini-push-button-switch?utm_source=chatgpt.com)
- [Seeed Studio XIAO ESP32-C3 — RobotShop](https://ca.robotshop.com/products/seeedstudio-xiao-esp32c3-tiny-mcu-w-wi-fi-ble?utm_source=chatgpt.com)
- [Grove OLED Display — RobotShop](https://ca.robotshop.com/products/seeedstudio-grove-oled-yellow-blue-display-096-in-ssd1315-v10?utm_source=chatgpt.com)

## PCB Design

The schematic and PCB were designed using KiCad. The layout was arranged to keep the microcontroller, OLED connector, buttons, RGB LED, and current-limiting resistors organized within a compact board.

The circuit uses the XIAO ESP32-C3's GPIO pins to control the three LED channels and read the push buttons. The OLED communicates with the microcontroller over I²C.

Three 330 Ω resistors are included to limit the current flowing through the RGB LED's individual colour channels. The push buttons use the microcontroller's internal pull-up resistors, allowing them to connect directly between their GPIO pins and GND.

### Pin Configuration

| Component | Function | XIAO Pin | GPIO |
|---|---|---|---|
| OLED | SDA | D5 | GPIO7 |
| OLED | SCL | D4 | GPIO6 |
| RGB LED | Red | D1 | GPIO3 |
| RGB LED | Green | D2 | GPIO4 |
| RGB LED | Blue | D3 | GPIO5 |
| UP Button | Input | D6 | GPIO21 |
| DOWN Button | Input | D7 | GPIO20 |
| SELECT Button | Input | D10 | GPIO10 |

### PCB Images

<img width="401" height="206" alt="image" src="https://github.com/user-attachments/assets/65fe6ed7-03f3-47d5-9f0c-c5781ee60601" />

<img width="229" height="205" alt="image" src="https://github.com/user-attachments/assets/bea8aa74-7824-4743-9e63-656ee8a368c7" />


## Controls

MEMO uses three physical push buttons to navigate through the reminder list.

| Button | Function |
|---|---|
| UP | Move to the previous reminder |
| DOWN | Move to the next reminder |
| SELECT | Mark the selected reminder as completed or incomplete |

The reminder list loops around when navigating. For example, pressing UP while viewing the first reminder takes you to the last reminder, while pressing DOWN on the last reminder returns to the first.

The buttons use `INPUT_PULLUP`, meaning that their normal state is HIGH and their pressed state is LOW. A debounce interval is also included to prevent a single physical press from being registered multiple times.

## RGB LED Status Indicators

The RGB LED provides a quick visual indication of the selected reminder's status.

| LED Colour | Meaning |
|---|---|
| Green | More than 24 hours remaining |
| Blue | Less than 24 hours remaining |
| Red | Reminder is overdue |
| Off | Reminder has been completed |

The LED also displays blue while the device is waiting for a valid time synchronization.

## Firmware

The firmware was written in C++ using Arduino IDE for the Seeed Studio XIAO ESP32-C3.

### Libraries

The following libraries and built-in ESP32 features are used:

| Library | Purpose |
|---|---|
| `WiFi.h` | Connects the ESP32-C3 to Wi-Fi |
| `Wire.h` | Handles I²C communication with the OLED |
| `time.h` | Provides time conversion and deadline calculations |
| `Adafruit_GFX` | Provides graphics and text functionality |
| `Adafruit_SSD1306` | Controls the OLED display |

### How It Works

**1. Reminder Storage**

Reminders are stored in a C++ structure containing:
- Title
- Year
- Month
- Day
- Hour
- Minute
- Completion status

An array stores multiple reminders, while `selectedReminder` tracks which entry is currently displayed.

**2. Time Synchronization**

When MEMO starts, it attempts to connect to Wi-Fi. After successfully connecting, it retrieves the current time from an NTP server.

The timezone is configured for Eastern Time, including automatic daylight saving time adjustments.

**3. Deadline Calculations**

The firmware converts each reminder's date and time into a timestamp using `mktime()`. It then compares this value with the current time using `difftime()`.

Based on the difference, MEMO determines whether a reminder is overdue, due within 24 hours, or due further in the future.

**4. OLED Updates**

The `updateDisplay()` function refreshes the OLED with the current date, time, selected reminder, and its deadline status.

The display refreshes approximately once every second using `millis()`.

**5. Button Interaction**

The main `loop()` continuously checks for button presses. Depending on the input, it changes the selected reminder or toggles its completion status.

**6. LED Notifications**

The `setLED()` function controls the three RGB channels independently, allowing the firmware to display the appropriate colour for each reminder status.

## Setting Up the Firmware

To upload the firmware to your own MEMO board:

1. Install [Arduino IDE](https://www.arduino.cc/en/software).
2. Install the ESP32 board package and select the Seeed Studio XIAO ESP32-C3.
3. Install the Adafruit GFX and Adafruit SSD1306 libraries through the Arduino Library Manager.
4. Open the MEMO firmware file.
5. Enter your Wi-Fi credentials in the following section:

```cpp
const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";
```

6. Check that the GPIO definitions match your PCB connections.
7. Connect the XIAO ESP32-C3 to your computer and select the correct port.
8. Compile and upload the firmware.

## Current Limitations and Future Improvements

This is the first firmware version of MEMO, so there are a few features that could be improved in future updates:

- **Hardcoded Reminders:** Reminders currently need to be edited directly in the source code and uploaded again.
- **No Persistent Storage:** Completion statuses reset when the device restarts.
- **Wi-Fi Dependency:** A network connection is required to synchronize the clock and obtain accurate time.
- **Limited Reminder Management:** The current buttons allow users to browse and complete reminders, but not create or edit them directly on the device.

Potential improvements for future versions include:

- Adding the ability to create, edit, and delete reminders directly on the device.
- Saving reminder data and completion statuses using ESP32 non-volatile storage.
- Improving the OLED interface with icons, better layouts, and additional information.
- Adding more notification options and customizable LED behaviour.
- Improving offline functionality by retaining the last synchronized time.

## Summary

MEMO is a custom-built desktop reminder system designed to make tracking upcoming deadlines and events easier. Powered by the Seeed Studio XIAO ESP32-C3, it uses an OLED display to show reminders, three push buttons for navigation and selection, and an RGB LED to communicate reminder statuses. The project combines custom PCB design in KiCad with firmware developed in Arduino IDE, including Wi-Fi connectivity and automatic time synchronization. 

<img width="600" height="300" alt="image" src="https://github.com/user-attachments/assets/7e362c4c-c3c7-43bc-862b-06fdb6f7b5b9" />
