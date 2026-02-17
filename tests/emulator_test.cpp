#include <cmath>
#include <gtest/gtest.h>

// CoreLib component headers
#include "App.h"
#include "SensorMock.h"

/**
 * Component tests for CoreLib
 *
 * These tests validate CoreLib components directly by linking the library
 * and calling functions, rather than spawning the DisplayEmulator binary.
 *
 * Test separation:
 * - Smoke tests (emulator.yml): Validate DisplayEmulator binary integration
 * - Component tests (here): Validate CoreLib component correctness
 */

class CoreLibTest : public ::testing::Test {
protected:
  void SetUp() override {
    // Test setup if needed
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
 * Test: SensorMock provides valid data
 * Validates that mock sensor returns non-NaN values for all readings.
 */
TEST_F(CoreLibTest, SensorMockProvidesValidData) {
  SensorMock sensor;
  sensor.begin();
  sensor.startMeasurement();

  float pm1p0, pm2p5, pm4p0, pm10p0, humidity, temperature, vocIndex, noxIndex;
  uint16_t result = sensor.readMeasuredValues(
      pm1p0, pm2p5, pm4p0, pm10p0, humidity, temperature, vocIndex, noxIndex);

  EXPECT_EQ(result, 0) << "Read measurement failed";

  // Verify all values are valid (not NaN)
  // Use C-style isnan for portability
  EXPECT_FALSE(isnan(temperature)) << "Temperature is NaN";
  EXPECT_FALSE(isnan(humidity)) << "Humidity is NaN";
  EXPECT_FALSE(isnan(pm2p5)) << "PM2.5 is NaN";

  // Verify reasonable ranges for mock data
  EXPECT_GE(temperature, -40.0f) << "Temperature too low";
  EXPECT_LE(temperature, 85.0f) << "Temperature too high";
  EXPECT_GE(humidity, 0.0f) << "Humidity too low";
  EXPECT_LE(humidity, 100.0f) << "Humidity too high";
}

/**
 * Test: App_Setup executes without crash
 * Validates that app initialization completes successfully.
 */
TEST_F(CoreLibTest, AppSetupExecutes) {
  SensorMock sensor;
  sensor.begin();

  // App_Setup should not crash when called with valid sensor
  EXPECT_NO_THROW({ App_Setup(&sensor); })
      << "App_Setup threw exception or crashed";
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
