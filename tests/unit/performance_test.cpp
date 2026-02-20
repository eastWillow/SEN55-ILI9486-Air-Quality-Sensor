#include <gtest/gtest.h>
#include <chrono>
#include <iostream>
#include "DEV_Config.h"

/**
 * Performance test for Driver_Delay_us
 */
TEST(PerformanceTest, DriverDelayUsBaseline) {
    const int delay_us = 200;
    const int iterations = 10;

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; ++i) {
        Driver_Delay_us(delay_us);
    }
    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double average_duration = static_cast<double>(duration) / iterations;

    std::cout << "[          ] Measured average duration for Driver_Delay_us(" << delay_us << "): "
              << average_duration << " us" << std::endl;

    // Check that it's reasonably close to 200us.
    // We allow some slack for OS scheduling overhead.
    EXPECT_LT(average_duration, 1000.0); // Should be much better than SDL_Delay(1)
    EXPECT_GE(average_duration, 150.0);  // Should be at least close to 200us
}
