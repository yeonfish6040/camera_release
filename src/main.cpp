#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#define out DD2
#define ll long
#define breakOnEnter if (Serial.read() == '\n') break;

using namespace std;

LiquidCrystal_I2C lcd(0x3f, 16, 2);

void setup() {
    pinMode(out, OUTPUT);

    digitalWrite(out, HIGH);

    lcd.init();
    lcd.backlight();
    lcd.clear();
    lcd.home();

    Serial.begin(9600);
    while (!Serial) continue;
}


// mode 0: bulb
// mode 1: interval
// mode 2: bulb interval
int mode = 0;

ll bulb = 1;
ll intervalDuration = 30;
ll intervalRepeat = 3;

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

            digitalWrite(out, LOW);
            ll time = (ll)millis();
            while (bulb-((ll)millis()-time)/1000 > 0) {
                lcd.setCursor(0, 1);
                lcd.print(bulb-((ll)millis()-time)/1000);
                lcd.print("s  ");

                breakOnEnter
            }
            digitalWrite(out, HIGH);

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

            ll repeatTmp = intervalRepeat;
            ll time = (ll)millis();
            while (repeatTmp != 0) {
                while (((ll)millis()-time)/(intervalDuration*100) == (intervalRepeat-repeatTmp)) {
                    lcd.setCursor(0, 1);
                    lcd.print(((intervalDuration*100)-(((ll)millis()-time)%(intervalDuration*100)))/100);
                    lcd.print("00ms ");
                    lcd.print(repeatTmp);
                    lcd.print("ea   ");

                    breakOnEnter
                }
                digitalWrite(out, LOW);
                delay(1);
                digitalWrite(out, HIGH);
                digitalWrite(out, LOW);
                delay(10);
                digitalWrite(out, HIGH);
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

            ll repeatTmp = intervalRepeat;
            ll time = (ll)millis();
            while (repeatTmp != 0) {
                while (((ll)millis()-time)/(intervalDuration*1000) == (intervalRepeat-repeatTmp)) {
                    lcd.setCursor(0, 1);
                    lcd.print(((intervalDuration*1000)-(((ll)millis()-time)%(intervalDuration*1000)))/1000);
                    lcd.print("s ");
                    lcd.print(repeatTmp);
                    lcd.print("ea        ");

                    breakOnEnter
                }
                // wake up
                digitalWrite(out, LOW);
                delay(1);
                digitalWrite(out, HIGH);
                digitalWrite(out, LOW);
                ll time2 = (ll)millis();
                while (bulb-((ll)millis()-time2)/1000 > 0) {
                    lcd.setCursor(0, 1);
                    lcd.print("bulb ");
                    lcd.print(bulb-((ll)millis()-time2)/1000);
                    lcd.print("s               ");

                    breakOnEnter
                }
                digitalWrite(out, HIGH);
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
            if (read == 'u') bulb += 100;
            if (read == 'i') intervalDuration += 100;
            if (read == 'o') intervalRepeat += 100;
            if (read == 'j') bulb -= 100;
            if (read == 'k') intervalDuration -= 100;
            if (read == 'l') intervalRepeat -= 100;
            if (read == '\n') running = true;
            repaint = true;
        }
    }
}