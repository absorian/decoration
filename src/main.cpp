#include <Arduino.h>

#include <GyverButton.h>
#include <DS3231.h>

#include "define.h"
#include "ledHandle.h"
#include "menuConstructor.h"

#define mRefreshMenu menuInit(); Hour = now.hour(); Minute = now.minute(); Second = now.second(); drawMenu();

struct parameter {
public:
    const char *text;
    int value;
    const int limit;
};

GButton btn_up(BUTTON1_PIN); // mechanic button
GButton btn_down(BUTTON2_PIN); // mechanic button
GButton btn_set(BUTTON3_PIN, LOW_PULL); // sensor


DS3231 clock;
unsigned long stamp_plotter, stamp_digits, stamp_value_blink, stamp_settings_delay, stamp_settings_timeout;
//            show plot &  , show right  , arrow blink in   , delay between enter/, timeout to quit settings
//            add to plot    bar & dots    settings           quit settings to
//                           blink in                         prevent random click
//                           normal Clock
bool settings_delay_state = false;

int average_count = 0;
double average_t = 0; // to plot

bool settings_mode = false, value_changing_mode = false;
bool blink = true; // default state

int settings_cursor = 0, lcd_cursor = 0;
const int setting_menu_size = 7;
parameter parameters[setting_menu_size] = {
        {"Brightness:", LED_BRIGHTNESS_DEFAULT,    255},
        {"Breathing:",  LED_BREATHING,             1},
        {"Breath spd:", LED_BREATHING_SPEED,       20},
        {"LED delay:",  static_cast<int>(delay_r), 50},
        {"Plot upper:", plotter_upper_limit,       50},
        {"Plot lower:", plotter_bottom_limit,      plotter_upper_limit - 1},
        {"Rect side:",  LED_SIDE,                  4}
};

void parametersApply(parameter *p) {
    setBrightness(p[0].value);
    setBreathing(p[1].value, p[2].value);
    delay_r = p[3].value;
    plotter_upper_limit = p[4].value;
    plotter_bottom_limit = p[5].value;
    setSide(p[6].value);
}

double readThermistor() {
    const double ROOM_TEMP = NOMINAL_T + 273.15;
    double r, t;

    r = SERIAL_R * ((1023.0 / analogRead(THERM_PIN)) - 1);

    t = (B * ROOM_TEMP) /
        (B + (ROOM_TEMP * log(r / THERMISTOR_R)));
    t -= 273.15;

    return t;
}

void GetDateStuff(byte &yr, byte &mth, byte &day, byte &dow,
                  byte &hr, byte &mnt, byte &sc) {
    // Call this if you notice something coming in on
    // the serial port. The stuff coming in should be in
    // the order YY MM DD w HH MM SS x (without spaces), with an 'x' at the end.
    boolean GotString = false;
    char InChar;
    byte Temp1, Temp2;
    char InString[20];

    byte j = 0;
    while (!GotString) {
        if (Serial.available()) {
            InChar = (char) Serial.read();
            InString[j] = InChar;
            j += 1;
            if (InChar == 'x') {
                GotString = true;
            }
        }
    }
    Serial.println(InString);
    // Read Year first
    Temp1 = (byte) InString[0] - 48;
    Temp2 = (byte) InString[1] - 48;
    yr = Temp1 * 10 + Temp2;
    // now month
    Temp1 = (byte) InString[2] - 48;
    Temp2 = (byte) InString[3] - 48;
    mth = Temp1 * 10 + Temp2;
    // now date
    Temp1 = (byte) InString[4] - 48;
    Temp2 = (byte) InString[5] - 48;
    day = Temp1 * 10 + Temp2;
    // now Day of Week
    dow = (byte) InString[6] - 48;
    // now Hour
    Temp1 = (byte) InString[7] - 48;
    Temp2 = (byte) InString[8] - 48;
    hr = Temp1 * 10 + Temp2;
    // now Minute
    Temp1 = (byte) InString[9] - 48;
    Temp2 = (byte) InString[10] - 48;
    mnt = Temp1 * 10 + Temp2;
    // now Second
    Temp1 = (byte) InString[11] - 48;
    Temp2 = (byte) InString[12] - 48;
    sc = Temp1 * 10 + Temp2;
}

// ......................................................................................................................
void setup() {
    pinMode(THERM_PIN, INPUT);
    stamp_plotter = 0;
    stamp_digits = DELAY_T_REFRESH;
    stamp_value_blink = 700;

    btn_down.setTickMode(AUTO);
    btn_up.setTickMode(AUTO);
    btn_set.setTickMode(AUTO);

    btn_set.setTimeout(2000);

    btn_up.setStepTimeout(200);
    btn_down.setStepTimeout(200);

    Serial.begin(9600);
    lcd.begin();
    lcd.backlight();
    initLeds();

    DateTime now = RTClib::now();
    mRefreshMenu
}

// ......................................................................................................................
void loop() {
    // read
    double temperature = readThermistor();
    DateTime now = RTClib::now();

    // time set throughout serial
    if (Serial.available()) {
        GetDateStuff(Year, Month, Date, DoW, Hour, Minute, Second);

        clock.setClockMode(false);    // set to 24h
        //setClockMode(true);	// set to 12h

        clock.setYear(Year);
        clock.setMonth(Month);
        clock.setDate(Date);
        clock.setDoW(DoW);
        clock.setHour(Hour);
        clock.setMinute(Minute);
        clock.setSecond(Second);
    }

    // led Tick
    if (millis() - led_stamp >= delay_r) {
        led_stamp = millis();
        if (LED_BREATHING)
            mode_breathing();
        ledTick();
    }

    // settings enter/out
    if (btn_set.isHolded()) {
        lcd.clear();

        settings_mode = !settings_mode;
        if (!settings_mode) {
            value_changing_mode = false;
            mRefreshMenu
        } else { // on enable begin timeout count
            stamp_settings_timeout = millis();
            blink = true;
        }

        settings_delay_state = true;
        stamp_settings_delay = millis();
    }

    // freezing btn_set after entering/quiting settings
    if (millis() - stamp_settings_delay < 800 && settings_delay_state)
        btn_set.resetStates();
    else settings_delay_state = false;

    if (!settings_mode) {
        // mode handle
        if (btn_up.isPress() && LED_MODE < 3) {
            setMode(LED_MODE + 1);
        } else if (btn_down.isPress() && LED_MODE > 0) {
            setMode(LED_MODE - 1);
        }

        bool changed = btn_set.isRelease();
        if (changed) {
            lcd.clear();
            if (menu_mode < 3)
                menu_mode++;
            else menu_mode = 0;
            mRefreshMenu
        }

        drawMenu(now);
    }

    // plotter refresh handle
    if (millis() - stamp_plotter >= DELAY_PLOTTER) {
        stamp_plotter = millis();

        double temp = average_t / average_count;
        average_count = 0;
        average_t = 0;
        addToPlot(temp);

        if (!settings_mode && menu_mode == 0) {
            drawPlot();
        }
    }

    // temperature refresh handle
    if (millis() - stamp_digits >= DELAY_T_REFRESH) {
        stamp_digits = millis();

        average_t += temperature;
        average_count++;

        if (!settings_mode) {
            if (menu_mode != 3) { // if not big clock

                // temperature print
                lcd.setCursor(LCD_BORDER, 1);
                lcd.print("T:" + String(temperature));

                // time print
                String hrs_s, min_s;
                hrs_s += (Hour < 10 ? "0" : "") + String(Hour);
                min_s += (Minute < 10 ? "0" : "") + String(Minute);
                lcd.setCursor(LCD_BORDER, 0);
                lcd.print("B:" + hrs_s + ":" + min_s);
            } else dotsBlink(7); // blink for big clock
        }
    }

    // settings mode ....................................................................................................
    if (settings_mode) {

        // enter the values manage mode
        if (btn_set.isRelease()) {
            value_changing_mode = !value_changing_mode;
            blink = true;

            Serial.print("Value changing mode: ");
            Serial.println(value_changing_mode);

            stamp_settings_timeout = millis(); // start time count again because of activity
        }

        if (value_changing_mode) {
            // realtime response
            parametersApply(parameters);

            // value change
            if (btn_up.isClick() || btn_up.isStep()) {
                // set limitation
                if (parameters[settings_cursor + lcd_cursor].value <
                    parameters[settings_cursor + lcd_cursor].limit)
                    parameters[settings_cursor + lcd_cursor].value++;
                else parameters[settings_cursor + lcd_cursor].value = 0;
                stamp_settings_timeout = millis(); // start time count again because of activity
            }
            if (btn_down.isClick() || btn_down.isStep()) {
                // set limitation
                if (parameters[settings_cursor + lcd_cursor].value > 0)
                    parameters[settings_cursor + lcd_cursor].value--;
                else parameters[settings_cursor + lcd_cursor].value = parameters[settings_cursor + lcd_cursor].limit;
                stamp_settings_timeout = millis(); // start time count again because of activity
            }
            // value blink
            if (millis() - stamp_value_blink >= 700) {
                stamp_value_blink = millis();

                blink = !blink;
            }
        } else {
            // menu listing mechanics
            if (btn_up.isPress()) {
                lcd.clear();
                if (lcd_cursor > 0)
                    lcd_cursor--;
                else if (settings_cursor > 0)
                    settings_cursor--;

                stamp_settings_timeout = millis(); // start time count again because of activity
            } else if (btn_down.isPress()) {
                lcd.clear();
                if (lcd_cursor < LCD_HEIGHT - 1)
                    lcd_cursor++;
                else if (settings_cursor < setting_menu_size - 1 - (lcd_cursor % LCD_HEIGHT))
                    settings_cursor++;

                stamp_settings_timeout = millis(); // start time count again because of activity
            }
        }

        String to_show[LCD_HEIGHT];
        for (int i = 0; i < LCD_HEIGHT; ++i) {
            // fill menu
            if (settings_cursor < setting_menu_size - 1) {
                to_show[i] = parameters[settings_cursor + i].text + String(parameters[settings_cursor + i].value);
                if (parameters[settings_cursor + i].value < 100) to_show[i] += " ";
                if (parameters[settings_cursor + i].value < 10) to_show[i] += "  ";
            }
            // show menu
            lcd.setCursor(0, i);
            lcd.print(to_show[i]);
        }

        // show pointer
        lcd.setCursor(LCD_WIDTH - 1, lcd_cursor);
        lcd.print(blink ? "<" : " ");

        // timeout
        if (millis() - stamp_settings_timeout >= SETTINGS_TIMEOUT) {
            lcd.clear();
            settings_mode = false;
            value_changing_mode = false;
            mRefreshMenu
        }
    }
}
