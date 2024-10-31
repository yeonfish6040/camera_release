#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define out DD2
#define breakOnEnter if (Serial.read() == '\n') break;

LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup() {
    Serial.begin(9600);

    pinMode(out, OUTPUT);

    digitalWrite(out, LOW);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.home();
}


// mode 0: bulb
// mode 1: interval
// mode 2: bulb interval
int mode = 0;

unsigned int bulb = 1;
unsigned int intervalDuration = 30;
unsigned int intervalRepeat = 3;

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
                lcd.print("00ms ");
                lcd.print(intervalRepeat);
                lcd.print("ea");
                break;
            case 2:
            case 3:
                lcd.setCursor(0, 0);
                lcd.print("Both mode");
                lcd.setCursor(0, 1);
                lcd.print(bulb);
                lcd.print("s ");
                lcd.print(intervalDuration);
                lcd.print("s ");
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
            while (millis()-time < (bulb*1000)) {
                lcd.setCursor(0, 1);
                lcd.print(bulb-(millis()-time)/1000);
                lcd.print("s  ");

                breakOnEnter
            }
            digitalWrite(out, LOW);

            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 0);
            lcd.print("Complete!");
            delay(2000);
        }
        if (mode == 1) {
            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 0);
            lcd.print("Running... Interval");

            int repeatTmp = intervalRepeat;
            unsigned long time = millis();
            while (repeatTmp != 0) {
                while ((millis()-time)/(intervalDuration*100) == (intervalRepeat-repeatTmp)) {
                    lcd.setCursor(0, 1);
                    lcd.print(((intervalDuration*100)-((millis()-time)%(intervalDuration*100)))/100);
                    lcd.print("00ms ");
                    lcd.print(repeatTmp);
                    lcd.print("ea   ");

                    breakOnEnter
                }
                digitalWrite(out, HIGH);
                delay(10);
                digitalWrite(out, LOW);
                repeatTmp--;
            }

            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 0);
            lcd.print("Complete!");
            delay(2000);
        }
        if (mode == 2) {
            lcd.clear();
            lcd.home();
            lcd.setCursor(0, 0);
            lcd.print("Running... both");

            int repeatTmp = intervalRepeat;
            unsigned long time = millis();
            while (repeatTmp != 0) {
                while ((millis()-time)/(intervalDuration*1000) == (intervalRepeat-repeatTmp)) {
                    lcd.setCursor(0, 1);
                    lcd.print(((intervalDuration*1000)-((millis()-time)%(intervalDuration*1000)))/1000);
                    lcd.print("s ");
                    lcd.print(repeatTmp);
                    lcd.print("ea   ");

                    breakOnEnter
                }
                digitalWrite(out, HIGH);
                unsigned long time2 = millis();
                while (millis()-time2 < bulb*1000) {
                    lcd.setCursor(0, 1);
                    lcd.print(bulb-(millis()-time2)/1000);
                    lcd.print("s               ");

                    breakOnEnter
                }
                digitalWrite(out, LOW);
                repeatTmp--;
            }

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
                if (mode > 2) mode = mode % 3;
            }
            if (read == 'q') bulb++;
            if (read == 'w') intervalDuration++;
            if (read == 'e') intervalRepeat++;
            if (read == 'a') bulb--;
            if (read == 's') intervalDuration--;
            if (read == 'd') intervalRepeat--;
            if (read == 'r') bulb += 10;
            if (read == 't') intervalDuration += 10;
            if (read == 'y') intervalRepeat += 10;
            if (read == 'f') bulb -= 10;
            if (read == 'g') intervalDuration -= 10;
            if (read == 'h') intervalRepeat -= 10;
            if (read == '\n') running = true;
            repaint = true;
        }
    }
}