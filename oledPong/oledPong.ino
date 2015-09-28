/*
The MIT License (MIT)

Copyright (c) 2015 Martin Hudoba

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include "U8glib.h"

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);	// Fast I2C / TWI

#define START 1
#define UP_RIGHT 2
#define UP_LEFT 3
#define DOWN_RIGHT 4
#define DOWN_LEFT 5
#define UP_RIGHTS 6
#define UP_LEFTS 7
#define DOWN_RIGHTS 8
#define DOWN_LEFTS 9

#define RIGHT_WALL 1
#define LEFT_WALL 2
#define UPPER_WALL 3
#define BOTTOM_WALL 4
#define PADDLE_REND 5
#define PADDLE_RMID 6
#define PADDLE_LEND 7
#define PADDLE_LMID 8

#define SPD0 0
#define SPD1 48

#define PADDLE_XL 0
#define PADDLE_XR 126

#define JOYPIN_L 2
#define JOYPIN_R 0

int16_t joyYL;
int16_t joyYR;
int16_t paddleL;
int16_t paddleR;
uint8_t movementVal;
uint16_t scoreR;
uint16_t scoreL;
uint8_t bodX;
uint8_t bodY;
uint8_t lastBodX = 64;
uint8_t lastBodY = 32;
uint8_t direct = START;
uint8_t lastDirection;
uint8_t rndDirection;

void assignDirection(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h) {
  movementVal = 0;
  lastDirection = direct;
  lastBodX = bodX;
  lastBodY = bodY;
   if (lastDirection == a) {direct = b;}
  else if (lastDirection == c) {direct = d;}
  else if (lastDirection == e) {direct = f;}
  else if (lastDirection == g) {direct = h;}

  if (direct == START) {
    if (rndDirection == UP_RIGHT) {direct = DOWN_RIGHT;}
    else if (rndDirection == UP_LEFT) {direct = DOWN_LEFT;}
    else if (rndDirection == DOWN_RIGHT) {direct = UP_RIGHT;}
    else if (rndDirection == DOWN_LEFT) {direct = UP_LEFT;}
  }
}

void movement(uint8_t i, uint8_t num) {
  if (direct == START) {
    lastBodX = 64;
    lastBodY = 32;
  }
  if (i == UP_RIGHT) {
    bodX = lastBodX + map(movementVal, SPD0, SPD1, 0, 128);
    bodY = lastBodY - map(movementVal, SPD0, SPD1, 0, num);
  } else if (i == UP_LEFT) {
    bodX = lastBodX - map(movementVal, SPD0, SPD1, 0, 128);
    bodY = lastBodY - map(movementVal, SPD0, SPD1, 0, num);
  } else if (i == DOWN_RIGHT) {
    bodX = lastBodX + map(movementVal, SPD0, SPD1, 0, 128);
    bodY = lastBodY + map(movementVal, SPD0, SPD1, 0, num);
  } else if (i == DOWN_LEFT) {
    bodX = lastBodX - map(movementVal, SPD0, SPD1, 0, 128);
    bodY = lastBodY + map(movementVal, SPD0, SPD1, 0, num);
  }
  u8g.drawPixel(bodX, bodY);
  checkCollision();
}

void checkCollision() {
  if ( ((bodY >= paddleR) && (bodY <= paddleR + 2)) && (bodX >= PADDLE_XR)) { //Right Paddle TOP
    assignDirection(UP_RIGHT, UP_LEFT, UP_RIGHTS, UP_LEFT, DOWN_RIGHT, DOWN_LEFT, DOWN_RIGHTS, DOWN_LEFT);
  } else if ( ((bodY >= paddleR + 3) && (bodY <= paddleR + 3)) && (bodX >= PADDLE_XR)) { //Right Paddle TOP-MIDDLE
    assignDirection(UP_RIGHT, UP_LEFTS, UP_RIGHTS, UP_LEFTS, DOWN_RIGHT, DOWN_LEFTS, DOWN_RIGHTS, DOWN_LEFTS);
  } else if ( ((bodY >= paddleR + 4) && (bodY <= paddleR + 5)) && (bodX >= PADDLE_XR)) { //Right Paddle BOTTOM
    assignDirection(UP_RIGHT, UP_LEFT, UP_RIGHTS, UP_LEFT, DOWN_RIGHT, DOWN_LEFT, DOWN_RIGHTS, DOWN_LEFT);
  } else if ( ((bodY >= paddleL) && (bodY <= paddleL + 2)) && ((bodX >= PADDLE_XL + 1) && (bodX <= PADDLE_XL + 3)) ) { //Left Paddle TOP
    assignDirection(UP_LEFT, UP_RIGHT, UP_LEFTS, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, DOWN_LEFTS, DOWN_RIGHT);
  } else if ( ((bodY >= paddleL + 3) && (bodY <= paddleL + 3)) && ((bodX >= PADDLE_XL + 1) && (bodX <= PADDLE_XL + 3)) ) { //Left Paddle TOP-MIDDLE
    assignDirection(UP_LEFT, UP_RIGHTS, UP_LEFTS, UP_RIGHTS, DOWN_LEFT, DOWN_RIGHTS, DOWN_LEFTS, DOWN_RIGHTS);
  } else if ( ((bodY >= paddleL + 4) && (bodY <= paddleL + 5)) && ((bodX >= PADDLE_XL + 1) && (bodX <= PADDLE_XL + 3)) ) { //Left Paddle BOTTOM
    assignDirection(UP_LEFT, UP_RIGHT, UP_LEFTS, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, DOWN_LEFTS, DOWN_RIGHT);
  } else if (bodX >= 128) { //Right wall
    rndDirection = random(2, 6);
    direct = START;
    movementVal = 0;
    lastBodX = bodX;
    lastBodY = bodY;
    scoreL++;
  } else if (bodX <= 0) { //Left wall
    rndDirection = random(2, 6);
    direct = START;
    movementVal = 0;
    lastBodX = bodX;
    lastBodY = bodY;
    scoreR++;
  } else if (bodY <= 0) { //Upper wall
    bodY = 0;
    assignDirection(UP_RIGHT, DOWN_RIGHT, UP_LEFT, DOWN_LEFT, UP_RIGHTS, DOWN_RIGHTS, UP_LEFTS, DOWN_LEFTS);
  } else if (bodY >= 64) { //Bottom wall
    bodY = 64;
    assignDirection(DOWN_RIGHT, UP_RIGHT, DOWN_LEFT, UP_LEFT, DOWN_RIGHTS, UP_RIGHTS, DOWN_LEFTS, UP_LEFTS);
  }
}

void draw(void) {
  if (direct == START) {
      if (rndDirection == UP_RIGHT) {movement(UP_RIGHT, 128);}
      else if (rndDirection == UP_LEFT) {movement(UP_LEFT, 128);}
      else if (rndDirection == DOWN_RIGHT) {movement(DOWN_RIGHT, 128);}
      else if (rndDirection == DOWN_LEFT) {movement(DOWN_LEFT, 128);
      }    
  } else if (direct == UP_RIGHT) {movement(UP_RIGHT, 64);}
    else if (direct == UP_LEFT) {movement(UP_LEFT, 64);}
    else if (direct == DOWN_RIGHT) {movement(DOWN_RIGHT, 64);}
    else if (direct == DOWN_LEFT) {movement(DOWN_LEFT, 64);}
    else if (direct == UP_RIGHTS) {movement(UP_RIGHT, 16);}
    else if (direct == UP_LEFTS) {movement(UP_LEFT, 16);}
    else if (direct == DOWN_RIGHTS) {movement(DOWN_RIGHT, 16);}
    else if (direct == DOWN_LEFTS) {movement(DOWN_LEFT, 16);
  }

  checkCollision();
  u8g.setFont(u8g_font_helvB08);
  /*u8g.setPrintPos(27, 35);
  u8g.print(direct);*/

  u8g.setPrintPos(32, 15);
  u8g.print(scoreL);
  u8g.setPrintPos(80, 15);
  u8g.print(scoreR);

  u8g.drawPixel(bodX, bodY);
  u8g.drawBox(64, 0, 1, 64);
  u8g.drawBox(PADDLE_XL, paddleL, 2, 5);
  u8g.drawBox(PADDLE_XR, paddleR, 2, 5);
}

void setup() {
  randomSeed(analogRead(3));
}

void loop() {
  int16_t joyL = analogRead(JOYPIN_L);
  int16_t joyR = analogRead(JOYPIN_R);
  paddleL = map(joyL, 0, 1023, 0, 59);
  paddleR = map(joyR, 0, 1023, 0, 59);

  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );

  movementVal++;
}
