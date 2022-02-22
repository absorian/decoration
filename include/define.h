
#ifndef DECORATION_DEFINE_H
#define DECORATION_DEFINE_H

#define PREVIEW 1 // plot preview

#define LCD_HEIGHT 2 // display's height
#define LCD_WIDTH 16 // display's width
#define LCD_BORDER 9 // border between plot and text information | LCD_WIDTH - LCD_BORDER = should be enough for text

#define BUTTON1_PIN 4 // pin for button-up
#define BUTTON2_PIN 5 // pin for button-down
#define BUTTON3_PIN 6 // pin for button-set
#define STRIP_PIN 2 // pin for led strip
#define THERM_PIN A0 // thermistor's pin

#define B 3940 // beta constant
#define SERIAL_R 9710.0 // resistor's R (Ohm)
#define THERMISTOR_R 9820.0 // thermistor's R (Ohm) at NOMINAL_T
#define NOMINAL_T 25 // temperature (C) when THERMISTOR_R was found

#if !PREVIEW
#define DELAY_PLOTTER 7200000 // 2 hour delay for refreshing plotter
#else
#define DELAY_PLOTTER 10000 // 10 sec delay for plotter preview
#endif
#define DELAY_T_REFRESH 500 // delay for updating temperature
#define SETTINGS_TIMEOUT 10000 // timeout for settings

#define LED_COUNT 17 // led count in strip
#define LED_BRIGHTNESS_DEFAULT 150 // default led's brightness byte
#define LED_MODEL WS2811 // led's chip model
#define LED_RGB_MODE GRB // led's rgb mode

#endif //DECORATION_DEFINE_H
