#include "App.h"
#include "Log.h"
#include "../Display/DEV_Config.h"
#include "../Display/LCD_Driver.h"
#include "../Display/LCD_GUI.h"
#include "../Display/LCD_Touch.h"
#include <stdio.h>
#include <string.h>

#if defined(ARDUINO)
#include <Arduino.h>
#else
#include <cmath>
#include <cstring>
#include <string>
using namespace std;
#endif

// UI Button Definitions
const Button btnInfo = {400, 10, 70, 30, "INFO"};
const Button btnTrend = {320, 10, 75, 30, "Trend"};
const Button btnBack = {10, 280, 70, 30, "BACK"};

namespace {
// Application State
AppState currentState = APP_STATE_MAIN;
unsigned long lastSensorUpdate = 0;
unsigned long lastTransitionTime = static_cast<unsigned long>(-200);
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

// Text Memoization Cache
struct TextCache {
  uint16_t x;
  uint16_t y;
  char text[64];
};
#define MAX_TEXT_CACHE 16
TextCache textCache[MAX_TEXT_CACHE];
int textCacheCount = 0;

void RecordTrendData(float pm25) {
  if (trendCount < TREND_MAX_POINTS) {
    pm25History[trendCount++] = pm25;
  } else {
    // Optimization: Use memmove for O(1) bulk memory shift instead of O(N) loop
    memmove(pm25History, pm25History + 1, (TREND_MAX_POINTS - 1) * sizeof(float));
    pm25History[TREND_MAX_POINTS - 1] = pm25;
  }
}
} // namespace

unsigned long SystemTimeProvider::getMillis() {
#ifdef ARDUINO
  return millis();
#else
  // Default PC implementation or fallback
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
  lastTransitionTime = static_cast<unsigned long>(-200);
  appTime = nullptr;
  trendCount = 0;
  lastTrendUpdate = 0;
  inFeedback = false;
  textCacheCount = 0;
}

AppState App_GetState() { return currentState; }

// Helper function: Draw a button
static void DrawButton(const Button& btn, bool inverted) {
  if (inverted) {
    // Feedback mode: Invert colors and fill background
    GUI_DrawRectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h, BLUE, DRAW_FULL, DOT_PIXEL_DFT);
    GUI_DisString_EN(btn.x + 10, btn.y + 8, btn.label, &Font16, BLUE, LCD_BACKGROUND);
  } else {
    // Normal mode: Standard border and text, no explicit background fill inside
    // border to perfectly match original golden samples.
    GUI_DrawRectangle(btn.x, btn.y, btn.x + btn.w, btn.y + btn.h, BLUE, DRAW_EMPTY, DOT_PIXEL_DFT);
    GUI_DisString_EN(btn.x + 10, btn.y + 8, btn.label, &Font16, LCD_BACKGROUND, BLUE);
  }
}

// Helper function: Display a value at the specified position
static void displayValue(uint16_t x, uint16_t y, float value,
                         const char *unit, uint16_t color) {
  // 1. Format value
  char valStr[64];
  if (isnan(value)) {
    snprintf(valStr, sizeof(valStr), "n/a");
  } else {
    snprintf(valStr, sizeof(valStr), "%.1f%s", value, unit);
  }

  // Set the starting X coordinate of the value (consistent with the coordinates below)
  uint16_t valX = x + 150;

  size_t prevLen = 0;
  size_t newLen = strlen(valStr);

  // 2. Check memoization cache
  for (int i = 0; i < textCacheCount; i++) {
    if (textCache[i].x == valX && textCache[i].y == y) {
      if (strncmp(textCache[i].text, valStr, sizeof(textCache[i].text)) == 0) {
        return; // Value unchanged, skip redraw
      } else {
        prevLen = strlen(textCache[i].text);
        // Update cache
        strncpy(textCache[i].text, valStr, sizeof(textCache[i].text));
        textCache[i].text[sizeof(textCache[i].text) - 1] = '\0';
        break;
      }
    }
    if (i == textCacheCount - 1 && textCacheCount < MAX_TEXT_CACHE) {
       textCache[textCacheCount].x = valX;
       textCache[textCacheCount].y = y;
       strncpy(textCache[textCacheCount].text, valStr, sizeof(textCache[textCacheCount].text));
       textCache[textCacheCount].text[sizeof(textCache[textCacheCount].text) - 1] = '\0';
       textCacheCount++;
       break;
    }
  }

  if (textCacheCount == 0) {
     textCache[0].x = valX;
     textCache[0].y = y;
     strncpy(textCache[0].text, valStr, sizeof(textCache[0].text));
     textCache[0].text[sizeof(textCache[0].text) - 1] = '\0';
     textCacheCount++;
  }

  // ---------------------------------------------------------
  // Focus: Before displaying the new value, draw a solid rectangle
  // with the background color to clear the old content
  // ---------------------------------------------------------

  // Calculate the width to be cleared dynamically based on text length to minimize SPI overhead.
  // If the string was not found in the cache (prevLen == 0), fallback to a safe 150px clear
  // to ensure any static placeholders are fully overwritten.
  uint16_t clearWidth = 150;
  if (prevLen > 0) {
    size_t maxLen = (prevLen > newLen) ? prevLen : newLen;
    clearWidth = static_cast<uint16_t>(maxLen * Font20.Width);
  }

  // Simple boundary check to prevent errors caused by exceeding the screen edge (480)
  if (valX >= 480) {
    clearWidth = 0;
  } else if (valX + clearWidth > 480) {
    clearWidth = 480 - valX;
  }

  // Draw a solid rectangle (LCD_BACKGROUND = White, DRAW_FULL = Solid fill)
  // Set height to 20 (to match Font20)
  GUI_DrawRectangle(valX, y, valX + clearWidth, y + 20, LCD_BACKGROUND,
                    DRAW_FULL, DOT_PIXEL_DFT);

  // ---------------------------------------------------------

  // 3. Display new value
  GUI_DisString_EN(valX, y, valStr, &Font20, LCD_BACKGROUND, color);
}

static void DrawScreenHeader(const char* title) {
  LCD_Clear(LCD_BACKGROUND);
  GUI_DisString_EN(10, 10, title, &Font24, LCD_BACKGROUND, BLUE);
  GUI_DrawLine(0, 40, 480, 40, BLUE, LINE_SOLID, DOT_PIXEL_2X2);
}

void DrawMainScreen() {
  textCacheCount = 0;

  // Display header
  DrawScreenHeader("SEN55 Air Quality");

  // Draw Info Button
  DrawButton(btnInfo, false);
  // Draw Trend Button
  DrawButton(btnTrend, false);

  // Draw static labels for sensor values
  GUI_DisString_EN(10, 60, "PM 1.0:", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 90, "PM 2.5:", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 120, "PM 4.0:", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 150, "PM 10 :", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 180, "Temp  :", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 210, "Humid :", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 240, "VOC Idx:", &Font20, LCD_BACKGROUND, BLACK);
  GUI_DisString_EN(10, 270, "NOx Idx:", &Font20, LCD_BACKGROUND, BLACK);
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
  // Optimization: Pre-calculate the scale to avoid dividing inside the loop,
  // and reuse the previous y2 value as the new y1 to cut math ops in half.
  if (trendCount > 0) {
    float scale = chartH / range;
    int y1 = chartY + chartH - static_cast<int>((pm25History[0] - minVal) * scale);
    int runStartX = chartX;

    for (int i = 0; i < trendCount - 1; i++) {
      int x2 = chartX + i + 1;
      int y2 = chartY + chartH - static_cast<int>((pm25History[i + 1] - minVal) * scale);

      if (y1 != y2) {
        // Optimization: Group contiguous horizontal segments with the same Y-coordinate
        // into a single GUI_DrawLine call to avoid N separate LCD_SetArealColor block-fills.
        // Skip drawing horizontal segments of length 1, as the subsequent vertical jump already covers that pixel.
        if (runStartX < x2 - 1) {
          GUI_DrawLine(runStartX, y1, x2 - 1, y1, RED, LINE_SOLID, DOT_PIXEL_1X1);
        }

        // Draw the vertical jump
        GUI_DrawLine(x2 - 1, y1, x2 - 1, y2, RED, LINE_SOLID, DOT_PIXEL_1X1);

        y1 = y2; // Cache for next iteration
        runStartX = x2 - 1;
      }
    }

    // Draw the final horizontal run
    GUI_DrawLine(runStartX, y1, chartX + trendCount - 1, y1, RED, LINE_SOLID, DOT_PIXEL_1X1);
  }
}

void DrawTrendScreen() {
  DrawScreenHeader("PM 2.5 Trend");

  // Draw Back Button
  DrawButton(btnBack, false);

  DrawTrendChart();
}

void DrawInfoScreen() {
  DrawScreenHeader("Information");

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

  // 1. Initialize underlying LCD system (including Serial)
  System_Init();

  App_Log("SEN55 Air Quality LCD Demo");
  App_Log("LCD Init...");

  // 2. Initialize LCD
  LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT;
  LCD_Init(Lcd_ScanDir, 100);
  TP_Init(Lcd_ScanDir);

  App_Log("LCD Clear...");

  DrawMainScreen();

  // 3. Initialize SEN55
  App_Log("Sensirion Init...");

  // Note: Wire.begin() is handled inside SensorReal::begin() for Arduino
  // For PC, SensorMock::begin() does nothing.

  uint16_t error;
  char errorMessage[256];

  error = sen5x->deviceReset();
  if (error) {
    sen5x->errorToString(error, errorMessage, 256);
    App_Log("Device Reset Error: %s", errorMessage);
    GUI_DisString_EN(10, 60, "Sensor Reset Error", &Font20, LCD_BACKGROUND,
                     RED);
  }

  // Set temperature compensation (based on example)
  float tempOffset = 0.0;
  sen5x->setTemperatureOffsetSimple(tempOffset);

  // Start measurement
  error = sen5x->startMeasurement();
  if (error) {
    sen5x->errorToString(error, errorMessage, 256);
    App_Log("Start Measurement Error: %s", errorMessage);
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
      char errorMessage[256];
      sen5x->errorToString(error, errorMessage, 256);
      App_Log("Read Error: %s", errorMessage);
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
        displayValue(10, 60, massConcentrationPm1p0, " ug/m3", BLACK);
        // PM 2.5 (Highlighted in Red)
        displayValue(10, 90, massConcentrationPm2p5, " ug/m3", RED);
        // PM 4.0
        displayValue(10, 120, massConcentrationPm4p0, " ug/m3", BLACK);
        // PM 10.0
        displayValue(10, 150, massConcentrationPm10p0, " ug/m3", BLACK);

        // --- Environment Values ---
        displayValue(10, 180, ambientTemperature, " C", BLUE);
        displayValue(10, 210, ambientHumidity, " %", BLUE);

        // --- Gas Indices ---
        displayValue(10, 240, vocIndex, "", MAGENTA);
        displayValue(10, 270, noxIndex, "", MAGENTA);
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
