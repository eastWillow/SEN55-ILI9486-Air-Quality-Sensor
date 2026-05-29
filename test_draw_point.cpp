#include <iostream>
#include <stdint.h>

void simulate_new(uint16_t Xpoint, uint16_t Ypoint, uint16_t Dot_Pixel, int style) {
  int16_t XDir_Start;
  int16_t YDir_Start;
  if (style == 0) {
    XDir_Start = static_cast<int16_t>(Xpoint) - Dot_Pixel;
    YDir_Start = static_cast<int16_t>(Ypoint) - Dot_Pixel;
  } else {
    XDir_Start = static_cast<int16_t>(Xpoint) - 1;
    YDir_Start = static_cast<int16_t>(Ypoint) - 1;
  }

  if (XDir_Start < 0) {
    XDir_Start = 0;
  }
  if (YDir_Start < 0) {
    YDir_Start = 0;
  }

  int16_t XDir_End = static_cast<int16_t>(Xpoint) + Dot_Pixel - 1;
  int16_t YDir_End = static_cast<int16_t>(Ypoint) + Dot_Pixel - 1;

  std::cout << "New:\n";
  std::cout << "Start: (" << XDir_Start << ", " << YDir_Start << ")\n";
  std::cout << "End: (" << XDir_End << ", " << YDir_End << ")\n";
}

int main() {
  std::cout << "Testing Dot_Pixel = 2, DOT_STYLE_DFT (style 0)\n";
  simulate_new(10, 10, 2, 0);

  std::cout << "\nTesting Dot_Pixel = 2, style 1\n";
  simulate_new(10, 10, 2, 1);
  return 0;
}
