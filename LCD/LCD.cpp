/* mbed LCD Library, for a 4-bit LCD based on HD44780
 * Copyright (c) 2007-2010, hb9gaa
 *
 */

#include "LCD.h"
#include "mbed.h"

TextLCD::TextLCD(PinName rs, PinName rw, PinName e, PinName d0, PinName d1,
                 PinName d2, PinName d3, LCDType type) : _rs(rs), _rw(rw),
        _e(e), _d(d0, d1, d2, d3),
        _type(type) {

    _e  = 1;
    _rw = 0;
    _rs = 0;            // command mode

     wait(0.015);        // Wait 15ms to ensure powered up

    // send "Display Settings" 3 times (Only top nibble of 0x30 as we've got 4-bit bus)
    for (int i=0; i<3; i++) {
        writeByte(0x3);
        wait(0.00164);  // this command takes 1.64ms, so wait for it
    }
    writeByte(0x2);     // 4-bit mode
    wait(0.0001f);    // most instructions take 100us

    writeCommand(0x28); // Function set 001 BW N F - -
    writeCommand(0x0C);
    writeCommand(0x6);  // Cursor Direction and Display Shift : 0000 01 CD S (CD 0-left, 1-right S(hift) 0-no, 1-yes
    cls();
}

void TextLCD::character(int column, int row, int c) {
    int a = address(column, row);
    writeCommand(a);
    writeData(c);
}

void TextLCD::cls() {
    writeCommand(0x01); // cls, and set cursor to 0
    wait(0.004100f);    // This command takes 4.1 ms
    locate(0, 0);
}

void TextLCD::locate(int row, int column) {
    _row = row;
    _column = column;
}

int TextLCD::_putc(int value) {
    if (value == '\n') {
        _column = 0;
        _row++;
        if (_row >= rows()) {
            _row = 0;
        }
    } else {
        character(_column, _row, value);
        _column++;
        if (_column >= columns()) {
            _column = 0;
            _row++;
            if (_row >= rows()) {
                _row = 0;
            }
        }
    }
    return value;
}

int TextLCD::_getc() {
    return -1;
}

void TextLCD::writeByte(int value) {
    _d = value >> 4;
    wait(0.0001); // most instructions take 100us
    _e = 0;
    wait(0.0001);
    _e = 1;
    _d = value >> 0;
    wait(0.0001);
    _e = 0;
    wait(0.0001);  // most instructions take 100us
    _e = 1;
}

void TextLCD::writeCommand(int command) {
    _rs = 0;
    writeByte(command);
}

void TextLCD::writeData(int data) {
    _rs = 1;
    writeByte(data);
}

int TextLCD::address(int column, int row) {
    switch (_type) {
        case LCD20x4:
            switch (row) {
                case 0:
                    return 0x80 + column;
                case 1:
                    return 0xc0 + column;
                case 2:
                    return 0x94 + column;
                case 3:
                    return 0xd4 + column;
            }
        case LCD16x2B:
            return 0x80 + (row * 40) + column;
        case LCD16x2:
        case LCD20x2:
        default:
            return 0x80 + (row * 0x40) + column;
    }
}

int TextLCD::columns() {
    switch (_type) {
        case LCD20x4:
        case LCD20x2:
            return 20;
        case LCD16x2:
        case LCD16x2B:
        default:
            return 16;
    }
}

int TextLCD::rows() {
    switch (_type) {
        case LCD20x4:
            return 4;
        case LCD16x2:
        case LCD16x2B:
        case LCD20x2:
        default:
            return 2;
    }
}