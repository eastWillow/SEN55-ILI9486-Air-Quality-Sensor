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
#define String std::to_string
#endif

// Application State
static AppState currentState = APP_STATE_MAIN;

AppState App_GetState() { return currentState; }

// Sensor Data
static SensorValues currentSensorValues = {0};

const SensorValues* App_GetSensorValues() {
  return &currentSensorValues;
}

void App_UpdateSensor(SensorIntf *sen5x) {
    uint16_t error;
    char errorMessage[256];

    error = sen5x->readMeasuredValues(
        currentSensorValues.massConcentrationPm1p0,
        currentSensorValues.massConcentrationPm2p5,
        currentSensorValues.massConcentrationPm4p0,
        currentSensorValues.massConcentrationPm10p0,
        currentSensorValues.ambientHumidity,
        currentSensorValues.ambientTemperature,
        currentSensorValues.vocIndex,
        currentSensorValues.noxIndex);

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
      currentSensorValues.valid = false;
    } else {
      currentSensorValues.valid = true;
    }
}

// Chart Data
static float chartData[CHART_MAX_POINTS];
static int chartDataCount = 0;
static unsigned long lastChartUpdate = 0;

void App_AddChartData(float value) {
  if (chartDataCount < CHART_MAX_POINTS) {
    chartData[chartDataCount++] = value;
  } else {
    // Shift left
    for (int i = 0; i < CHART_MAX_POINTS - 1; i++) {
      chartData[i] = chartData[i + 1];
    }
    chartData[CHART_MAX_POINTS - 1] = value;
  }
}

const float* App_GetChartData() {
  return chartData;
}

int App_GetChartCount() {
  return chartDataCount;
}

void App_ResetChartData() {
  chartDataCount = 0;
  lastChartUpdate = 0;
}

void App_ResetState() {
  currentState = APP_STATE_MAIN;
}

// 用於顯示數值的緩衝區
static char buf[30];

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

  // Draw Chart Button
  GUI_DrawRectangle(BTN_CHART_X, BTN_CHART_Y, BTN_CHART_X + BTN_CHART_W,
                    BTN_CHART_Y + BTN_CHART_H, BLUE, DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DisString_EN(BTN_CHART_X + 10, BTN_CHART_Y + 8, "CHART", &Font16,
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

void DrawChartScreen() {
  LCD_Clear(LCD_BACKGROUND);
  GUI_DisString_EN(10, 10, "PM2.5 Trading Chart", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DrawLine(0, 40, 480, 40, BLUE, LINE_SOLID, DOT_PIXEL_2X2);

  // Draw Back Button
  GUI_DrawRectangle(BTN_BACK_X, BTN_BACK_Y, BTN_BACK_X + BTN_BACK_W,
                    BTN_BACK_Y + BTN_BACK_H, BLUE, DRAW_EMPTY, DOT_PIXEL_1X1);
  GUI_DisString_EN(BTN_BACK_X + 10, BTN_BACK_Y + 8, "BACK", &Font16,
                   LCD_BACKGROUND, BLUE);

  // Draw Chart Frame
  GUI_DrawRectangle(10, 50, 470, 270, BLACK, DRAW_EMPTY, DOT_PIXEL_1X1);
}

void DrawChart() {
  // Define chart area
  const int xStart = 12;
  const int xEnd = 468;
  const int yStart = 52;
  const int yEnd = 268;
  const int width = xEnd - xStart;
  const int height = yEnd - yStart;

  // Clear chart area
  GUI_DrawRectangle(xStart, yStart, xEnd, yEnd, LCD_BACKGROUND, DRAW_FULL, DOT_PIXEL_DFT);

  if (chartDataCount < 2) return;

  // Find min and max
  float minVal = chartData[0];
  float maxVal = chartData[0];
  for (int i = 1; i < chartDataCount; i++) {
    if (chartData[i] < minVal) minVal = chartData[i];
    if (chartData[i] > maxVal) maxVal = chartData[i];
  }

  // Add some padding to min/max
  if (maxVal == minVal) {
    maxVal += 10;
    minVal -= 10;
  } else {
    float range = maxVal - minVal;
    maxVal += range * 0.1f;
    minVal -= range * 0.1f;
  }
  if (minVal < 0) minVal = 0;

  // Draw lines
  // Spacing based on total capacity (scrolling window)
  float xStep = (float)width / (CHART_MAX_POINTS - 1);

  for (int i = 0; i < chartDataCount - 1; i++) {
    int x1 = xStart + (int)(i * xStep);
    int x2 = xStart + (int)((i + 1) * xStep);

    // Map Y values to screen coordinates (inverted Y axis)
    int y1 = yEnd - (int)((chartData[i] - minVal) / (maxVal - minVal) * height);
    int y2 = yEnd - (int)((chartData[i + 1] - minVal) / (maxVal - minVal) * height);

    GUI_DrawLine(x1, y1, x2, y2, BLACK, LINE_SOLID, DOT_PIXEL_1X1);
  }

  // Display Min/Max labels
  char buf[32];
  snprintf(buf, sizeof(buf), "%.1f", maxVal);
  GUI_DisString_EN(xEnd - 60, yStart + 5, buf, &Font16, LCD_BACKGROUND, RED);

  snprintf(buf, sizeof(buf), "%.1f", minVal);
  GUI_DisString_EN(xEnd - 60, yEnd - 20, buf, &Font16, LCD_BACKGROUND, BLUE);
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

static unsigned long lastSensorUpdate = 0;

void App_Loop(SensorIntf *sen5x) {
  // --- Touch Handling ---
  unsigned char touchState = TP_Scan(0);
  uint16_t x = 0, y = 0;

  if (touchState & TP_PRESS_DOWN) {
    TP_GetXY(&x, &y);

    // Debounce / State Transition
    if (currentState == APP_STATE_MAIN) {
      // Check Info Button
      if (x >= BTN_INFO_X && x <= BTN_INFO_X + BTN_INFO_W && y >= BTN_INFO_Y &&
          y <= BTN_INFO_Y + BTN_INFO_H) {
        currentState = APP_STATE_INFO;
        DrawInfoScreen();
        Driver_Delay_ms(200); // Simple debounce
      }
      // Check Chart Button
      if (x >= BTN_CHART_X && x <= BTN_CHART_X + BTN_CHART_W && y >= BTN_CHART_Y &&
          y <= BTN_CHART_Y + BTN_CHART_H) {
        currentState = APP_STATE_CHART;
        DrawChartScreen();
        DrawChart(); // Initial draw
        Driver_Delay_ms(200); // Simple debounce
      }
    } else if (currentState == APP_STATE_INFO) {
      // Check Back Button
      if (x >= BTN_BACK_X && x <= BTN_BACK_X + BTN_BACK_W && y >= BTN_BACK_Y &&
          y <= BTN_BACK_Y + BTN_BACK_H) {
        currentState = APP_STATE_MAIN;
        DrawMainScreen();
        Driver_Delay_ms(200); // Simple debounce
      }
    } else if (currentState == APP_STATE_CHART) {
      // Check Back Button
      if (x >= BTN_BACK_X && x <= BTN_BACK_X + BTN_BACK_W && y >= BTN_BACK_Y &&
          y <= BTN_BACK_Y + BTN_BACK_H) {
        currentState = APP_STATE_MAIN;
        DrawMainScreen();
        Driver_Delay_ms(200); // Simple debounce
      }
    }
  }

  // --- Update Sensor Data (Only in MAIN State, Every 1000ms) ---
  unsigned long currentMillis;
#ifdef ARDUINO
  currentMillis = millis();
#else
  // Mock millis or just increment logic
  static unsigned long mockMillis = 0;
  mockMillis += 50; // Simulate 50ms per loop
  currentMillis = mockMillis;
  // Add small delay to avoid CPU hogging on PC
  Driver_Delay_ms(50);
#endif

  if (currentMillis - lastSensorUpdate >= 1000) {
    lastSensorUpdate = currentMillis;

    // Always update sensor data regardless of state
    App_UpdateSensor(sen5x);

    // Only update display if in MAIN state
    if (currentState == APP_STATE_MAIN) {
      if (!currentSensorValues.valid) {
        GUI_DisString_EN(10, 300, "Read Error...", &Font16, LCD_BACKGROUND, RED);
      } else {
        // 如果讀取成功，更新 LCD 顯示

        // --- PM 數值 ---
        // PM 1.0
        displayValue(10, 60, "PM 1.0:", currentSensorValues.massConcentrationPm1p0, " ug/m3", BLACK);
        // PM 2.5 (使用紅色強調)
        displayValue(10, 90, "PM 2.5:", currentSensorValues.massConcentrationPm2p5, " ug/m3", RED);
        // PM 4.0
        displayValue(10, 120, "PM 4.0:", currentSensorValues.massConcentrationPm4p0, " ug/m3", BLACK);
        // PM 10.0
        displayValue(10, 150, "PM 10 :", currentSensorValues.massConcentrationPm10p0, " ug/m3",
                     BLACK);

        // --- 環境數值 ---
        // 溫度
        displayValue(10, 180, "Temp  :", currentSensorValues.ambientTemperature, " C", BLUE);
        // 濕度
        displayValue(10, 210, "Humid :", currentSensorValues.ambientHumidity, " %", BLUE);

        // --- 氣體指數 ---
        // VOC
        // 處理 NaN 狀況 (感測器預熱時可能是 NaN)
        if (isnan(currentSensorValues.vocIndex)) {
          GUI_DisString_EN(10, 240, "VOC Idx:   n/a", &Font20, LCD_BACKGROUND,
                           BLACK);
        } else {
          displayValue(10, 240, "VOC Idx:", currentSensorValues.vocIndex, "", MAGENTA);
        }

        // NOx
        if (isnan(currentSensorValues.noxIndex)) {
          GUI_DisString_EN(10, 270, "NOx Idx:   n/a", &Font20, LCD_BACKGROUND,
                           BLACK);
        } else {
          displayValue(10, 270, "NOx Idx:", currentSensorValues.noxIndex, "", MAGENTA);
        }
      }
    }
  } else {
    // In Info state, we just wait for touch events.
    // Maybe sleep a bit to save CPU in emulation
    Driver_Delay_ms(100);
  }

  // Chart Update
  if (currentMillis - lastChartUpdate >= CHART_UPDATE_INTERVAL) {
    lastChartUpdate = currentMillis;
    if (currentSensorValues.valid) {
      // Use PM2.5 for the chart
      App_AddChartData(currentSensorValues.massConcentrationPm2p5);

      if (currentState == APP_STATE_CHART) {
        DrawChart();
      }
    }
  }
}
