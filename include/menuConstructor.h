
#ifndef DECORATION_MENUCONSTRUCTOR_H
#define DECORATION_MENUCONSTRUCTOR_H

#include <Arduino.h>

#include <LiquidCrystal_I2C.h>
#include "define.h"

int menu_mode = 1; // 0 - temperature plotter, 1 - bin clock small, 2 - bin clock bigger, 3 - clock;

byte plotter_upper_limit = 32, plotter_bottom_limit = 20;
double plot_array[LCD_BORDER] = {(double)plotter_bottom_limit};

byte Year;
byte Month;
byte Date;
byte DoW;
byte Hour;
byte Minute;
byte Second;

LiquidCrystal_I2C lcd(0x27, LCD_WIDTH, LCD_HEIGHT);

void initBigDigits();

void initSmallDigits();

void initDots();

void initPlotterSymbols();

void menuInit();

void drawMenu(DateTime time);

void drawMenu();

void swap(char &a, char &b);

String toBin(int num, unsigned int size = 8);

void drawClock(byte start_point = 1, byte spacing = 0);

void dotsBlink(byte start_point);

void drawDig(byte dig, byte x, byte y);

void drawBinWDots(byte start_point = 0);

void drawBinWDigits(byte start_point = 0);

void addToPlot(double val);

void drawPlot(byte pos = 0, byte row = LCD_HEIGHT - 1, byte width = LCD_BORDER, byte height = LCD_HEIGHT);

// for usual clock ......................................................................................................
void initBigDigits() {
    uint8_t LT[8] = {0b00111, 0b01111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
    uint8_t UB[8] = {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000};
    uint8_t RT[8] = {0b11100, 0b11110, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
    uint8_t LL[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b01111, 0b00111};
    uint8_t LB[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111};
    uint8_t LR[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11110, 0b11100};
    uint8_t UMB[8] = {0b11111, 0b11111, 0b11111, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111};
    uint8_t LMB[8] = {0b11111, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111};
    lcd.createChar(0, LT);
    lcd.createChar(1, UB);
    lcd.createChar(2, RT);
    lcd.createChar(3, LL);
    lcd.createChar(4, LB);
    lcd.createChar(5, LR);
    lcd.createChar(6, UMB);
    lcd.createChar(7, LMB);
}

void drawDig(byte dig, byte x, byte y) {
    switch (dig) {
        case 0:
            lcd.setCursor(x, y); // set cursor to column 0, line 0 (first row)
            lcd.write(0);  // call each segment to create
            lcd.write(1);  // top half of the number
            lcd.write(2);
            lcd.setCursor(x, y + 1); // set cursor to colum 0, line 1 (Secondond row)
            lcd.write(3);  // call each segment to create
            lcd.write(4);  // bottom half of the number
            lcd.write(5);
            break;
        case 1:
            lcd.setCursor(x + 1, y);
            lcd.write(1);
            lcd.write(2);
            lcd.setCursor(x + 2, y + 1);
            lcd.write(5);
            break;
        case 2:
            lcd.setCursor(x, y);
            lcd.write(6);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(3);
            lcd.write(7);
            lcd.write(7);
            break;
        case 3:
            lcd.setCursor(x, y);
            lcd.write(6);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(7);
            lcd.write(7);
            lcd.write(5);
            break;
        case 4:
            lcd.setCursor(x, y);
            lcd.write(3);
            lcd.write(4);
            lcd.write(2);
            lcd.setCursor(x + 2, y + 1);
            lcd.write(5);
            break;
        case 5:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(6);
            lcd.setCursor(x, y + 1);
            lcd.write(7);
            lcd.write(7);
            lcd.write(5);
            break;
        case 6:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(6);
            lcd.setCursor(x, y + 1);
            lcd.write(3);
            lcd.write(7);
            lcd.write(5);
            break;
        case 7:
            lcd.setCursor(x, y);
            lcd.write(1);
            lcd.write(1);
            lcd.write(2);
            lcd.setCursor(x + 1, y + 1);
            lcd.write(0);
            break;
        case 8:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x, y + 1);
            lcd.write(3);
            lcd.write(7);
            lcd.write(5);
            break;
        case 9:
            lcd.setCursor(x, y);
            lcd.write(0);
            lcd.write(6);
            lcd.write(2);
            lcd.setCursor(x + 1, y + 1);
            lcd.write(4);
            lcd.write(5);
            break;
        case 10:
            lcd.setCursor(x, y);
            lcd.write(32);
            lcd.write(32);
            lcd.write(32);
            lcd.setCursor(x, y + 1);
            lcd.write(32);
            lcd.write(32);
            lcd.write(32);
            break;
        default:
            break;
    }
}

void dotsBlink(byte start_point) {
    static bool blink_ = false;
    blink_ = !blink_;

    byte code = blink_ ? 165 : 32;
    lcd.setCursor(start_point, 0);
    lcd.write(code);
    lcd.setCursor(start_point, 1);
    lcd.write(code);
}

void drawClock(byte start_point, byte spacing) {
    lcd.setCursor(start_point, 0);
    lcd.print("             ");
    lcd.setCursor(start_point, 1);
    lcd.print("             ");

    byte x = 0 + start_point, y = 0;
    if (Hour == 10) drawDig(1, x, y);
    else if (Hour / 10 == 0) drawDig(0, x, y);
    else drawDig(Hour / 10, x, y);
    x += 3 + spacing; // digit length + spacing
    drawDig(Hour % 10, x, y);

    x += 3 + 1;
    // blink

    drawDig(Minute / 10, x, y);
    x += 3 + spacing;
    drawDig(Minute % 10, x, y);
}

// for bin clock ........................................................................................................
void swap(char &a, char &b) {
    char c = a;
    a = b;
    b = c;
}

void initSmallDigits() {
    byte u00[] = {B00100, B01010, B01010, B00100, B00000, B00000, B00100, B01010};
    byte u10[] = {B00100, B01100, B00100, B00100, B00000, B00000, B00100, B01010};
    byte u01[] = {B00100, B01010, B01010, B00100, B00000, B00000, B00100, B01100};
    byte u11[] = {B00100, B01100, B00100, B00100, B00000, B00000, B00100, B01100};
    byte d00[] = {B01010, B00100, B00000, B00000, B00100, B01010, B01010, B00100};
    byte d01[] = {B01010, B00100, B00000, B00000, B00100, B01100, B00100, B00100};
    byte d10[] = {B00100, B00100, B00000, B00000, B00100, B01010, B01010, B00100};
    byte d11[] = {B00100, B00100, B00000, B00000, B00100, B01100, B00100, B00100};
    lcd.createChar(0, u00);
    lcd.createChar(1, u01);
    lcd.createChar(2, u10);
    lcd.createChar(3, u11);
    lcd.createChar(4, d00);
    lcd.createChar(5, d01);
    lcd.createChar(6, d10);
    lcd.createChar(7, d11);
}

void initDots() {
    byte zero[] = {B01110, B10001, B10001, B10001, B10001, B10001, B10001, B01110};
    byte one[] = {B01110, B11111, B11111, B11111, B11111, B11111, B11111, B01110};
    lcd.createChar(0, zero);
    lcd.createChar(1, one);
}

String toBin(int num, unsigned int size) {
    String output;
    while (num != 0) {
//        strcat(output, itoa(num % 2, nullptr, DEC));
        output += String(num % 2);
        num /= 2;
    }
    unsigned int m_size = output.length();
    for (unsigned int i = m_size; i < size; ++i)
        output += "0";
    for (unsigned int i = 0; i < size / 2; ++i)
        swap(output[i], output[size - 1 - i]);

    return output;
}

void drawBinWDots(byte start_point) {
    lcd.setCursor(start_point, 0);
    lcd.print("        ");
    lcd.setCursor(start_point, 1);
    lcd.print("        ");

    String Hour_s = toBin(Hour);
    String Minute_s = toBin(Minute);
    for (unsigned int i = 0; i < Hour_s.length(); ++i) {
        byte symbol = Hour_s[i] - '0';
        lcd.setCursor(i + start_point, 0);
        lcd.write(symbol);

        symbol = Minute_s[i] - '0';
        lcd.setCursor(i + start_point, 1);
        lcd.write(symbol);
    }
}

void drawBinWDigits(byte start_point) {
    lcd.setCursor(start_point, 0);
    lcd.print("        ");
    lcd.setCursor(start_point, 1);
    lcd.print("        ");

    String Hour_s = toBin(Hour);
    String Minute_s = toBin(Minute);
    String Second_s = toBin(Second);
    for (unsigned int i = 0; i < Hour_s.length(); ++i) {
        byte symbol = 0;
        lcd.setCursor(i + start_point, 0);
        if (Hour_s[i] == '1') symbol += 2;
        if (Minute_s[i] == '1') symbol += 1;
        lcd.write(symbol);

        symbol = 4;
        lcd.setCursor(i + start_point, 1);
        if (Minute_s[i] == '1') symbol += 2;
        if (Second_s[i] == '1') symbol += 1;
        lcd.write(symbol);
    }
}

// for plotter by AlexGyver .............................................................................................
void initPlotterSymbols() {
    // made in http://maxpromer.github.io/LCD-Character-Creator/
    byte row8[8] = {0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
    byte row7[8] = {0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
    byte row6[8] = {0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
    byte row5[8] = {0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111, 0b11111};
    byte row4[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111, 0b11111};
    byte row3[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111, 0b11111};
    byte row2[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111, 0b11111};
    byte row1[8] = {0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b00000, 0b11111};
    lcd.createChar(0, row8);
    lcd.createChar(1, row1);
    lcd.createChar(2, row2);
    lcd.createChar(3, row3);
    lcd.createChar(4, row4);
    lcd.createChar(5, row5);
    lcd.createChar(6, row6);
    lcd.createChar(7, row7);
}

void drawPlot(byte pos, byte row, byte width, byte height) {
    for (byte i = 0; i < width; i++) {                  // каждый столбец параметров
        byte infill, fract;
        // найти количество целых блоков с учЄтом минимума и максимума дл€ отображени€ на графике
        infill = floor((double) (plot_array[i] - plotter_bottom_limit) / (plotter_upper_limit - plotter_bottom_limit) * height * 10);
        fract = (infill % 10) * 8 / 10;                   // найти количество оставшихс€ полосок
        infill = infill / 10;
        for (byte n = 0; n < height; n++) {     // дл€ всех строк графика
            if (n < infill && infill > 0) {       // пока мы ниже уровн€
                lcd.setCursor(pos + i, (row - n));        // заполн€ем полными €чейками
                lcd.write(0);
            }
            if (n >= infill) {                    // если достигли уровн€
                lcd.setCursor(pos + i, (row - n));
                if (fract > 0) lcd.write(fract);          // заполн€ем дробные €чейки
                else lcd.write(16);                       // если дробные == 0, заливаем пустой
                for (byte k = n + 1; k < height; k++) {   // всЄ что сверху заливаем пустыми
                    lcd.setCursor(pos + i, (row - k));
                    lcd.write(16);
                }
                break;
            }
        }
    }
}

void addToPlot(double val) {
    for (byte i = 0; i < LCD_BORDER - 1; i++) {
        plot_array[i] = plot_array[i + 1];
    }
    val = constrain(val, plotter_bottom_limit, plotter_upper_limit);
    plot_array[LCD_BORDER - 1] = val;
}
//

void menuInit() {
    switch (menu_mode) {
        case 0:
            initPlotterSymbols();
            break;
        case 1:
            initSmallDigits();
            break;
        case 2:
            initDots();
            break;
        case 3:
            initBigDigits();
            break;
        default:
            break;
    }
}

void drawMenu() {
    switch (menu_mode) {
        case 0:
            drawPlot();
            break;
        case 1:
            drawBinWDigits();
            break;
        case 2:
            drawBinWDots();
            break;
        case 3:
            drawClock();
            break;
        default:
            break;
    }
}

void drawMenu(DateTime time) {
    if (menu_mode == 1 && time.second() != Second) { // draw every second
        Hour = time.hour();
        Minute = time.minute();
        Second = time.second();
        drawBinWDigits();
    } else if (menu_mode != 0 && time.minute() != Minute) { // draw every minute
        Hour = time.hour();
        Minute = time.minute();
        if (menu_mode == 2)
            drawBinWDots();
        else if (menu_mode == 3)
            drawClock();
    } else if (menu_mode == 0) {
        Hour = time.hour();
        Minute = time.minute();
    }
}
#endif //DECORATION_MENUCONSTRUCTOR_H
