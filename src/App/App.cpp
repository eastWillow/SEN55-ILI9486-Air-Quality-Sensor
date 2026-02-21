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
#endif

// Define Buttons
const Button btnInfo = {400, 10, 70, 30, "INFO"};
const Button btnTrend = {320, 10, 75, 30, "Trend"};
const Button btnBack = {10, 280, 70, 30, "BACK"};

namespace {
// Application State
AppState currentState = APP_STATE_MAIN;
unsigned long lastSensorUpdate = 0;
unsigned long lastTransitionTime = (unsigned long)-200;
TimeProvider *appTime = nullptr;

// Feedback state
bool inFeedback = false;
unsigned long feedbackStartTime = 0;
AppState pendingState = APP_STATE_MAIN;

// PM2.5 Trend Data
#define TREND_MAX_POINTS 400
float pm25History[TREND_MAX_POINTS];
int trendCount = 0;
unsigned long lastTrendUpdate = 0;

void RecordTrendData(float pm25) {
  if (trendCount < TREND_MAX_POINTS) {
    pm25History[trendCount++] = pm25;
  } else {
    for (int i = 0; i < TREND_MAX_POINTS - 1; i++) {
      pm25History[i] = pm25History[i + 1];
    }
    pm25History[TREND_MAX_POINTS - 1] = pm25;
  }
}
} // namespace

unsigned long SystemTimeProvider::getMillis() {
#ifdef ARDUINO
  return millis();
#else
  // Default PC implementation or fallback
  static unsigned long pcMillis = 0;
  pcMillis += 50;
  return pcMillis;
#endif
}

unsigned long MockTimeProvider::getMillis() { return current_time; }

void MockTimeProvider::advance(unsigned long ms) { current_time += ms; }

void MockTimeProvider::set(unsigned long ms) { current_time = ms; }

void App_ResetState() {
  currentState = APP_STATE_MAIN;
  lastSensorUpdate = 0;
  lastTransitionTime = (unsigned long)-200;
  appTime = nullptr;
  trendCount = 0;
  lastTrendUpdate = 0;
  inFeedback = false;
}

AppState App_GetState() { return currentState; }

// 輔助函式：繪製按鈕
static void DrawButton(const Button &btn, bool inverted) {
  uint16_t x = btn.x;
  uint16_t y = btn.y;
  uint16_t w = btn.w;
  uint16_t h = btn.h;
  const char *label = btn.label;

  if (inverted) {
    // Feedback mode: Invert colors and fill background
    GUI_DrawRectangle(x, y, x + w, y + h, BLUE, DRAW_FULL, DOT_PIXEL_DFT);
    GUI_DisString_EN(x + 10, y + 8, label, &Font16, BLUE, LCD_BACKGROUND);
  } else {
    // Normal mode: Standard border and text, no explicit background fill inside
    // border to perfectly match original golden samples.
    GUI_DrawRectangle(x, y, x + w, y + h, BLUE, DRAW_EMPTY, DOT_PIXEL_DFT);
    GUI_DisString_EN(x + 10, y + 8, label, &Font16, LCD_BACKGROUND, BLUE);
  }
}

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
  DrawButton(btnInfo, false);
  // Draw Trend Button
  DrawButton(btnTrend, false);
}

static void DrawTrendChart() {
  if (trendCount < 2) {
    GUI_DisString_EN(100, 150, "Waiting for data...", &Font20, LCD_BACKGROUND,
                     BLACK);
    return;
  }

  float minVal = pm25History[0];
  float maxVal = pm25History[0];
  for (int i = 1; i < trendCount; i++) {
    if (pm25History[i] < minVal)
      minVal = pm25History[i];
    if (pm25History[i] > maxVal)
      maxVal = pm25History[i];
  }

  if (maxVal - minVal < 1.0f) {
    maxVal = minVal + 5.0f;
    minVal = minVal - 5.0f;
    if (minVal < 0)
      minVal = 0;
  } else {
    // Add 10% padding
    float pad = (maxVal - minVal) * 0.1f;
    maxVal += pad;
    minVal -= pad;
    if (minVal < 0)
      minVal = 0;
  }

  float range = maxVal - minVal;
  const int chartX = 40;
  const int chartY = 60;
  const int chartW = 400;
  const int chartH = 200;

  // Clear chart area
  GUI_DrawRectangle(chartX, chartY, chartX + chartW, chartY + chartH,
                    LCD_BACKGROUND, DRAW_FULL, DOT_PIXEL_DFT);

  // Draw axes
  GUI_DrawLine(chartX, chartY, chartX, chartY + chartH, BLACK, LINE_SOLID,
               DOT_PIXEL_1X1);
  GUI_DrawLine(chartX, chartY + chartH, chartX + chartW, chartY + chartH, BLACK,
               LINE_SOLID, DOT_PIXEL_1X1);

  // Clear label area
  GUI_DrawRectangle(0, chartY, chartX - 1, chartY + chartH, LCD_BACKGROUND,
                    DRAW_FULL, DOT_PIXEL_DFT);

  // Draw Y-axis labels
  char label[32];
  snprintf(label, sizeof(label), "%.0f", maxVal);
  GUI_DisString_EN(5, chartY, label, &Font12, LCD_BACKGROUND, BLACK);
  snprintf(label, sizeof(label), "%.0f", minVal);
  GUI_DisString_EN(5, chartY + chartH - 12, label, &Font12, LCD_BACKGROUND,
                   BLACK);

  // Draw data lines
  for (int i = 0; i < trendCount - 1; i++) {
    int x1 = chartX + i;
    int y1 =
        chartY + chartH - (int)((pm25History[i] - minVal) / range * chartH);
    int x2 = chartX + i + 1;
    int y2 =
        chartY + chartH - (int)((pm25History[i + 1] - minVal) / range * chartH);
    GUI_DrawLine(x1, y1, x2, y2, RED, LINE_SOLID, DOT_PIXEL_1X1);
  }
}

void DrawTrendScreen() {
  LCD_Clear(LCD_BACKGROUND);
  GUI_DisString_EN(10, 10, "PM 2.5 Trend", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DrawLine(0, 40, 480, 40, BLUE, LINE_SOLID, DOT_PIXEL_2X2);

  // Draw Back Button
  DrawButton(btnBack, false);

  DrawTrendChart();
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
  DrawButton(btnBack, false);
}

void App_Setup(SensorIntf *sen5x, TimeProvider *timeProvider) {
  // Reset state to ensure clean start (especially important for tests)
  App_ResetState();

  // Store the time provider
  appTime = timeProvider;

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
  unsigned long currentMillis = appTime ? appTime->getMillis() : 0;

#ifndef ARDUINO
  // Add small delay to avoid CPU hogging on PC
  Driver_Delay_ms(50);
#endif

  // --- Touch Handling ---
  unsigned char touchState = TP_Scan(0);
  uint16_t x = 0, y = 0;

  if (touchState & TP_PRESS_DOWN) {
    TP_GetXY(&x, &y);

    // Debounce / State Transition
    if (currentMillis - lastTransitionTime >= 200 && !inFeedback) {
      if (currentState == APP_STATE_MAIN) {
        // Check Info Button
        if (btnInfo.contains(x, y)) {
          inFeedback = true;
          feedbackStartTime = currentMillis;
          pendingState = APP_STATE_INFO;
          DrawButton(btnInfo, true);
        }
        // Check Trend Button
        else if (btnTrend.contains(x, y)) {
          inFeedback = true;
          feedbackStartTime = currentMillis;
          pendingState = APP_STATE_TREND;
          DrawButton(btnTrend, true);
        }
      } else if (currentState == APP_STATE_INFO ||
                 currentState == APP_STATE_TREND) {
        // Check Back Button
        if (btnBack.contains(x, y)) {
          inFeedback = true;
          feedbackStartTime = currentMillis;
          pendingState = APP_STATE_MAIN;
          DrawButton(btnBack, true);
        }
      }
    }
  }

  // --- Feedback handling ---
  if (inFeedback && (currentMillis - feedbackStartTime >= 100)) {
    inFeedback = false;
    currentState = pendingState;
    if (currentState == APP_STATE_MAIN) {
      DrawMainScreen();
    } else if (currentState == APP_STATE_INFO) {
      DrawInfoScreen();
    } else if (currentState == APP_STATE_TREND) {
      DrawTrendScreen();
    }
    lastTransitionTime = currentMillis;
  }

  // --- Update Sensor Data (Every 1000ms) ---
  if (currentMillis - lastSensorUpdate >= 1000) {
    lastSensorUpdate = currentMillis;
    uint16_t error;
    char errorMessage[256];

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
      if (currentState == APP_STATE_MAIN) {
        GUI_DisString_EN(10, 300, "Read Error...", &Font16, LCD_BACKGROUND,
                         RED);
      }
    } else {
      // Record Trend Data every 10s
      if (currentMillis - lastTrendUpdate >= 10000) {
        lastTrendUpdate = currentMillis;
        RecordTrendData(massConcentrationPm2p5);
      }

      // Update UI if in MAIN state
      if (currentState == APP_STATE_MAIN) {
        // PM 1.0
        displayValue(10, 60, "PM 1.0:", massConcentrationPm1p0, " ug/m3",
                     BLACK);
        // PM 2.5 (使用紅色強調)
        displayValue(10, 90, "PM 2.5:", massConcentrationPm2p5, " ug/m3", RED);
        // PM 4.0
        displayValue(10, 120, "PM 4.0:", massConcentrationPm4p0, " ug/m3",
                     BLACK);
        // PM 10.0
        displayValue(10, 150, "PM 10 :", massConcentrationPm10p0, " ug/m3",
                     BLACK);

        // --- 環境數值 ---
        displayValue(10, 180, "Temp  :", ambientTemperature, " C", BLUE);
        displayValue(10, 210, "Humid :", ambientHumidity, " %", BLUE);

        // --- 氣體指數 ---
        if (isnan(vocIndex)) {
          GUI_DisString_EN(10, 240, "VOC Idx:   n/a", &Font20, LCD_BACKGROUND,
                           BLACK);
        } else {
          displayValue(10, 240, "VOC Idx:", vocIndex, "", MAGENTA);
        }

        if (isnan(noxIndex)) {
          GUI_DisString_EN(10, 270, "NOx Idx:   n/a", &Font20, LCD_BACKGROUND,
                           BLACK);
        } else {
          displayValue(10, 270, "NOx Idx:", noxIndex, "", MAGENTA);
        }
      }

      // If in TREND state, redraw chart if it was just updated
      if (currentState == APP_STATE_TREND &&
          (currentMillis - lastTrendUpdate < 100)) {
        DrawTrendChart();
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
