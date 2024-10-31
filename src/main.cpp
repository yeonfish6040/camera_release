#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define out DD2

LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup() {
    Serial.begin(9600);

    pinMode(out, OUTPUT);

    digitalWrite(out, LOW);

    lcd.init();
    lcd.backlight();
}


// mode 0: bulb
// mode 1: interval
// mode 2: bulb interval - bulb
// mode 3: bulb interval - interval
int mode = 0;

int bulb = 20;
int intervalDuration = 1000;
int intervalRepeat = 3;

bool repaint = true;
bool running = false;
void loop() {
    if (repaint && !running) {
        lcd.clear();
        lcd.home();
        switch (mode) {
            case 0:
                lcd.setCursor(0, 0);
                lcd.print("Bulb mode");
                lcd.setCursor(0, 1);
                lcd.print(bulb);
                lcd.print("s");
                break;
            case 1:
                lcd.setCursor(0, 0);
                lcd.print("Interval mode");
                lcd.setCursor(0, 1);
                lcd.print(intervalDuration);
                lcd.print("ms ");
                lcd.print(intervalRepeat);
                lcd.print("ea");
                break;
            case 2:
            case 3:
                lcd.setCursor(0, 0);
                lcd.print(mode == 2 ? "Both mode - b" : "Both mode - i");
                lcd.setCursor(0, 1);
                lcd.print(bulb);
                lcd.print("s ");
                lcd.print(intervalDuration);
                lcd.print("ms ");
                lcd.print(intervalRepeat);
                lcd.print("ea");
                break;
        }
        repaint = false;
    }

    if (running) {
        if (mode == 0) {
            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 0);
            lcd.print("Running... Bulb");
            lcd.setCursor(0, 1);
            lcd.print(bulb);
            lcd.print("s");

            digitalWrite(out, HIGH);
            unsigned long time = millis();
            while (millis()-time < bulb*1000) {
                lcd.setCursor(0, 1);
                lcd.print(bulb-(millis()-time)/1000);
                lcd.print("s  ");
            }
            digitalWrite(out, LOW);

            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 0);
            lcd.print("Complete!");
            delay(2000);
        }
        running = false;
    }

    if (!running) {
        int read = Serial.read();
        if (read != -1) {
            if (read == 'm') {
                do {
                    read = Serial.read();
                } while (read == -1);
                mode = read-'0';
                if (mode > 3) mode = mode % 3;
            }
            if (read == '\n') running = true;
            repaint = true;
        }
    }
}