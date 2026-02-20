/******************************************************************************
**************************Hardware interface layer*****************************
* | file          :   DEV_Config.
* | version     :   V1.0
* | date        :   2017-12-11
* | function    :
    Provide the hardware underlying interface
******************************************************************************/
#include "DEV_Config.h"

#ifdef ARDUINO
#include <SPI.h>

/********************************************************************************
  function:    System Init and exit
  note:
    Initialize the communication method
********************************************************************************/
uint8_t System_Init(void) {
  // set pin
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RST, OUTPUT);
  pinMode(LCD_DC, OUTPUT);
  pinMode(LCD_BL, OUTPUT);

  // Touch
  pinMode(TP_CS, OUTPUT);
  pinMode(TP_IRQ, INPUT);
  pinMode(TP_BUSY, INPUT);

  // set Serial
  Serial.begin(115200);

  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.begin();

  return 0;
}

void PWM_SetValue(uint16_t value) { analogWrite(LCD_BL, value); }

/********************************************************************************
  function:    Hardware interface
  note:
    SPI4W_Write_Byte(value) :
        hardware SPI
    I2C_Write_Byte(value, cmd):
        hardware I2C
********************************************************************************/
void SPI4W_Write_Byte(uint8_t DATA) { SPI.transfer(DATA); }
uint8_t SPI4W_Read_Byte(uint8_t DATA) { return SPI.transfer(DATA); }

/********************************************************************************
  function:    Delay function
  note:
    Driver_Delay_ms(xms) : Delay x ms
    Driver_Delay_us(xus) : Delay x us
********************************************************************************/
void Driver_Delay_ms(unsigned long xms) { delay(xms); }

void Driver_Delay_us(int xus) { delayMicroseconds(xus); }

#else // PC implementation
#include <SDL2/SDL.h>
#include <chrono>
#include <stdio.h>
#include <thread>
#include <unistd.h>

static bool s_fastMode = false;

void System_SetFastMode(bool fast) { s_fastMode = fast; }

uint8_t System_Init(void) {
  // Already handled in LCD_Init for SDL, or we can do console init here
  return 0;
}

void PWM_SetValue(uint16_t value) {
  // PC brightness control - no-op or adjust gamma?
}

void SPI4W_Write_Byte(uint8_t DATA) {
  // No-op
}

uint8_t SPI4W_Read_Byte(uint8_t DATA) { return 0; }

void Driver_Delay_ms(unsigned long xms) {
  if (s_fastMode)
    return;
  SDL_Delay(xms);
}

void Driver_Delay_us(int xus) {
  if (s_fastMode)
    return;
  std::this_thread::sleep_for(std::chrono::microseconds(xus));
}

#endif
