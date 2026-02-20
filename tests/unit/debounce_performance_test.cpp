#include <gtest/gtest.h>
#include <chrono>
#include "App.h"
#include "SensorMock.h"
#include "LCD_Driver_SDL.h"
#include "DEV_Config.h"

class DebouncePerformanceTest : public ::testing::Test {
protected:
  void SetUp() override {
    SDL_SetMouseState(0, 0, false);
    System_SetFastMode(false); // Ensure delays are enabled for baseline
  }
};

TEST_F(DebouncePerformanceTest, ButtonClickDelay) {
    SensorMock mockSensor;
    App_Setup(&mockSensor);

    // Initial state should be MAIN
    ASSERT_EQ(App_GetState(), APP_STATE_MAIN);

    // Position of Info button (BTN_INFO_X, BTN_INFO_Y)
    int btnX = BTN_INFO_X + 5;
    int btnY = BTN_INFO_Y + 5;

    // Simulate click
    SDL_SetMouseState(btnX, btnY, true);

    auto start = std::chrono::high_resolution_clock::now();
    App_Loop(&mockSensor);
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << "[          ] Button click App_Loop duration: " << duration << " ms" << std::endl;

    // After optimization, it should be much faster than 200ms.
    // On PC with pacing, it's around 60ms (50ms pacing + 10ms info loop delay).
    EXPECT_LT(duration, 100);

    // Switch back to MAIN to be clean
    SDL_SetMouseState(0, 0, false);
    App_Loop(&mockSensor);
    SDL_SetMouseState(BTN_BACK_X + 5, BTN_BACK_Y + 5, true);
    App_Loop(&mockSensor);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
