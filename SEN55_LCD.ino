#include "DEV_Config.h"
#include "LCD_Driver.h"
#include "LCD_GUI.h"
#include <SensirionI2CSen5x.h>
#include <Wire.h>

// 宣告 SEN55 物件
SensirionI2CSen5x sen5x;

// 用於顯示數值的緩衝區
char buf[30];

void setup()
{
  // 1. 初始化 LCD 底層系統 (包含 Serial)
  System_Init();

  Serial.println("SEN55 Air Quality LCD Demo");
  
  // 2. 初始化 LCD
  Serial.println("LCD Init...");
  LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT;  
  LCD_Init(Lcd_ScanDir, 100);
  
  Serial.println("LCD Clear...");
  LCD_Clear(LCD_BACKGROUND);
  
  // 顯示標題
  GUI_DisString_EN(10, 10, "SEN55 Air Quality", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DrawLine(0, 40, 480, 40, BLUE, LINE_SOLID, DOT_PIXEL_2X2);

  // 3. 初始化 SEN55
  Serial.println("Sensirion Init...");
  Wire.begin();
  sen5x.begin(Wire);

  uint16_t error;
  char errorMessage[256];
  
  error = sen5x.deviceReset();
  if (error) {
    Serial.print("Device Reset Error: ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    GUI_DisString_EN(10, 60, "Sensor Reset Error", &Font20, LCD_BACKGROUND, RED);
  }

  // 設定溫度補償 (依據範例)
  float tempOffset = 0.0;
  sen5x.setTemperatureOffsetSimple(tempOffset);

  // 開始測量
  error = sen5x.startMeasurement();
  if (error) {
    Serial.print("Start Measurement Error: ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    GUI_DisString_EN(10, 90, "Start Error", &Font20, LCD_BACKGROUND, RED);
  }
}

// 輔助函式：在指定位置顯示標籤與數值
void displayValue(uint16_t x, uint16_t y, const char* label, float value, const char* unit, uint16_t color) {
    // 1. 顯示標籤 (標籤通常固定不變，直接畫即可)
    GUI_DisString_EN(x, y, label, &Font20, LCD_BACKGROUND, BLACK);
    
    // 2. 格式化數值
    String valStr = String(value, 1); 
    valStr += unit;
    
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
    GUI_DrawRectangle(valX, y, valX + clearWidth, y + 20, LCD_BACKGROUND, DRAW_FULL, DOT_PIXEL_DFT);

    // ---------------------------------------------------------

    // 3. 顯示新數值
    GUI_DisString_EN(valX, y, valStr.c_str(), &Font20, LCD_BACKGROUND, color);
}
void loop()
{
  uint16_t error;
  char errorMessage[256];

  delay(1000);

  // 讀取測量值
  float massConcentrationPm1p0;
  float massConcentrationPm2p5;
  float massConcentrationPm4p0;
  float massConcentrationPm10p0;
  float ambientHumidity;
  float ambientTemperature;
  float vocIndex;
  float noxIndex;

  error = sen5x.readMeasuredValues(
    massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
    massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex,
    noxIndex);

  if (error) {
    Serial.print("Read Error: ");
    errorToString(error, errorMessage, 256);
    Serial.println(errorMessage);
    GUI_DisString_EN(10, 300, "Read Error...", &Font16, LCD_BACKGROUND, RED);
  } else {
    // 如果讀取成功，更新 LCD 顯示
    // 為了避免整頁刷新閃爍，我們只覆蓋文字區域，或者直接重繪字串
    // LCD_GUI 函式繪製字串時會繪製背景色，所以直接呼叫即可覆蓋舊數值
    
    // --- PM 數值 ---
    // PM 1.0
    displayValue(10, 60, "PM 1.0:", massConcentrationPm1p0, " ug/m3", BLACK);
    // PM 2.5 (使用紅色強調)
    displayValue(10, 90, "PM 2.5:", massConcentrationPm2p5, " ug/m3", RED);
    // PM 4.0
    displayValue(10, 120, "PM 4.0:", massConcentrationPm4p0, " ug/m3", BLACK);
    // PM 10.0
    displayValue(10, 150, "PM 10 :", massConcentrationPm10p0, " ug/m3", BLACK);

    // --- 環境數值 ---
    // 溫度
    displayValue(10, 180, "Temp  :", ambientTemperature, " C", BLUE);
    // 濕度
    displayValue(10, 210, "Humid :", ambientHumidity, " %", BLUE);

    // --- 氣體指數 ---
    // VOC
    // 處理 NaN 狀況 (感測器預熱時可能是 NaN)
    if (isnan(vocIndex)) {
       GUI_DisString_EN(10, 240, "VOC Idx:   n/a", &Font20, LCD_BACKGROUND, BLACK);
    } else {
       displayValue(10, 240, "VOC Idx:", vocIndex, "", MAGENTA);
    }

    // NOx
    if (isnan(noxIndex)) {
       GUI_DisString_EN(10, 270, "NOx Idx:   n/a", &Font20, LCD_BACKGROUND, BLACK);
    } else {
       displayValue(10, 270, "NOx Idx:", noxIndex, "", MAGENTA);
    }
  }
}
