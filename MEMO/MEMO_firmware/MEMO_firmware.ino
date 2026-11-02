/*
  MEMO - Desktop Reminder Display
  Version 1.0
  Board: Seeed Studio XIAO ESP32-C3

  OLED: SDA = D5, SCL = D4
  Buttons: UP = D6, DOWN = D7, SELECT = D10
  RGB LED: RED = D1, GREEN = D2, BLUE = D3
*/

// ---------------- REMINDERS ----------------

struct Reminder {
  const char* title;
  int year;
  int month;
  int day;
  int hour;
  int minute;
  bool completed;
};

// Edit these reminders to suit your schedule.
// Months use normal calendar numbers (1 = January).

Reminder reminders[] = {
  {"Physics Assignment", 2026, 11, 5, 23, 59, false},
  {"Math Test",          2026, 11, 8, 9, 0, false},
  {"Science Project",    2026, 11, 12, 23, 59, false},
  {"Club Meeting",       2026, 11, 15, 15, 0, false}
};

const int reminderCount =
  sizeof(reminders) / sizeof(reminders[0]);

int selectedReminder = 0;


#include <WiFi.h>
#include <Wire.h>
#include <time.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- WIFI ----------------

const char* ssid = "YOUR_WIFI_NAME";
const char* password = "YOUR_WIFI_PASSWORD";

//Set for my location in Toronto time zone 
const char* timezone = "EST5EDT,M3.2.0/2,M11.1.0/2";

// ---------------- OLED ----------------

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDRESS 0x3C

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  -1
);

// ---------------- PIN CONFIGURATION ----------------

#define SDA_PIN D5
#define SCL_PIN D4

#define BUTTON_UP D6
#define BUTTON_DOWN D7
#define BUTTON_SELECT D10

#define LED_RED D1
#define LED_GREEN D2
#define LED_BLUE D3

// ---------------- BUTTON HANDLING ----------------

bool buttonPressed(int pin, unsigned long &lastPress) {
  if (digitalRead(pin) == LOW &&
      millis() - lastPress > 200) {

    lastPress = millis();

    while (digitalRead(pin) == LOW) {
      delay(5);
    }

    return true;
  }

  return false;
}

unsigned long lastUp = 0;
unsigned long lastDown = 0;
unsigned long lastSelect = 0;

// ---------------- LED CONTROL ----------------

void setLED(bool red, bool green, bool blue) {
  digitalWrite(LED_RED, red);
  digitalWrite(LED_GREEN, green);
  digitalWrite(LED_BLUE, blue);
}

// ---------------- DEADLINE CALCULATION ----------------

time_t getDeadline(Reminder &r) {
  struct tm deadline = {};

  deadline.tm_year = r.year - 1900;
  deadline.tm_mon = r.month - 1;
  deadline.tm_mday = r.day;
  deadline.tm_hour = r.hour;
  deadline.tm_min = r.minute;
  deadline.tm_isdst = -1;

  return mktime(&deadline);
}

// ---------------- DISPLAY ----------------

void updateDisplay() {
  struct tm currentTime;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);

  if (!getLocalTime(&currentTime, 100)) {
    display.println("MEMO");
    display.println();
    display.println("Waiting for WiFi time...");
    display.display();

    setLED(false, false, true);
    return;
  }

  char timeBuffer[20];
  char dateBuffer[20];

  strftime(timeBuffer, sizeof(timeBuffer),
           "%I:%M %p", &currentTime);

  strftime(dateBuffer, sizeof(dateBuffer),
           "%b %d, %Y", &currentTime);

  display.println("MEMO  |  REMINDERS");
  display.println(timeBuffer);
  display.println(dateBuffer);
  display.println("---------------------");

  Reminder &r = reminders[selectedReminder];

  display.print(selectedReminder + 1);
  display.print("/");
  display.println(reminderCount);

  display.println(r.title);

  if (r.completed) {
    display.println("Status: COMPLETED");
    setLED(false, false, false);
  } else {
    time_t now = time(nullptr);
    time_t deadline = getDeadline(r);

    long secondsLeft = difftime(deadline, now);

    display.print("Due: ");
    display.printf("%02d/%02d %02d:%02d\n",
                   r.month, r.day, r.hour, r.minute);

    if (secondsLeft < 0) {
      display.println("Status: OVERDUE");
      setLED(true, false, false);
    } else if (secondsLeft < 86400) {
      long hours = secondsLeft / 3600;
      long minutes = (secondsLeft % 3600) / 60;

      display.printf("Due in: %ldh %ldm\n",
                     hours, minutes);

      setLED(false, false, true);
    } else {
      long days = secondsLeft / 86400;

      display.printf("Due in: %ld days\n", days);

      setLED(false, true, false);
    }
  }

  display.println("UP/DOWN: Browse");
  display.println("SELECT: Complete");

  display.display();
}

// ---------------- SETUP ----------------

void setup() {
  Serial.begin(115200);

  // Initialize buttons
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);

  // Initialize RGB LED
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  setLED(false, false, false);

  // Initialize OLED with your chosen I2C pins
  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(
        SSD1306_SWITCHCAPVCC,
        OLED_ADDRESS)) {
    Serial.println("OLED initialization failed!");
    while (true) {
      delay(100);
    }
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("MEMO starting...");
  display.display();

  // Connect to WiFi
  WiFi.begin(ssid, password);

  display.println("Connecting WiFi...");
  display.display();

  unsigned long startAttempt = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startAttempt < 20000) {
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    configTzTime(timezone,
                 "pool.ntp.org",
                 "time.nist.gov");

    // Wait for the clock to synchronize
    struct tm timeInfo;

    getLocalTime(&timeInfo, 15000);
  } else {
    Serial.println("WiFi connection failed!");
  }

  updateDisplay();
}

// ---------------- MAIN LOOP ----------------

void loop() {

  // Previous reminder
  if (buttonPressed(BUTTON_UP, lastUp)) {
    selectedReminder--;

    if (selectedReminder < 0) {
      selectedReminder = reminderCount - 1;
    }

    updateDisplay();
  }

  // Next reminder
  if (buttonPressed(BUTTON_DOWN, lastDown)) {
    selectedReminder++;

    if (selectedReminder >= reminderCount) {
      selectedReminder = 0;
    }

    updateDisplay();
  }

  // Mark reminder as completed
  if (buttonPressed(BUTTON_SELECT, lastSelect)) {
    reminders[selectedReminder].completed =
      !reminders[selectedReminder].completed;

    updateDisplay();
  }

  // Refresh the clock and deadline countdown
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= 1000) {
    lastUpdate = millis();
    updateDisplay();
  }
}
