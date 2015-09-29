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

#define PADDLE_XL 0
#define PADDLE_XR 126

#define SPD0 0
#define SPD1 48 //Ball speed - the smaller this number is, the faster the ball goes

#define JOYPIN_L 2 //Analog pin of the left joystick
#define JOYPIN_R 0 //Analog pin of the right joystick

int16_t joyYL;
int16_t joyYR;
int16_t paddleL;
int16_t paddleR;
uint8_t movementVal;
uint16_t scoreR;
uint16_t scoreL;
uint8_t ballX;
uint8_t ballY;
uint8_t lastballX = 64;
uint8_t lastballY = 32;
uint8_t direct = START;
uint8_t lastDirection;
uint8_t rndDirection;

void assignDirection(uint8_t a, uint8_t b, uint8_t c, uint8_t d, uint8_t e, uint8_t f, uint8_t g, uint8_t h) {
  movementVal = 0;
  lastDirection = direct;
  lastballX = ballX;
  lastballY = ballY;
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
    lastballX = 64;
    lastballY = 32;
  }
  if (i == UP_RIGHT) {
    ballX = lastballX + map(movementVal, SPD0, SPD1, 0, 128);
    ballY = lastballY - map(movementVal, SPD0, SPD1, 0, num);
  } else if (i == UP_LEFT) {
    ballX = lastballX - map(movementVal, SPD0, SPD1, 0, 128);
    ballY = lastballY - map(movementVal, SPD0, SPD1, 0, num);
  } else if (i == DOWN_RIGHT) {
    ballX = lastballX + map(movementVal, SPD0, SPD1, 0, 128);
    ballY = lastballY + map(movementVal, SPD0, SPD1, 0, num);
  } else if (i == DOWN_LEFT) {
    ballX = lastballX - map(movementVal, SPD0, SPD1, 0, 128);
    ballY = lastballY + map(movementVal, SPD0, SPD1, 0, num);
  }
}

void checkCollision() {
  if ( ((ballY >= paddleR) && (ballY <= paddleR + 2)) && (ballX >= PADDLE_XR)) { //Right Paddle TOP
    assignDirection(UP_RIGHT, UP_LEFT, UP_RIGHTS, UP_LEFT, DOWN_RIGHT, DOWN_LEFT, DOWN_RIGHTS, DOWN_LEFT);
  } else if ( ((ballY >= paddleR + 3) && (ballY <= paddleR + 3)) && (ballX >= PADDLE_XR)) { //Right Paddle TOP-MIDDLE
    assignDirection(UP_RIGHT, UP_LEFTS, UP_RIGHTS, UP_LEFTS, DOWN_RIGHT, DOWN_LEFTS, DOWN_RIGHTS, DOWN_LEFTS);
  } else if ( ((ballY >= paddleR + 4) && (ballY <= paddleR + 5)) && (ballX >= PADDLE_XR)) { //Right Paddle BOTTOM
    assignDirection(UP_RIGHT, UP_LEFT, UP_RIGHTS, UP_LEFT, DOWN_RIGHT, DOWN_LEFT, DOWN_RIGHTS, DOWN_LEFT);
  } else if ( ((ballY >= paddleL) && (ballY <= paddleL + 2)) && (ballX <= PADDLE_XL + 2)) { //Left Paddle TOP
    assignDirection(UP_LEFT, UP_RIGHT, UP_LEFTS, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, DOWN_LEFTS, DOWN_RIGHT);
  } else if ( ((ballY >= paddleL + 3) && (ballY <= paddleL + 3)) && (ballX <= PADDLE_XL + 2)) { //Left Paddle TOP-MIDDLE
    assignDirection(UP_LEFT, UP_RIGHTS, UP_LEFTS, UP_RIGHTS, DOWN_LEFT, DOWN_RIGHTS, DOWN_LEFTS, DOWN_RIGHTS);
  } else if ( ((ballY >= paddleL + 4) && (ballY <= paddleL + 5)) && (ballX <= PADDLE_XL + 2)) { //Left Paddle BOTTOM
    assignDirection(UP_LEFT, UP_RIGHT, UP_LEFTS, UP_RIGHT, DOWN_LEFT, DOWN_RIGHT, DOWN_LEFTS, DOWN_RIGHT);
  } else if (ballX >= 128) { //Right wall
    rndDirection = random(2, 6);
    direct = START;
    movementVal = 0;
    lastballX = ballX;
    lastballY = ballY;
    scoreL++;
  } else if (ballX <= 0) { //Left wall
    rndDirection = random(2, 6);
    direct = START;
    movementVal = 0;
    lastballX = ballX;
    lastballY = ballY;
    scoreR++;
  } else if (ballY <= 1) { //Upper wall
    assignDirection(UP_RIGHT, DOWN_RIGHT, UP_LEFT, DOWN_LEFT, UP_RIGHTS, DOWN_RIGHTS, UP_LEFTS, DOWN_LEFTS);
  } else if (ballY >= 64) { //Bottom wall
    assignDirection(DOWN_RIGHT, UP_RIGHT, DOWN_LEFT, UP_LEFT, DOWN_RIGHTS, UP_RIGHTS, DOWN_LEFTS, UP_LEFTS);
  }
}

void draw(void) {  
  u8g.setFont(u8g_font_helvB08);
  u8g.drawPixel(ballX, ballY);
  u8g.drawBox(64, 0, 1, 64);
  u8g.drawBox(PADDLE_XL, paddleL, 2, 5);
  u8g.drawBox(PADDLE_XR, paddleR, 2, 5);
  /*u8g.setPrintPos(27, 35);
  u8g.print(ballY);*/
  
  if (scoreL <= 9) {u8g.setPrintPos(27, 15);}
  else if (scoreL >= 10 && scoreL <= 99){u8g.setPrintPos(24, 15);}
  else if (scoreL >= 100 && scoreL <= 999) {u8g.setPrintPos(21, 15);}
  else if (scoreL >= 1000 && scoreL <= 9999){u8g.setPrintPos(18, 15);}
  else if (scoreL >= 10000 && scoreL <= 99999){u8g.setPrintPos(15, 15);}  
  u8g.print(scoreL);
  
  if (scoreR <= 9) {u8g.setPrintPos(98, 15);}
  else if (scoreR >= 10 && scoreR <= 99){u8g.setPrintPos(95, 15);}
  else if (scoreR >= 100 && scoreR <= 999) {u8g.setPrintPos(92, 15);}
  else if (scoreR >= 1000 && scoreR <= 9999){u8g.setPrintPos(89, 15);}
  else if (scoreR >= 10000 && scoreR <= 99999){u8g.setPrintPos(86, 15);}
  u8g.print(scoreR);
}

void setup() {
  randomSeed(analogRead(3));
}

void loop() {
  int16_t joyL = analogRead(JOYPIN_L);
  int16_t joyR = analogRead(JOYPIN_R);
  paddleL = map(joyL, 0, 1023, 0, 59);
  paddleR = map(joyR, 0, 1023, 0, 59);
  
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
  
  u8g.firstPage();
  do {
    draw();
  } while ( u8g.nextPage() );
  
  checkCollision();
  movementVal++;
}
