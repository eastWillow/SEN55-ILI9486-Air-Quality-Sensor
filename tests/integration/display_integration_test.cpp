#include "DEV_Config.h"
#include "EmulatorEngine.h"
#include "LCD_Driver_SDL.h"
#include "SensorMock.h"
#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>
#include <stdio.h> // for popen
#include <string>

// Provided by CMake via target_compile_definitions
#ifndef TEST_RESOURCE_DIR
#define TEST_RESOURCE_DIR "."
#endif

class DisplayIntegrationTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Ensure deterministic behavior
    srand(1);
    System_SetFastMode(true);
  }

  void TearDown() override {
    // Optional cleanup
  }

  // Helper to compare images using ImageMagick
  // Returns number of different pixels (AE metric)
  // Returns -1 on error
  // Returns -2 if reference missing
  int CompareWithReference(const std::string &actual,
                           const std::string &referenceName) {
    std::string referencePath =
        std::string(TEST_RESOURCE_DIR) + "/" + referenceName;
    std::string diffPath = "diff_" + referenceName;

    // Check if reference exists
    std::ifstream f(referencePath.c_str());
    if (!f.good()) {
      return -2;
    }

    // Construct command: compare -metric AE actual reference diff > null 2>&1
    // ImageMagick compare writes metric to stderr (!)
    // Note: We capture stderr to read the metric
    std::string cmd = "compare -metric AE " + actual + " " + referencePath +
                      " " + diffPath + " 2>&1";

    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
      return -1;

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
      result += buffer;
    }
    pclose(pipe);

    // Trim whitespace from result
    size_t first = result.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
      return -1;
    size_t last = result.find_last_not_of(" \t\n\r");
    result = result.substr(first, (last - first + 1));

    // Parse result (should be an integer number of pixels)
    try {
      return std::stoi(result);
    } catch (...) {
      // If ImageMagick isn't installed or command failed, it might return text
      std::cerr << "ImageMagick Error or Parse Failed: '" << result << "'"
                << std::endl;
      return -1;
    }
  }
  // Helper to compare two images existing in the current working directory
  int CompareLocalImages(const std::string &image1, const std::string &image2) {
    std::string diffPath = "diff_local_" + image1 + "_" + image2;
    std::string cmd = "compare -metric AE " + image1 + " " + image2 + " " +
                      diffPath + " 2>&1";

    FILE *pipe = popen(cmd.c_str(), "r");
    if (!pipe)
      return -1;

    char buffer[128];
    std::string result = "";
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
      result += buffer;
    }
    pclose(pipe);

    size_t first = result.find_first_not_of(" \t\n\r");
    if (first == std::string::npos)
      return -1;
    size_t last = result.find_last_not_of(" \t\n\r");
    result = result.substr(first, (last - first + 1));

    try {
      return std::stoi(result);
    } catch (...) {
      return -1;
    }
  }
};

TEST_F(DisplayIntegrationTest, CheckpointStartup) {
  SensorMock sensor;
  SystemTimeProvider timeProvider;
  EmulatorEngine engine;

  engine.initialize(&sensor, &timeProvider);

  // Deterministic sensor values
  sensor.setFixedValue(10, 15, 20, 25, 30, 21, 50, 20);

  // Frame 0: Just initialized
  engine.captureScreenshot("actual_startup.bmp");

  int diff = CompareWithReference("actual_startup.bmp", "startup.bmp");

  if (diff == -2) {
    std::cout << "[WARNING] Reference missing. Generated actual_startup.bmp."
              << std::endl;
    FAIL() << "Reference image missing: startup.bmp. Generated "
              "actual_startup.bmp for manual inspection.";
  } else if (diff == -1) {
    FAIL() << "ImageMagick comparison failed. Is 'compare' tool installed?";
  } else {
    ASSERT_LE(diff, 0) << "Startup screen mismatch! See diff_startup.bmp ("
                       << diff << " pixels differ)";
  }

  engine.shutdown();
}

TEST_F(DisplayIntegrationTest, CheckpointFinal) {
  SensorMock sensor;
  SystemTimeProvider timeProvider;
  EmulatorEngine engine;

  engine.initialize(&sensor, &timeProvider);

  // Deterministic sensor values
  sensor.setFixedValue(10, 15, 20, 25, 30, 21, 50, 20);

  // Run 25 frames
  engine.stepFrames(25);

  engine.captureScreenshot("actual_final.bmp");

  int diff = CompareWithReference("actual_final.bmp", "final.bmp");

  if (diff == -2) {
    std::cout << "[WARNING] Reference missing. Generated actual_final.bmp."
              << std::endl;
    FAIL() << "Reference image missing: final.bmp. Generated actual_final.bmp "
              "for manual inspection.";
  } else if (diff == -1) {
    FAIL() << "ImageMagick comparison failed. Is 'compare' tool installed?";
  } else {
    ASSERT_LE(diff, 0) << "Final screen mismatch! See diff_final.bmp (" << diff
                       << " pixels differ)";
  }

  engine.shutdown();
}

TEST_F(DisplayIntegrationTest, CheckpointButtonFeedback) {
  SensorMock sensor;
  MockTimeProvider timeProvider;
  EmulatorEngine engine;

  engine.initialize(&sensor, &timeProvider);

  // Deterministic sensor values
  sensor.setFixedValue(10, 15, 20, 25, 30, 21, 50, 20);

  // Initial state MAIN
  engine.stepFrames(1);

  // 1. Press INFO button (Coordinate defined in App.h)
  // We use the coordinate relative to the button
  SDL_SetMouseState(btnInfo.x + 5, btnInfo.y + 5, true);
  engine.stepFrames(1); // Detect press, enter feedback
  SDL_SetMouseState(btnInfo.x + 5, btnInfo.y + 5, false);

  // Advance 50ms (feedback window is 100ms)
  timeProvider.advance(50);
  engine.stepFrames(1); // Feedback should still be active

  engine.captureScreenshot("actual_feedback_info.bmp");

  int diff =
      CompareWithReference("actual_feedback_info.bmp", "feedback_info.bmp");

  if (diff == -2) {
    std::cout
        << "[WARNING] Reference missing. Generated actual_feedback_info.bmp."
        << std::endl;
    // We don't FAIL here if it's the first time, but we notify.
    // Actually, following the project pattern:
    FAIL() << "Reference image missing: feedback_info.bmp. Generated "
              "actual_feedback_info.bmp for manual inspection.";
  } else if (diff == -1) {
    FAIL() << "ImageMagick comparison failed.";
  } else {
    ASSERT_LE(diff, 0) << "Feedback screen mismatch! (" << diff
                       << " pixels differ)";
  }

  // Advance another 100ms (total > 100ms from start of feedback)
  timeProvider.advance(100);
  engine.stepFrames(1); // Should transition to INFO screen

  engine.captureScreenshot("actual_after_feedback.bmp");
  // Comparison for the final Info screen could also be done if we had a
  // reference.

  engine.shutdown();
}

TEST_F(DisplayIntegrationTest, CheckpointChart) {
  SensorMock sensor;
  MockTimeProvider timeProvider;
  EmulatorEngine engine;

  engine.initialize(&sensor, &timeProvider);

  // Use fixed values for determinism
  sensor.setFixedValue(20, 25, 30, 35, 40, 22, 100, 50);

  // Initial state MAIN
  engine.stepFrames(1);

  // 1. Advance time to populate trend data
  // Record 10 points (100 seconds)
  for (int i = 0; i < 10; i++) {
    // Vary the PM2.5 value slightly for each point to see a line
    sensor.setFixedValue(20, 25 + i * 2, 30, 35, 40, 22, 100, 50);
    timeProvider.advance(10000); // 10s
    engine.stepFrames(1);        // App_Loop should record trend data
  }

  // 2. Press TREND button (Coordinates btnTrend.x, btnTrend.y, W=75, H=30)
  SDL_SetMouseState(btnTrend.x + 5, btnTrend.y + 5, true);
  engine.stepFrames(1); // Detect press, enter feedback
  SDL_SetMouseState(btnTrend.x + 5, btnTrend.y + 5, false);

  // 3. Advance past feedback (100ms)
  timeProvider.advance(150);
  engine.stepFrames(1); // Transition to TREND state

  // 4. Capture screenshot
  engine.captureScreenshot("actual_trend_chart.bmp");

  int diff = CompareWithReference("actual_trend_chart.bmp", "trend_chart.bmp");

  if (diff == -2) {
    std::cout
        << "[WARNING] Reference missing. Generated actual_trend_chart.bmp."
        << std::endl;
    FAIL() << "Reference image missing: trend_chart.bmp. Generated "
              "actual_trend_chart.bmp for manual inspection.";
  } else if (diff == -1) {
    FAIL() << "ImageMagick comparison failed.";
  } else {
    ASSERT_LE(diff, 0) << "Trend chart screen mismatch! (" << diff
                       << " pixels differ)";
  }

  engine.shutdown();
}

TEST_F(DisplayIntegrationTest, CheckpointYAxisStability) {
  SensorMock sensor;
  MockTimeProvider timeProvider;
  EmulatorEngine engine;

  engine.initialize(&sensor, &timeProvider);

  // 1. Initial State (Trend Page with 400 zero values)
  sensor.setFixedValue(0, 0, 0, 0, 0, 0, 0, 0);
  for (int i = 0; i < 400; i++) {
    timeProvider.advance(10000);
    engine.stepFrames(1);
  }

  // Go to Trend Screen
  SDL_SetMouseState(btnTrend.x + 5, btnTrend.y + 5, true);
  engine.stepFrames(1);
  SDL_SetMouseState(btnTrend.x + 5, btnTrend.y + 5, false);
  timeProvider.advance(150);
  engine.stepFrames(1);

  // Capture Step 1 Baseline (Range 0-20 or similar padding)
  engine.captureScreenshot("y_axis_step1.bmp");

  // 2. Change to high values (Range 0-100+)
  sensor.setFixedValue(100, 100, 100, 100, 100, 100, 100, 100);
  for (int i = 0; i < 5; i++) {
    timeProvider.advance(10000);
    engine.stepFrames(1); // This will update trend data and redraw chart
  }
  engine.captureScreenshot("y_axis_step2.bmp");

  // 3. Change back to low values (Range 0-20 again)
  sensor.setFixedValue(0, 0, 0, 0, 0, 0, 0, 0);
  // We need to push out ALL the 100s to make the chart identical to Step 1
  // TREND_MAX_POINTS is 400.
  for (int i = 0; i < 400; i++) {
    timeProvider.advance(10000);
    engine.stepFrames(1);
  }
  engine.captureScreenshot("y_axis_step3.bmp");

  // Compare Step 1 and Step 3
  int diff = CompareLocalImages("y_axis_step3.bmp", "y_axis_step1.bmp");

  if (diff == -1) {
    FAIL() << "ImageMagick comparison failed.";
  } else {
    // We expect ZERO pixels difference. If > 0, digits are likely
    // overlapping/leaking.
    ASSERT_EQ(diff, 0) << "Y-Axis stability failed! pixels differ: " << diff
                       << ". Old digits likely remained on screen.";
  }

  engine.shutdown();
}
