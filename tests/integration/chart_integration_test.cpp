#include "DEV_Config.h"
#include "EmulatorEngine.h"
#include "SensorMock.h"
#include "App.h"
#include "LCD_Driver_SDL.h" // For SDL_SetMouseState
#include <gtest/gtest.h>

class ChartIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    System_SetFastMode(true);
    App_ResetState();
    App_ResetChartData();
    SDL_SetMouseState(0, 0, false);
  }
};

TEST_F(ChartIntegrationTest, ChartNavigationAndRendering) {
  SensorMock sensor;
  EmulatorEngine engine;
  engine.initialize(&sensor);

  // 1. Initial State
  EXPECT_EQ(App_GetState(), APP_STATE_MAIN);

  // 2. Click Chart Button
  int clickX = BTN_CHART_X + BTN_CHART_W / 2;
  int clickY = BTN_CHART_Y + BTN_CHART_H / 2;

  SDL_SetMouseState(clickX, clickY, true); // Down
  engine.stepFrames(1);
  SDL_SetMouseState(clickX, clickY, false); // Up
  engine.stepFrames(5); // Wait for debounce and processing

  EXPECT_EQ(App_GetState(), APP_STATE_CHART) << "Failed to navigate to Chart";

  engine.captureScreenshot("chart_view_initial.bmp");

  // 3. Wait for Chart Update (5 seconds)
  // Each step is 50ms. 5000ms / 50ms = 100 frames.
  engine.stepFrames(110);

  // Check if data count increased.
  EXPECT_GE(App_GetChartCount(), 1) << "Chart data not updated after 5 seconds";

  engine.captureScreenshot("chart_view_updated.bmp");

  engine.shutdown();
}
