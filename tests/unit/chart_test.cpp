#include <gtest/gtest.h>
#include "App.h"
#include "SensorMock.h"
#include "LCD_Driver_SDL.h"

class ChartTest : public ::testing::Test {
protected:
    void SetUp() override {
        App_ResetChartData();
        App_ResetState();
        SDL_SetMouseState(0, 0, false);
    }
};

TEST_F(ChartTest, AddDataLogic) {
    // Fill the chart
    for (int i = 0; i < CHART_MAX_POINTS; i++) {
        App_AddChartData((float)i);
        EXPECT_EQ(App_GetChartCount(), i + 1);
    }

    // Check first and last
    EXPECT_EQ(App_GetChartData()[0], 0.0f);
    EXPECT_EQ(App_GetChartData()[CHART_MAX_POINTS - 1], (float)(CHART_MAX_POINTS - 1));

    // Add one more (overflow)
    App_AddChartData(999.0f);
    EXPECT_EQ(App_GetChartCount(), CHART_MAX_POINTS);

    // Data shifted left
    EXPECT_EQ(App_GetChartData()[0], 1.0f);
    EXPECT_EQ(App_GetChartData()[CHART_MAX_POINTS - 1], 999.0f);
}

TEST_F(ChartTest, StateTransitionToChart) {
    SensorMock sensor;
    App_Setup(&sensor);

    EXPECT_EQ(App_GetState(), APP_STATE_MAIN);

    // Click Chart Button
    SDL_SetMouseState(BTN_CHART_X + 5, BTN_CHART_Y + 5, true);
    App_Loop(&sensor); // Detect
    SDL_SetMouseState(BTN_CHART_X + 5, BTN_CHART_Y + 5, false);
    App_Loop(&sensor); // Process

    EXPECT_EQ(App_GetState(), APP_STATE_CHART);
}

TEST_F(ChartTest, StateTransitionBackFromChart) {
    SensorMock sensor;
    App_Setup(&sensor);

    // Force enter chart state
    SDL_SetMouseState(BTN_CHART_X + 5, BTN_CHART_Y + 5, true);
    App_Loop(&sensor);
    SDL_SetMouseState(BTN_CHART_X + 5, BTN_CHART_Y + 5, false);
    App_Loop(&sensor);
    ASSERT_EQ(App_GetState(), APP_STATE_CHART);

    // Click Back Button
    SDL_SetMouseState(BTN_BACK_X + 5, BTN_BACK_Y + 5, true);
    App_Loop(&sensor);
    SDL_SetMouseState(BTN_BACK_X + 5, BTN_BACK_Y + 5, false);
    App_Loop(&sensor);

    EXPECT_EQ(App_GetState(), APP_STATE_MAIN);
}
