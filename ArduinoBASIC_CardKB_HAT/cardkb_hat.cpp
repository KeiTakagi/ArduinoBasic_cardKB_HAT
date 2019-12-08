/*
    @file cardkb_hat.cpp
    @brief cardKeyboard

    @author Kei Takagi
    @date 2019.12.08

    Copyright (c) 2019 Kei Takagi
*/

#include <Adafruit_NeoPixel.h>
#include "cardkb_hat.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

uint8_t idle = 0;
uint8_t _shift = 0, _fn = 0;
uint8_t KEY = 0, hadPressed = 0;
uint8_t Mode = 0; //0->normal.1->shift 2->long_shift, 3->fn,4->long_fn

void flashOn(byte r, byte g, byte b) {
  pixels.setPixelColor(0, pixels.Color(r, g, b)); pixels.show();
}

void keybordSetup(void) {
  pinMode(A3, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A1, OUTPUT);
  pinMode(A0, OUTPUT);

  DDRB = 0x00;
  PORTB = 0xff;
  DDRD = 0x00;
  PORTD = 0xff;

  pixels.begin();
  flashOn(0, 0, 0);
}
byte getInput(uint8_t delay_time) {
  byte ret = 0;
  byte i, j;
  hadPressed = 1;
  for (i = 0; i < 4; i++) {
    digitalWrite(A3, (0b00001110 >> i) & 0b00000001);
    digitalWrite(A2, (0b00001101 >> i) & 0b00000001);
    digitalWrite(A1, (0b00001011 >> i) & 0b00000001);
    digitalWrite(A0, (0b00000111 >> i) & 0b00000001);

    for (j = 0; j < 8; j++) {
      ret++;
      if (PIND == pgm_read_byte(&pinDmap[j])) {
        flashOn(3, 3, 3);
        delay(delay_time);
        flashOn(0, 0, 0);
        return ret;
      }
    }
    for (j = 0; j < 7; j++) {
      ret++;
      if (PINB == pgm_read_byte(&pinBmap[j])) {
        flashOn(3, 3, 3);
        delay(delay_time);
        flashOn(0, 0, 0);
        return ret;
      }
    }
  }
  hadPressed = 0;
  return 0;
}

byte getChar(uint8_t delay_time)
{
  byte c = 0;
  if (KEY == 31 && shiftPressed ) {
    if (Mode > 0) {
      _shift = 0;
      Mode = 0;
      delay(200);
    } else {
      if (_shift < LONGPRESSEDTIME)_shift++;
      _fn = 0; idle = 0;
      return 0;
    }
  }
  if ( _shift == LONGPRESSEDTIME) {
    //long shift
    Mode = 2;
  } else if ( 0 < _shift) {
    // shift
    Mode = 1;
  }

  if (KEY == 46 && fnPressed) {
    if (Mode > 0) {
      _fn = 0;
      Mode = 0;
      delay(200);
    } else {
      if (_fn < LONGPRESSEDTIME)_fn++;
      _shift = 0; idle = 0;
      return 0;
    }
  }
  if ( _fn == LONGPRESSEDTIME) {
    //long fn
    Mode = 4;
  } else if ( 0 < _fn) {
    // fn
    Mode = 3;
  }

  switch (Mode) {
    case 0://normal
      flashOn(0, 0, 0); break;
    case 1://shift
      flashOn(4, 0, 0); break;
    case 2://long_shift
      if ( idle < 4) {
        flashOn(0, 0, 0);
      } else {
        flashOn(4, 0, 0);
      }
      break;
    case 3://fn
      flashOn(0, 0, 4); break;
    case 4://long_fn
      if ( idle < 4) {
        flashOn(0, 0, 0);
      } else {
        flashOn(0, 0, 4);
      }
      break;
  }

  if (hadPressed == 0) {
    KEY = getInput(delay_time);
    if (hadPressed == 1) {
      c = pgm_read_byte(&KeyMap[KEY - 1][Mode]);
      if ((Mode == 1) || (Mode == 3) ) {
        Mode = 0;
        _shift = 0;
        _fn = 0;
      }
      hadPressed = 0;
    }
  }
  idle < 8 ?  idle++ : idle = 0;
  return c;
}
