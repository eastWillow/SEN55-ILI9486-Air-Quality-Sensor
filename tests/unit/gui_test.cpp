#include <gtest/gtest.h>
#include "LCD_GUI.h"

/**
 * Test: GUI_GetFontSize logic
 * Validates that the adaptive font size selection works correctly for various display area sizes.
 */

TEST(GUITest, GetFontSizeAboveFont24) {
    // Font24: 17x24. Logic: Dx > 17 && Dy > 24
    EXPECT_EQ(GUI_GetFontSize(18, 25), &Font24);
}

TEST(GUITest, GetFontSizeExactFont24) {
    // Current logic: Dx > 17 && Dy > 24. 17 and 24 should return NULL (gap).
    // We expect this to fail initially if we want it to return Font24.
    // However, for the first pass, I'll write what it CURRENTLY does if I want to "verify" it,
    // OR I write what it SHOULD do and see it fail.
    // I'll write what it SHOULD do (>= logic).
    EXPECT_EQ(GUI_GetFontSize(17, 24), &Font24);
}

TEST(GUITest, GetFontSizeFont20Range) {
    // Font20: 14x20. Font24: 17x24.
    // Current Logic: (Dx > 14 && Dx < 17) && (Dy > 20 && Dy < 24)
    // Means Dx=15,16 and Dy=21,22,23
    EXPECT_EQ(GUI_GetFontSize(15, 21), &Font20);
    EXPECT_EQ(GUI_GetFontSize(16, 23), &Font20);
}

TEST(GUITest, GetFontSizeExactFont20) {
    EXPECT_EQ(GUI_GetFontSize(14, 20), &Font20);
}

TEST(GUITest, GetFontSizeFont16Range) {
    // Font16: 11x16. Font20: 14x20.
    EXPECT_EQ(GUI_GetFontSize(12, 17), &Font16);
    EXPECT_EQ(GUI_GetFontSize(13, 19), &Font16);
}

TEST(GUITest, GetFontSizeExactFont16) {
    EXPECT_EQ(GUI_GetFontSize(11, 16), &Font16);
}

TEST(GUITest, GetFontSizeFont12Range) {
    // Font12: 7x12. Font16: 11x16.
    EXPECT_EQ(GUI_GetFontSize(8, 13), &Font12);
    EXPECT_EQ(GUI_GetFontSize(10, 15), &Font12);
}

TEST(GUITest, GetFontSizeExactFont12) {
    EXPECT_EQ(GUI_GetFontSize(7, 12), &Font12);
}

TEST(GUITest, GetFontSizeFont8Range) {
    // Font8: 5x8. Font12: 7x12.
    EXPECT_EQ(GUI_GetFontSize(6, 9), &Font8);
}

TEST(GUITest, GetFontSizeExactFont8) {
    EXPECT_EQ(GUI_GetFontSize(5, 8), &Font8);
}

TEST(GUITest, GetFontSizeTooSmall) {
    EXPECT_EQ(GUI_GetFontSize(4, 7), nullptr);
    EXPECT_EQ(GUI_GetFontSize(0, 0), nullptr);
}

TEST(GUITest, GetFontSizeMismatched) {
    // One dimension large enough, the other not.
    // Width is large (20), but height (5) is smaller than any font height (min 8).
    EXPECT_EQ(GUI_GetFontSize(20, 5), nullptr);

    // Width is small (5), but height (30) is large.
    // Font8 (5x8) should fit.
    EXPECT_EQ(GUI_GetFontSize(5, 30), &Font8);
}

TEST(GUITest, GetFontSizeBestFit) {
    // Area 10x100:
    // Font24 (17x24) - too wide
    // Font20 (14x20) - too wide
    // Font16 (11x16) - too wide
    // Font12 (7x12) - fits!
    EXPECT_EQ(GUI_GetFontSize(10, 100), &Font12);
}
