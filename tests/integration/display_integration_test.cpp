#include "DEV_Config.h"
#include "EmulatorEngine.h"
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
  EmulatorEngine engine;

  engine.initialize(&sensor);

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
  EmulatorEngine engine;

  engine.initialize(&sensor);

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
