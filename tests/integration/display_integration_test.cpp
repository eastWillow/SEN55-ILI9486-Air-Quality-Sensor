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
};

TEST_F(DisplayIntegrationTest, CheckpointStartup) {
  SensorMock sensor;
  SystemTimeProvider timeProvider;
  EmulatorEngine engine;

  engine.initialize(&sensor, &timeProvider);

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

  // Initial state MAIN
  engine.stepFrames(1);

  // 1. Press INFO button (Coordinate defined in App.h)
  // We use the coordinate relative to the button
  SDL_SetMouseState(400 + 5, 10 + 5, true);
  engine.stepFrames(1); // Detect press, enter feedback
  SDL_SetMouseState(400 + 5, 10 + 5, false);

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
