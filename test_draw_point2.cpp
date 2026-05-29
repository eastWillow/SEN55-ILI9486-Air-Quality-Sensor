#include <iostream>
#include <stdint.h>

void simulate_fix(uint16_t Xpoint, uint16_t Ypoint, uint16_t Dot_Pixel, int style) {
  int16_t XDir_Start;
  int16_t YDir_Start;
  int16_t XDir_End;
  int16_t YDir_End;

  if (style == 0) { // DOT_STYLE_DFT
    XDir_Start = static_cast<int16_t>(Xpoint) - Dot_Pixel;
    YDir_Start = static_cast<int16_t>(Ypoint) - Dot_Pixel;
    XDir_End = static_cast<int16_t>(Xpoint) + Dot_Pixel - 2;
    YDir_End = static_cast<int16_t>(Ypoint) + Dot_Pixel - 2;
  } else {
    XDir_Start = static_cast<int16_t>(Xpoint) - 1;
    YDir_Start = static_cast<int16_t>(Ypoint) - 1;
    XDir_End = static_cast<int16_t>(Xpoint) + Dot_Pixel - 2;
    YDir_End = static_cast<int16_t>(Ypoint) + Dot_Pixel - 2;
  }

  std::cout << "Fix:\n";
  std::cout << "Start: (" << XDir_Start << ", " << YDir_Start << ")\n";
  std::cout << "End: (" << XDir_End << ", " << YDir_End << ")\n";
}

int main() {
  std::cout << "Testing Dot_Pixel = 2, DOT_STYLE_DFT (style 0)\n";
  simulate_fix(10, 10, 2, 0);

  std::cout << "\nTesting Dot_Pixel = 2, style 1\n";
  simulate_fix(10, 10, 2, 1);
  return 0;
}
