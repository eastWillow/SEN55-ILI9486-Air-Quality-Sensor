#include "App.h"
#include "../Display/DEV_Config.h"
#include "../Display/LCD_Driver.h"
#include "../Display/LCD_GUI.h"
#include "../Display/LCD_Touch.h"
#include <stdio.h>

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <cmath>
#include <cstring>
#include <string>
using namespace std;
// Define String class for PC if needed, or use std::string and helper
// But the original code uses Arduino String.
// Let's implement a minimal String helper or use std::string and adapt.
#define String std::to_string
#endif

// Helper for PC compatibility with Arduino String
#ifndef ARDUINO
// A simple adapter if needed, or just change logic to use snprintf
#endif

// Application State
static AppState currentState = APP_STATE_MAIN;
static unsigned long lastSensorUpdate = 0;
static unsigned long lastTransitionTime = (unsigned long)-200;

#ifndef ARDUINO
unsigned long mockMillis = 0;
#endif

void App_ResetState() {
  currentState = APP_STATE_MAIN;
  lastSensorUpdate = 0;
  lastTransitionTime = (unsigned long)-200;
#ifndef ARDUINO
  mockMillis = 0;
#endif
}

AppState App_GetState() { return currentState; }

// 輔助函式：在指定位置顯示標籤與數值
static void displayValue(uint16_t x, uint16_t y, const char *label, float value,
                         const char *unit, uint16_t color) {
  // 1. 顯示標籤 (標籤通常固定不變，直接畫即可)
  GUI_DisString_EN(x, y, label, &Font20, LCD_BACKGROUND, BLACK);

  // 2. 格式化數值
  char valStr[64];
  snprintf(valStr, sizeof(valStr), "%.1f%s", value, unit);

  // ---------------------------------------------------------
  // 修改重點：在顯示新數值前，先畫一個背景色的實心矩形清除舊內容
  // ---------------------------------------------------------

  // 設定數值的起始 X 座標 (與下方顯示數值的座標一致)
  uint16_t valX = x + 150;

  // 設定要清除的寬度 (例如 120 pixels，依據數值長度調整)
  // 如果數值包含單位很長，可能需要加大此數值
  uint16_t clearWidth = 150;

  // 簡單的邊界檢查，防止超出螢幕邊緣 (480) 導致函式報錯
  if (valX + clearWidth > 480) {
    clearWidth = 480 - valX;
  }

  // 畫實心矩形 (LCD_BACKGROUND = 白色, DRAW_FULL = 實心填滿)
  // 高度設為 20 (配合 Font20)
  GUI_DrawRectangle(valX, y, valX + clearWidth, y + 20, LCD_BACKGROUND,
                    DRAW_FULL, DOT_PIXEL_DFT);

  // ---------------------------------------------------------

  // 3. 顯示新數值
  GUI_DisString_EN(valX, y, valStr, &Font20, LCD_BACKGROUND, color);
}

void DrawMainScreen() {
  LCD_Clear(LCD_BACKGROUND);
  // 顯示標題
  GUI_DisString_EN(10, 10, "SEN55 Air Quality", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DrawLine(0, 40, 480, 40, BLUE, LINE_SOLID, DOT_PIXEL_2X2);

  // Draw Info Button
  GUI_DrawRectangle(BTN_INFO_X, BTN_INFO_Y, BTN_INFO_X + BTN_INFO_W,
                    BTN_INFO_Y + BTN_INFO_H, BLUE, DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DisString_EN(BTN_INFO_X + 10, BTN_INFO_Y + 8, "INFO", &Font16,
                   LCD_BACKGROUND, BLUE);
}

void DrawInfoScreen() {
  LCD_Clear(LCD_BACKGROUND);

  GUI_DisString_EN(10, 10, "Information", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DrawLine(0, 40, 480, 40, BLUE, LINE_SOLID, DOT_PIXEL_2X2);

  GUI_DisString_EN(10, 60, "Open Source Code", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 90, "Copyright (c) 2026", &Font16, LCD_BACKGROUND,
                   BLACK);
  GUI_DisString_EN(10, 110, "EastWillowLearningLog", &Font16, LCD_BACKGROUND,
                   BLACK);
  GUI_DisString_EN(10, 140, "MIT License", &Font16, LCD_BACKGROUND, BLACK);

  // Draw Back Button
  GUI_DrawRectangle(BTN_BACK_X, BTN_BACK_Y, BTN_BACK_X + BTN_BACK_W,
                    BTN_BACK_Y + BTN_BACK_H, BLUE, DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DisString_EN(BTN_BACK_X + 10, BTN_BACK_Y + 8, "BACK", &Font16,
                   LCD_BACKGROUND, BLUE);
}

void App_Setup(SensorIntf *sen5x) {
  // 1. 初始化 LCD 底層系統 (包含 Serial)
  System_Init();

#ifdef ARDUINO
  Serial.println("SEN55 Air Quality LCD Demo");
  Serial.println("LCD Init...");
#else
  printf("SEN55 Air Quality LCD Demo\n");
  printf("LCD Init...\n");
#endif

  // 2. 初始化 LCD
  LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT;
  LCD_Init(Lcd_ScanDir, 100);
  TP_Init(Lcd_ScanDir);

#ifdef ARDUINO
  Serial.println("LCD Clear...");
#else
  printf("LCD Clear...\n");
#endif

  DrawMainScreen();

  // 3. 初始化 SEN55
#ifdef ARDUINO
  Serial.println("Sensirion Init...");
#else
  printf("Sensirion Init...\n");
#endif

  // Note: Wire.begin() is handled inside SensorReal::begin() for Arduino
  // For PC, SensorMock::begin() does nothing.

  uint16_t error;
  char errorMessage[256];

  error = sen5x->deviceReset();
  if (error) {
#ifdef ARDUINO
    Serial.print("Device Reset Error: ");
#endif
    sen5x->errorToString(error, errorMessage, 256);
#ifdef ARDUINO
    Serial.println(errorMessage);
#else
    printf("Device Reset Error: %s\n", errorMessage);
#endif
    GUI_DisString_EN(10, 60, "Sensor Reset Error", &Font20, LCD_BACKGROUND,
                     RED);
  }

  // 設定溫度補償 (依據範例)
  float tempOffset = 0.0;
  sen5x->setTemperatureOffsetSimple(tempOffset);

  // 開始測量
  error = sen5x->startMeasurement();
  if (error) {
#ifdef ARDUINO
    Serial.print("Start Measurement Error: ");
#endif
    sen5x->errorToString(error, errorMessage, 256);
#ifdef ARDUINO
    Serial.println(errorMessage);
#else
    printf("Start Measurement Error: %s\n", errorMessage);
#endif
    GUI_DisString_EN(10, 90, "Start Error", &Font20, LCD_BACKGROUND, RED);
  }
}

void App_Loop(SensorIntf *sen5x) {
  unsigned long currentMillis;
#ifdef ARDUINO
  currentMillis = millis();
#else
  mockMillis += 50; // Simulate 50ms per loop
  currentMillis = mockMillis;
  // Add small delay to avoid CPU hogging on PC
  Driver_Delay_ms(50);
#endif

  // --- Touch Handling ---
  unsigned char touchState = TP_Scan(0);
  uint16_t x = 0, y = 0;

  if (touchState & TP_PRESS_DOWN) {
    TP_GetXY(&x, &y);

    // Debounce / State Transition
    if (currentMillis - lastTransitionTime >= 200) {
      if (currentState == APP_STATE_MAIN) {
        // Check Info Button
        if (x >= BTN_INFO_X && x <= BTN_INFO_X + BTN_INFO_W &&
            y >= BTN_INFO_Y && y <= BTN_INFO_Y + BTN_INFO_H) {
          currentState = APP_STATE_INFO;
          DrawInfoScreen();
          lastTransitionTime = currentMillis;
        }
      } else if (currentState == APP_STATE_INFO) {
        // Check Back Button
        if (x >= BTN_BACK_X && x <= BTN_BACK_X + BTN_BACK_W &&
            y >= BTN_BACK_Y && y <= BTN_BACK_Y + BTN_BACK_H) {
          currentState = APP_STATE_MAIN;
          DrawMainScreen();
          lastTransitionTime = currentMillis;
        }
      }
    }
  }

  // --- Update Sensor Data (Only in MAIN State, Every 1000ms) ---

  if (currentState == APP_STATE_MAIN &&
      (currentMillis - lastSensorUpdate >= 1000)) {
    lastSensorUpdate = currentMillis;
    uint16_t error;
    char errorMessage[256];

    // 讀取測量值
    float massConcentrationPm1p0;
    float massConcentrationPm2p5;
    float massConcentrationPm4p0;
    float massConcentrationPm10p0;
    float ambientHumidity;
    float ambientTemperature;
    float vocIndex;
    float noxIndex;

    error = sen5x->readMeasuredValues(
        massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
        massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
        noxIndex);

    if (error) {
#ifdef ARDUINO
      Serial.print("Read Error: ");
#endif
      sen5x->errorToString(error, errorMessage, 256);
#ifdef ARDUINO
      Serial.println(errorMessage);
#else
      printf("Read Error: %s\n", errorMessage);
#endif
      GUI_DisString_EN(10, 300, "Read Error...", &Font16, LCD_BACKGROUND, RED);
    } else {
      // 如果讀取成功，更新 LCD 顯示

      // --- PM 數值 ---
      // PM 1.0
      displayValue(10, 60, "PM 1.0:", massConcentrationPm1p0, " ug/m3", BLACK);
      // PM 2.5 (使用紅色強調)
      displayValue(10, 90, "PM 2.5:", massConcentrationPm2p5, " ug/m3", RED);
      // PM 4.0
      displayValue(10, 120, "PM 4.0:", massConcentrationPm4p0, " ug/m3", BLACK);
      // PM 10.0
      displayValue(10, 150, "PM 10 :", massConcentrationPm10p0, " ug/m3",
                   BLACK);

      // --- 環境數值 ---
      // 溫度
      displayValue(10, 180, "Temp  :", ambientTemperature, " C", BLUE);
      // 濕度
      displayValue(10, 210, "Humid :", ambientHumidity, " %", BLUE);

      // --- 氣體指數 ---
      // VOC
      // 處理 NaN 狀況 (感測器預熱時可能是 NaN)
      if (isnan(vocIndex)) {
        GUI_DisString_EN(10, 240, "VOC Idx:   n/a", &Font20, LCD_BACKGROUND,
                         BLACK);
      } else {
        displayValue(10, 240, "VOC Idx:", vocIndex, "", MAGENTA);
      }

      // NOx
      if (isnan(noxIndex)) {
        GUI_DisString_EN(10, 270, "NOx Idx:   n/a", &Font20, LCD_BACKGROUND,
                         BLACK);
      } else {
        displayValue(10, 270, "NOx Idx:", noxIndex, "", MAGENTA);
      }
    }
  } else {
    // In Info state, we just wait for touch events.
    // Reduce delay to improve responsiveness and match emulator timing better.
    // On Arduino, a shorter delay (e.g. 10ms) suffices to yield.
    // On PC, the main loop already has a 50ms delay/pacing, so we don't need
    // much here.
    Driver_Delay_ms(10);
  }
}
