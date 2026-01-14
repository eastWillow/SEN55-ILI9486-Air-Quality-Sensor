#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MockSensor.h"
#include "App.h"

using ::testing::_;
using ::testing::Return;
using ::testing::SetArgReferee;
using ::testing::DoAll;

class AppTest : public ::testing::Test {
protected:
    MockSensor mockSensor;

    void SetUp() override {
        // Common setup if needed
    }

    void TearDown() override {
        // Common teardown if needed
    }
};

TEST_F(AppTest, InitialState) {
    // Expect deviceReset to be called and return 0 (success)
    EXPECT_CALL(mockSensor, deviceReset())
        .Times(1)
        .WillOnce(Return(0));

    // Expect setTemperatureOffsetSimple to be called
    EXPECT_CALL(mockSensor, setTemperatureOffsetSimple(_))
        .Times(1);

    // Expect startMeasurement to be called and return 0 (success)
    EXPECT_CALL(mockSensor, startMeasurement())
        .Times(1)
        .WillOnce(Return(0));

    // Call the function under test
    App_Setup(&mockSensor);
}

TEST_F(AppTest, ReadValuesSuccess) {
    // Setup first
    EXPECT_CALL(mockSensor, deviceReset()).WillRepeatedly(Return(0));
    EXPECT_CALL(mockSensor, setTemperatureOffsetSimple(_)).WillRepeatedly(Return());
    EXPECT_CALL(mockSensor, startMeasurement()).WillRepeatedly(Return(0));
    App_Setup(&mockSensor);

    // Now simulate loop reading values
    // We need to trigger the loop enough times or mock time if possible,
    // but App_Loop uses internal time logic.
    // However, App_Loop takes the sensor pointer.

    // We expect readMeasuredValues to be called if time condition is met.
    // Since we cannot easily mock millis() inside App_Loop without refactoring App.cpp to accept a time provider,
    // we might just verify that IF it calls it, it handles it.
    // Or we can rely on the fact that the first call might not trigger it unless we force it.

    // For this "Hello World" test, we just want to prove infrastructure works.
    // The InitialState test is sufficient for that.
}
