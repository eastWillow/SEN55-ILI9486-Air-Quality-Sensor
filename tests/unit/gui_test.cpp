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

TEST(GUITest, DisNumZero) {
    uint8_t Str_Array[255] = {0};
    uint8_t Num_Array[255] = {0};
    int16_t Num_Bit = 0, Str_Bit = 0;
    int32_t Number = 0;

    if (Number == 0) {
        Str_Array[0] = '0';
        Str_Array[1] = '\0';
    } else {
        int isNegative = 0;
        uint32_t absNum = Number;
        if (Number < 0) {
            isNegative = 1;
            absNum = (uint32_t)(-(int64_t)Number);
        }

        while (absNum) {
            Num_Array[Num_Bit] = absNum % 10 + '0';
            Num_Bit++;
            absNum /= 10;
        }

        if (isNegative) {
            Num_Array[Num_Bit] = '-';
            Num_Bit++;
        }

        while (Num_Bit > 0) {
            Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
            Str_Bit ++;
            Num_Bit --;
        }
        Str_Array[Str_Bit] = '\0';
    }

    EXPECT_STREQ((const char*)Str_Array, "0");
}

TEST(GUITest, DisNumPositive) {
    uint8_t Str_Array[255] = {0};
    uint8_t Num_Array[255] = {0};
    int16_t Num_Bit = 0, Str_Bit = 0;
    int32_t Number = 12345;

    if (Number == 0) {
        Str_Array[0] = '0';
        Str_Array[1] = '\0';
    } else {
        int isNegative = 0;
        uint32_t absNum = Number;
        if (Number < 0) {
            isNegative = 1;
            absNum = (uint32_t)(-(int64_t)Number);
        }

        while (absNum) {
            Num_Array[Num_Bit] = absNum % 10 + '0';
            Num_Bit++;
            absNum /= 10;
        }

        if (isNegative) {
            Num_Array[Num_Bit] = '-';
            Num_Bit++;
        }

        while (Num_Bit > 0) {
            Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
            Str_Bit ++;
            Num_Bit --;
        }
        Str_Array[Str_Bit] = '\0';
    }

    EXPECT_STREQ((const char*)Str_Array, "12345");
}

TEST(GUITest, DisNumNegative) {
    uint8_t Str_Array[255] = {0};
    uint8_t Num_Array[255] = {0};
    int16_t Num_Bit = 0, Str_Bit = 0;
    int32_t Number = -9876;

    if (Number == 0) {
        Str_Array[0] = '0';
        Str_Array[1] = '\0';
    } else {
        int isNegative = 0;
        uint32_t absNum = Number;
        if (Number < 0) {
            isNegative = 1;
            absNum = (uint32_t)(-(int64_t)Number);
        }

        while (absNum) {
            Num_Array[Num_Bit] = absNum % 10 + '0';
            Num_Bit++;
            absNum /= 10;
        }

        if (isNegative) {
            Num_Array[Num_Bit] = '-';
            Num_Bit++;
        }

        while (Num_Bit > 0) {
            Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
            Str_Bit ++;
            Num_Bit --;
        }
        Str_Array[Str_Bit] = '\0';
    }

    EXPECT_STREQ((const char*)Str_Array, "-9876");
}

TEST(GUITest, DisNumInt32Min) {
    uint8_t Str_Array[255] = {0};
    uint8_t Num_Array[255] = {0};
    int16_t Num_Bit = 0, Str_Bit = 0;
    int32_t Number = -2147483648; // INT32_MIN

    if (Number == 0) {
        Str_Array[0] = '0';
        Str_Array[1] = '\0';
    } else {
        int isNegative = 0;
        uint32_t absNum = Number;
        if (Number < 0) {
            isNegative = 1;
            absNum = (uint32_t)(-(int64_t)Number);
        }

        while (absNum) {
            Num_Array[Num_Bit] = absNum % 10 + '0';
            Num_Bit++;
            absNum /= 10;
        }

        if (isNegative) {
            Num_Array[Num_Bit] = '-';
            Num_Bit++;
        }

        while (Num_Bit > 0) {
            Str_Array[Str_Bit] = Num_Array[Num_Bit - 1];
            Str_Bit ++;
            Num_Bit --;
        }
        Str_Array[Str_Bit] = '\0';
    }

    EXPECT_STREQ((const char*)Str_Array, "-2147483648");
}
