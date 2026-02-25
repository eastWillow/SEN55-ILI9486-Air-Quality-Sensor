#include <cmath>
#include <gtest/gtest.h>

// CoreLib component headers
#include "App.h"
#include "LCD_Driver_SDL.h"
#include "SensorMock.h"

/**
 * Component tests for CoreLib
 *
 * These tests validate CoreLib components directly by linking the library
 * and calling functions, rather than spawning the DisplayEmulator binary.
 */

class CoreLibTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Reset mouse state before each test
    SDL_SetMouseState(0, 0, false);
  }

  void TearDown() override {
    // Test cleanup if needed
  }
};

/**
 * Test: SensorMock initialization
 * Validates that the mock sensor can be initialized without errors.
 */
TEST_F(CoreLibTest, SensorMockInitializes) {
  SensorMock sensor;
  uint16_t result = sensor.begin();

  // begin() should return 0 on success (standard error code convention)
  EXPECT_EQ(result, 0) << "SensorMock initialization failed";
}

/**
 * Test: App State Transitions
 * Validates that the app switches between MAIN and INFO screens via touch.
 */
TEST_F(CoreLibTest, InteractionTransitions) {
  SensorMock sensor;
  MockTimeProvider timeProvider;
  App_Setup(&sensor, &timeProvider);

  // Initial state should be MAIN
  EXPECT_EQ(App_GetState(), APP_STATE_MAIN);

  // 1. Click INFO button (btnInfo.x, btnInfo.y)
  SDL_SetMouseState(btnInfo.x + 5, btnInfo.y + 5, true);
  App_Loop(&sensor); // Detect press, enter feedback
  SDL_SetMouseState(btnInfo.x + 5, btnInfo.y + 5, false);

  // State should still be MAIN during feedback
  EXPECT_EQ(App_GetState(), APP_STATE_MAIN);

  // Advance 150ms to clear feedback (100ms)
  timeProvider.advance(150);
  App_Loop(&sensor); // Process state change

  EXPECT_EQ(App_GetState(), APP_STATE_INFO)
      << "Failed to transition to INFO screen after feedback";

  // Explicitly advance time past the 200ms debounce threshold
  timeProvider.advance(250);

  // 2. Click BACK button (btnBack.x, btnBack.y)
  SDL_SetMouseState(btnBack.x + 5, btnBack.y + 5, true);
  App_Loop(&sensor); // Detect press, enter feedback
  SDL_SetMouseState(btnBack.x + 5, btnBack.y + 5, false);

  // State should still be INFO during feedback
  EXPECT_EQ(App_GetState(), APP_STATE_INFO);

  // Advance 150ms to clear feedback
  timeProvider.advance(150);
  App_Loop(&sensor); // Process state change

  EXPECT_EQ(App_GetState(), APP_STATE_MAIN)
      << "Failed to transition back to MAIN screen after feedback";
}

/**
 * Test: Handle NaN Values
 * Validates that the app handles NaN sensor data (e.g. during warmup) without
 * crashing.
 */
class NaNSensorMock : public SensorMock {
public:
  uint16_t readMeasuredValues(float &pm1p0, float &pm2p5, float &pm4p0,
                              float &pm10p0, float &humidity,
                              float &temperature, float &vocIndex,
                              float &noxIndex) override {
    pm1p0 = 10.0f;
    pm2p5 = 10.0f;
    pm4p0 = 10.0f;
    pm10p0 = 10.0f;
    humidity = 50.0f;
    temperature = 25.0f;
    vocIndex = NAN; // Inject NaN
    noxIndex = NAN; // Inject NaN
    return 0;
  }
};

TEST_F(CoreLibTest, HandleNaNValues) {
  NaNSensorMock nanSensor;
  MockTimeProvider timeProvider;
  App_Setup(&nanSensor, &timeProvider);

  // Force a sensor update (needs > 1000ms)
  timeProvider.advance(1050);
  App_Loop(&nanSensor);

  // If we reached here without crash, the NaN handling in App_Loop is working.
  // The logic in App.cpp (lines 278-291) handles the display strings.
  SUCCEED();
}

/**
 * Test: App_Setup executes without crash
 * Validates that app initialization completes successfully.
 */
TEST_F(CoreLibTest, AppSetupExecutes) {
  SensorMock sensor;
  MockTimeProvider timeProvider;
  sensor.begin();

  // App_Setup should not crash when called with valid sensor
  EXPECT_NO_THROW({ App_Setup(&sensor, &timeProvider); })
      << "App_Setup threw exception or crashed";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
