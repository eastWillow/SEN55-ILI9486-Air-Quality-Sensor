#include <climits>
#include <cmath>
#include <gtest/gtest.h>

// CoreLib component headers
#include "App.h"
#include "LCD_Driver_SDL.h"
#include "SensorMock.h"

class DebouncePerformanceTest : public ::testing::Test {
protected:
  void SetUp() override {
    SDL_SetMouseState(0, 0, false);
    App_ResetState();
  }

  void TearDown() override {
    // Cleanup if needed
  }
};

/**
 * Test: Debounce Optimization
 * Validates that App_Loop doesn't block for 200ms when handling touch events.
 * It checks that after a click, subsequent clicks within 200ms are ignored,
 * but App_Loop returns immediately.
 */
TEST_F(DebouncePerformanceTest, NonBlockingDebounce) {
  SensorMock sensor;
  MockTimeProvider timeProvider;
  App_Setup(&sensor, &timeProvider);

  // Initial state should be MAIN
  EXPECT_EQ(App_GetState(), APP_STATE_MAIN);

  // 1. Click INFO button (btnInfo.x, btnInfo.y)
  SDL_SetMouseState(btnInfo.x + 5, btnInfo.y + 5, true);

  // Measure how many mockMillis pass during App_Loop
  unsigned long startMillis = timeProvider.getMillis();
  App_Loop(&sensor); // Should detect press, start feedback
  timeProvider.advance(50);
  unsigned long elapsed = timeProvider.getMillis() - startMillis;

  // The state should NOT follow until feedback is done (100ms)
  EXPECT_EQ(App_GetState(), APP_STATE_MAIN);

  // Advance to complete feedback
  timeProvider.advance(60);
  App_Loop(&sensor);

  EXPECT_EQ(App_GetState(), APP_STATE_INFO);
  EXPECT_LE(elapsed, 100)
      << "App_Loop blocked too long, potentially still using delay";

  SDL_SetMouseState(btnInfo.x + 5, btnInfo.y + 5, false);
  App_Loop(&sensor);

  // 2. Click BACK button immediately (debounce active)
  SDL_SetMouseState(btnBack.x + 5, btnBack.y + 5, true);
  App_Loop(&sensor); // This should be ignored due to debounce

  EXPECT_EQ(App_GetState(), APP_STATE_INFO)
      << "State transitioned during debounce period";

  // Advance mockMillis to simulate time passing beyond debounce period
  timeProvider.advance(250);

  // 3. Click BACK button again after debounce period
  App_Loop(&sensor); // Starts feedback phase
  timeProvider.advance(150);
  App_Loop(&sensor); // Processes transition

  EXPECT_EQ(App_GetState(), APP_STATE_MAIN)
      << "State failed to transition after debounce period and feedback";
}

/**
 * Test: Unsigned Integer Arithmetic Rollover
 * Validates that the debounce logic correctly handles a clock rollover.
 */
TEST_F(DebouncePerformanceTest, ClockRollover) {
  SensorMock sensor;
  MockTimeProvider timeProvider;
  App_Setup(&sensor, &timeProvider);

  // Set mockMillis near max unsigned long value to trigger a rollover
  timeProvider.set(ULONG_MAX - 100);

  // Let initial debounce period (-200 initialization) pass
  timeProvider.advance(200);

  // 1. Click INFO button
  SDL_SetMouseState(btnInfo.x + 5, btnInfo.y + 5, true);
  App_Loop(&sensor); // Starts feedback

  // Advance to complete feedback
  timeProvider.advance(150);
  App_Loop(&sensor); // Completes transition

  EXPECT_EQ(App_GetState(), APP_STATE_INFO);

  SDL_SetMouseState(0, 0, false);
  App_Loop(&sensor);

  // Now mockMillis will have rolled over
  timeProvider.advance(50);
  App_Loop(&sensor);
  timeProvider.advance(50);
  App_Loop(&sensor);
  timeProvider.advance(50);
  App_Loop(&sensor);
  timeProvider.advance(100);
  App_Loop(&sensor);
  // Total added: ~ 250ms

  // 2. Click BACK button and verify it transitions
  SDL_SetMouseState(btnBack.x + 5, btnBack.y + 5, true);
  App_Loop(&sensor); // Starts feedback

  timeProvider.advance(150);
  App_Loop(&sensor); // Completes transition

  EXPECT_EQ(App_GetState(), APP_STATE_MAIN)
      << "Rollover prevented state transition";
}
