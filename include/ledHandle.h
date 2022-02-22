
#ifndef DECORATION_LEDHANDLE_H
#define DECORATION_LEDHANDLE_H

#include <Arduino.h>

#include <FastLED.h>
#include "define.h"

// led vars (change only in functions for aesthetic reasons)
struct CRGB LED_STRIP[LED_COUNT];
bool LED_BREATHING = false;
int LED_BREATHING_SPEED = 3;
int LED_MODE = 1;
byte LED_SIDE = 0;

// modes
void mode_fire(int huestart = 0, byte saturation = 255); // huestart - fire color (0 red, 80 green, 140 thunder, 190 pink)

void mode_rainbow(int speed = 1);

void mode_breathing(int speed = LED_BREATHING_SPEED);

void mode_colorCycle(byte saturation = 255);

// required for modes
unsigned long led_stamp = 0;
int counter = 0;
unsigned int delay_r = 26;


// service functions
CRGB wheel(byte WheelPos) {
    static CRGB c;

    if (WheelPos < 85) {
        c[0] = WheelPos * 3;
        c[1] = 255 - WheelPos * 3;
        c[2] = 0;
    } else if (WheelPos < 170) {
        WheelPos -= 85;
        c[0] = 255 - WheelPos * 3;
        c[1] = 0;
        c[2] = WheelPos * 3;
    } else {
        WheelPos -= 170;
        c[0] = 0;
        c[1] = WheelPos * 3;
        c[2] = 255 - WheelPos * 3;
    }

    return c;
}


void fillRGB(int cred, int cgrn, int cblu) {
    for (auto &i : LED_STRIP) {
        i.setRGB(cred, cgrn, cblu);
    }
}

void fillHSV(int ahue, byte saturation = 255) {
    for (auto &i : LED_STRIP) {
        i = CHSV(ahue, saturation, 255);
    }
}

void initLeds() {
    LEDS.setBrightness(LED_BRIGHTNESS_DEFAULT);
    LEDS.addLeds<LED_MODEL, STRIP_PIN, LED_RGB_MODE>(LED_STRIP, LED_COUNT);
    led_stamp = millis();
}

void setBrightness(byte brightness) {
    LEDS.setBrightness(brightness);
}

void clearSide() {
    switch (LED_SIDE) {
        case 1:
            for (int i = 4; i < LED_COUNT; ++i)
                LED_STRIP[i] = 0;
            break;
        case 2:
            for (int i = 0; i < 4; ++i)
                LED_STRIP[i] = 0;
            for (int i = 8; i < LED_COUNT; ++i)
                LED_STRIP[i] = 0;
            break;
        case 3:
            // one side unfortunately has one more led, because of sizes
            for (int i = 0; i < 8; ++i)
                LED_STRIP[i] = 0;
            for (int i = 13; i < LED_COUNT; ++i)
                LED_STRIP[i] = 0;
            break;
        case 4:
            for (int i = 0; i < 13; ++i)
                LED_STRIP[i] = 0;
            break;
        default:
            break;
    }
}

void setSide(byte side) {
    LED_SIDE = side;
    clearSide();
}

void setMode(int mode) {
    LED_MODE = mode;
    led_stamp = millis();
    counter = 0;
}

void setBreathing(bool mode, byte speed) {
    LED_BREATHING = mode;
    LED_BREATHING_SPEED = speed;
}

void ledTick() {
    switch (LED_MODE) {
        case 0:
            LEDS.clear();
            break;
        case 1:
            mode_colorCycle();
            break;
        case 2:
            mode_rainbow();
            break;
        case 3:
            mode_fire();
            break;
        default:
            break;
    }
    clearSide();
    LEDS.show();
}

void mode_fire(int huestart, byte saturation) {
    for (int i = 0; i < LED_COUNT; ++i) {
        int val = inoise8(i * 15, counter);
        LED_STRIP[i] = CHSV(
                huestart + map(val, 0, 255, 0, 21),                    // H
                constrain(map(val, 0, 255, saturation, 245), 0, 255),       // S
                constrain(map(val, 0, 255, 70, 255), 0, 255)  // V
        );
    }
    counter += 20;

}

void mode_rainbow(int speed) {
    for (int i = 0; i < LED_COUNT; i++) {
        LED_STRIP[i] = wheel(counter + i * 255 / LED_COUNT);   // counter смещает цвет
    }
    counter += speed;
    if (counter > 255)
        counter = 0;

}

void mode_colorCycle(byte saturation) {
    counter++;
    if (counter > 255)
        counter = 0;
    fillHSV(counter, saturation);

}

void mode_breathing(int speed) {
    static int dir = 1;
    static int bright = 0;
    bright += dir * speed;

    if (bright > 255) {
        bright = 255;
        dir = -1;
    }
    if (bright < 0) {
        bright = 0;
        dir = 1;
    }
    LEDS.setBrightness(bright);
}

#endif //DECORATION_LEDHANDLE_H
