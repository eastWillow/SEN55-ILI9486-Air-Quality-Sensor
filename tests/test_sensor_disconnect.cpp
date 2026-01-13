#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockSensor.h"
#include "../src/App/App.h"
#include "../src/Display/LCD_Driver_SDL.h" // For LCD_Quit and LCD_Init if needed
#include <stdlib.h>

using ::testing::Return;
using ::testing::_;
using ::testing::DoAll;
using ::testing::SetArgReferee;

class SensorDisconnectTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Prevent SDL from trying to open a real window
        setenv("SDL_VIDEODRIVER", "dummy", 1);
    }

    void TearDown() override {
        // Clean up SDL resources
        #ifndef ARDUINO
        LCD_Quit();
        #endif
    }
};

TEST_F(SensorDisconnectTest, HandlesReadErrorGracefully) {
    MockSensor mockSensor;

    // 1. Expect deviceReset and startMeasurement to succeed during setup
    EXPECT_CALL(mockSensor, deviceReset())
        .WillOnce(Return(0));
    EXPECT_CALL(mockSensor, setTemperatureOffsetSimple(_));
    EXPECT_CALL(mockSensor, startMeasurement())
        .WillOnce(Return(0));

    // Run Setup
    // Note: App_Setup calls LCD_Init.
    // Since we set SDL_VIDEODRIVER=dummy, it should run without GUI.
    App_Setup(&mockSensor);

    // 2. Expect readMeasuredValues to fail (return error code 1)
    // We expect it to be called eventually.
    EXPECT_CALL(mockSensor, readMeasuredValues(_, _, _, _, _, _, _, _))
        .WillOnce(Return(1)); // Error!

    EXPECT_CALL(mockSensor, errorToString(1, _, _))
        .WillOnce([](uint16_t, char* buf, uint16_t) {
             strcpy(buf, "Simulated Disconnect");
        });

    // 3. Run Loop
    // The loop runs every 50ms (simulated).
    // Sensor update is every 1000ms.
    // We need to loop at least 20 times (20 * 50 = 1000).
    // Let's loop 30 times to be safe.

    // Capture Stdout to verify "Read Error" message
    testing::internal::CaptureStdout();

    for (int i = 0; i < 30; i++) {
        App_Loop(&mockSensor);
    }

    std::string output = testing::internal::GetCapturedStdout();

    // 4. Verify
    EXPECT_THAT(output, testing::HasSubstr("Read Error"));
    EXPECT_THAT(output, testing::HasSubstr("Simulated Disconnect"));
}
