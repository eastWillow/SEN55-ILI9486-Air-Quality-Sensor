import re

def read_f(filepath):
    with open(filepath, 'r') as f: return f.read()

def write_f(filepath, content):
    with open(filepath, 'w') as f: f.write(content)

c = read_f('src/Display/LCD_Driver.cpp')
c = re.sub(r'void LCD_SetArealColor\(POINT Xstart.*?}\n}', '', c, flags=re.DOTALL)
c = re.sub(r'LCD_SetArealColor\(0, 0, sLCD_DIS\.LCD_Dis_Column , sLCD_DIS\.LCD_Dis_Page , Color\);', 'LCD_SetColor(Color, sLCD_DIS.LCD_Dis_Column, sLCD_DIS.LCD_Dis_Page);', c)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\nfunction:\n\t\tWrite register data\n\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\nfunction:\n\t\tCommon register initialization\n\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c)
write_f('src/Display/LCD_Driver.cpp', c)

c = read_f('src/Display/LCD_GUI.cpp')
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  \| File.*?Basic version\n\n\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  function:\tDraw a line of arbitrary slope.*?\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  function:\tDraw a rectangle.*?\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  function:\tUse the 8-point method.*?\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  function:\tShow English characters.*?\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  function:\tDisplay the string\n  parameter:\n\tXstart           ：X coordinate\n\tYstart           : Y coordinate.*?\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  function:\tDisplay the Gray map.*?\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  function:\tDisplay the bit map.*?\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
write_f('src/Display/LCD_GUI.cpp', c)

c = read_f('src/Display/LCD_Touch.cpp')
c = re.sub(r'/\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\n  \| File.*?Basic version\n\n\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*\*/\n', '', c, flags=re.DOTALL)
c = re.sub(r'if \(sLCD_DIS\.LCD_Dis_Column > sLCD_DIS\.LCD_Dis_Page\) \{\n    // Clear screen\n    GUI_DisString_EN\(sLCD_DIS\.LCD_Dis_Column - 60, 0, "CLEAR", &Font16, RED,\n                     BLUE\);\n    // adjustment\n    GUI_DisString_EN\(sLCD_DIS\.LCD_Dis_Column - 120, 0, "AD", &Font24, RED,\n                     BLUE\);\n    // choose the color\n    GUI_DrawRectangle\(sLCD_DIS\.LCD_Dis_Column - 50, 20, sLCD_DIS\.LCD_Dis_Column,\n                      70, BLUE, DRAW_FULL, DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(sLCD_DIS\.LCD_Dis_Column - 50, 80, sLCD_DIS\.LCD_Dis_Column,\n                      130, GREEN, DRAW_FULL, DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(sLCD_DIS\.LCD_Dis_Column - 50, 140,\n                      sLCD_DIS\.LCD_Dis_Column, 190, RED, DRAW_FULL,\n                      DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(sLCD_DIS\.LCD_Dis_Column - 50, 200,\n                      sLCD_DIS\.LCD_Dis_Column, 250, YELLOW, DRAW_FULL,\n                      DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(sLCD_DIS\.LCD_Dis_Column - 50, 260,\n                      sLCD_DIS\.LCD_Dis_Column, 310, BLACK, DRAW_FULL,\n                      DOT_PIXEL_1X1\);\n\n  \} else \{ // Vertical screen display\n    GUI_DisString_EN\(sLCD_DIS\.LCD_Dis_Column - 60, 0, "CLEAR", &Font16, RED,\n                     BLUE\);\n    GUI_DisString_EN\(sLCD_DIS\.LCD_Dis_Column - 120, 0, "AD", &Font24, RED,\n                     BLUE\);\n    GUI_DrawRectangle\(20, 20, 70, 70, BLUE, DRAW_FULL, DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(80, 20, 130, 70, GREEN, DRAW_FULL, DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(140, 20, 190, 70, RED, DRAW_FULL, DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(200, 20, 250, 70, YELLOW, DRAW_FULL, DOT_PIXEL_1X1\);\n    GUI_DrawRectangle\(260, 20, 310, 70, BLACK, DRAW_FULL, DOT_PIXEL_1X1\);\n  \}',
    r'''bool is_horizontal = sLCD_DIS.LCD_Dis_Column > sLCD_DIS.LCD_Dis_Page;
  GUI_DisString_EN(sLCD_DIS.LCD_Dis_Column - 60, 0, "CLEAR", &Font16, RED, BLUE);
  GUI_DisString_EN(sLCD_DIS.LCD_Dis_Column - 120, 0, "AD", &Font24, RED, BLUE);
  const COLOR colors[] = {BLUE, GREEN, RED, YELLOW, BLACK};
  for (int i = 0; i < 5; i++) {
    if (is_horizontal) {
      GUI_DrawRectangle(sLCD_DIS.LCD_Dis_Column - 50, 20 + i * 60, sLCD_DIS.LCD_Dis_Column, 70 + i * 60, colors[i], DRAW_FULL, DOT_PIXEL_1X1);
    } else {
      GUI_DrawRectangle(20 + i * 60, 20, 70 + i * 60, 70, colors[i], DRAW_FULL, DOT_PIXEL_1X1);
    }
  }''', c)

c = re.sub(r'if \(sLCD_DIS\.LCD_Dis_Column > sLCD_DIS\.LCD_Dis_Page\) \{\n        if \(sTP_Draw\.Xpoint > \(sLCD_DIS\.LCD_Dis_Column - 50\) &&\n                   sTP_Draw\.Ypoint > 20 && sTP_Draw\.Ypoint < 70\) \{\n          sTP_Draw\.Color = BLUE;\n        \} else if \(sTP_Draw\.Xpoint > \(sLCD_DIS\.LCD_Dis_Column - 50\) &&\n                   sTP_Draw\.Ypoint > 80 && sTP_Draw\.Ypoint < 130\) \{\n          sTP_Draw\.Color = GREEN;\n        \} else if \(sTP_Draw\.Xpoint > \(sLCD_DIS\.LCD_Dis_Column - 50\) &&\n                   sTP_Draw\.Ypoint > 140 && sTP_Draw\.Ypoint < 190\) \{\n          sTP_Draw\.Color = RED;\n        \} else if \(sTP_Draw\.Xpoint > \(sLCD_DIS\.LCD_Dis_Column - 50\) &&\n                   sTP_Draw\.Ypoint > 200 && sTP_Draw\.Ypoint < 250\) \{\n          sTP_Draw\.Color = YELLOW;\n        \} else if \(sTP_Draw\.Xpoint > \(sLCD_DIS\.LCD_Dis_Column - 50\) &&\n                   sTP_Draw\.Ypoint > 260 && sTP_Draw\.Ypoint < 310\) \{\n          sTP_Draw\.Color = BLACK;\n        \} else \{\n          GUI_DrawPoint\(sTP_Draw\.Xpoint, sTP_Draw\.Ypoint, sTP_Draw\.Color,\n                        DOT_PIXEL_1X1, DOT_FILL_RIGHTUP\);\n          GUI_DrawPoint\(sTP_Draw\.Xpoint \+ 1, sTP_Draw\.Ypoint, sTP_Draw\.Color,\n                        DOT_PIXEL_1X1, DOT_FILL_RIGHTUP\);\n          GUI_DrawPoint\(sTP_Draw\.Xpoint, sTP_Draw\.Ypoint \+ 1, sTP_Draw\.Color,\n                        DOT_PIXEL_1X1, DOT_FILL_RIGHTUP\);\n          GUI_DrawPoint\(sTP_Draw\.Xpoint \+ 1, sTP_Draw\.Ypoint \+ 1,\n                        sTP_Draw\.Color, DOT_PIXEL_1X1, DOT_FILL_RIGHTUP\);\n          GUI_DrawPoint\(sTP_Draw\.Xpoint, sTP_Draw\.Ypoint, sTP_Draw\.Color,\n                        DOT_PIXEL_2X2, DOT_FILL_RIGHTUP\);\n        \}\n        // Vertical screen\n      \} else \{\n        if \(sTP_Draw\.Xpoint > 20 && sTP_Draw\.Xpoint < 70 &&\n                   sTP_Draw\.Ypoint > 20 && sTP_Draw\.Ypoint < 70\) \{\n          sTP_Draw\.Color = BLUE;\n        \} else if \(sTP_Draw\.Xpoint > 80 && sTP_Draw\.Xpoint < 130 &&\n                   sTP_Draw\.Ypoint > 20 && sTP_Draw\.Ypoint < 70\) \{\n          sTP_Draw\.Color = GREEN;\n        \} else if \(sTP_Draw\.Xpoint > 140 && sTP_Draw\.Xpoint < 190 &&\n                   sTP_Draw\.Ypoint > 20 && sTP_Draw\.Ypoint < 70\) \{\n          sTP_Draw\.Color = RED;\n        \} else if \(sTP_Draw\.Xpoint > 200 && sTP_Draw\.Xpoint < 250 &&\n                   sTP_Draw\.Ypoint > 20 && sTP_Draw\.Ypoint < 70\) \{\n          sTP_Draw\.Color = YELLOW;\n        \} else if \(sTP_Draw\.Xpoint > 260 && sTP_Draw\.Xpoint < 310 &&\n                   sTP_Draw\.Ypoint > 20 && sTP_Draw\.Ypoint < 70\) \{\n          sTP_Draw\.Color = BLACK;\n        \} else \{\n          GUI_DrawPoint\(sTP_Draw\.Xpoint, sTP_Draw\.Ypoint, sTP_Draw\.Color,\n                        DOT_PIXEL_2X2, DOT_FILL_RIGHTUP\);\n        \}\n      \}',
    r'''bool is_horizontal = sLCD_DIS.LCD_Dis_Column > sLCD_DIS.LCD_Dis_Page;
      bool color_changed = false;
      const COLOR colors[] = {BLUE, GREEN, RED, YELLOW, BLACK};
      for (int i = 0; i < 5; i++) {
        bool hit = is_horizontal ?
                   (sTP_Draw.Xpoint > (sLCD_DIS.LCD_Dis_Column - 50) && sTP_Draw.Ypoint > (20 + i * 60) && sTP_Draw.Ypoint < (70 + i * 60)) :
                   (sTP_Draw.Ypoint > 20 && sTP_Draw.Ypoint < 70 && sTP_Draw.Xpoint > (20 + i * 60) && sTP_Draw.Xpoint < (70 + i * 60));
        if (hit) {
          sTP_Draw.Color = colors[i];
          color_changed = true;
          break;
        }
      }
      if (!color_changed) {
        GUI_DrawPoint(sTP_Draw.Xpoint, sTP_Draw.Ypoint, sTP_Draw.Color, DOT_PIXEL_2X2, DOT_FILL_RIGHTUP);
      }''', c)

c = re.sub(r'if \(sTP_DEV\.TP_Scan_Dir == R2L_D2U\) \{\n          DEBUG\("R2L_D2U"\);\n\n          sTP_DEV\.fXfac = \(float\)\(sLCD_DIS\.LCD_Dis_Column - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[1\]\[0\] - XYpoint_Arr\[0\]\[0\]\);\n          sTP_DEV\.fYfac = \(float\)\(sLCD_DIS\.LCD_Dis_Page - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[2\]\[1\] - XYpoint_Arr\[0\]\[1\]\);\n\n          sTP_DEV\.iXoff =\n              \(sLCD_DIS\.LCD_Dis_Column -\n               sTP_DEV\.fXfac \* \(XYpoint_Arr\[1\]\[0\] \+ XYpoint_Arr\[0\]\[0\]\)\) /\n              2;\n          sTP_DEV\.iYoff =\n              \(sLCD_DIS\.LCD_Dis_Page -\n               sTP_DEV\.fYfac \* \(XYpoint_Arr\[2\]\[1\] \+ XYpoint_Arr\[0\]\[1\]\)\) /\n              2;\n\n        \} else if \(sTP_DEV\.TP_Scan_Dir == L2R_U2D\) \{\n          DEBUG\("L2R_U2D"\);\n\n          sTP_DEV\.fXfac = \(float\)\(sLCD_DIS\.LCD_Dis_Column - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[0\]\[0\] - XYpoint_Arr\[1\]\[0\]\);\n          sTP_DEV\.fYfac = \(float\)\(sLCD_DIS\.LCD_Dis_Page - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[0\]\[1\] - XYpoint_Arr\[2\]\[1\]\);\n\n          sTP_DEV\.iXoff =\n              \(sLCD_DIS\.LCD_Dis_Column -\n               sTP_DEV\.fXfac \* \(XYpoint_Arr\[0\]\[0\] \+ XYpoint_Arr\[1\]\[0\]\)\) /\n              2;\n          sTP_DEV\.iYoff =\n              \(sLCD_DIS\.LCD_Dis_Page -\n               sTP_DEV\.fYfac \* \(XYpoint_Arr\[0\]\[1\] \+ XYpoint_Arr\[2\]\[1\]\)\) /\n              2;\n        \} else if \(sTP_DEV\.TP_Scan_Dir == U2D_R2L\) \{\n          DEBUG\("U2D_R2L"\);\n\n          sTP_DEV\.fXfac = \(float\)\(sLCD_DIS\.LCD_Dis_Column - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[1\]\[1\] - XYpoint_Arr\[0\]\[1\]\);\n          sTP_DEV\.fYfac = \(float\)\(sLCD_DIS\.LCD_Dis_Page - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[2\]\[0\] - XYpoint_Arr\[0\]\[0\]\);\n\n          sTP_DEV\.iXoff =\n              \(sLCD_DIS\.LCD_Dis_Column -\n               sTP_DEV\.fXfac \* \(XYpoint_Arr\[1\]\[1\] \+ XYpoint_Arr\[0\]\[1\]\)\) /\n              2;\n          sTP_DEV\.iYoff =\n              \(sLCD_DIS\.LCD_Dis_Page -\n               sTP_DEV\.fYfac \* \(XYpoint_Arr\[2\]\[0\] \+ XYpoint_Arr\[0\]\[0\]\)\) /\n              2;\n        \} else \{\n          DEBUG\("D2U_L2R"\);\n\n          sTP_DEV\.fXfac = \(float\)\(sLCD_DIS\.LCD_Dis_Column - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[0\]\[1\] - XYpoint_Arr\[1\]\[1\]\);\n          sTP_DEV\.fYfac = \(float\)\(sLCD_DIS\.LCD_Dis_Page - 2 \* Mar_Val\) /\n                          \(int16_t\)\(XYpoint_Arr\[0\]\[0\] - XYpoint_Arr\[2\]\[0\]\);\n\n          sTP_DEV\.iXoff =\n              \(sLCD_DIS\.LCD_Dis_Column -\n               sTP_DEV\.fXfac \* \(XYpoint_Arr\[0\]\[1\] \+ XYpoint_Arr\[1\]\[1\]\)\) /\n              2;\n          sTP_DEV\.iYoff =\n              \(sLCD_DIS\.LCD_Dis_Page -\n               sTP_DEV\.fYfac \* \(XYpoint_Arr\[0\]\[0\] \+ XYpoint_Arr\[2\]\[0\]\)\) /\n              2;\n        \}',
    r'''{
          int16_t dx = 0, dy = 0, sx = 0, sy = 0;
          if (sTP_DEV.TP_Scan_Dir == R2L_D2U) {
            DEBUG("R2L_D2U");
            dx = XYpoint_Arr[1][0] - XYpoint_Arr[0][0];
            dy = XYpoint_Arr[2][1] - XYpoint_Arr[0][1];
            sx = XYpoint_Arr[1][0] + XYpoint_Arr[0][0];
            sy = XYpoint_Arr[2][1] + XYpoint_Arr[0][1];
          } else if (sTP_DEV.TP_Scan_Dir == L2R_U2D) {
            DEBUG("L2R_U2D");
            dx = XYpoint_Arr[0][0] - XYpoint_Arr[1][0];
            dy = XYpoint_Arr[0][1] - XYpoint_Arr[2][1];
            sx = XYpoint_Arr[0][0] + XYpoint_Arr[1][0];
            sy = XYpoint_Arr[0][1] + XYpoint_Arr[2][1];
          } else if (sTP_DEV.TP_Scan_Dir == U2D_R2L) {
            DEBUG("U2D_R2L");
            dx = XYpoint_Arr[1][1] - XYpoint_Arr[0][1];
            dy = XYpoint_Arr[2][0] - XYpoint_Arr[0][0];
            sx = XYpoint_Arr[1][1] + XYpoint_Arr[0][1];
            sy = XYpoint_Arr[2][0] + XYpoint_Arr[0][0];
          } else {
            DEBUG("D2U_L2R");
            dx = XYpoint_Arr[0][1] - XYpoint_Arr[1][1];
            dy = XYpoint_Arr[0][0] - XYpoint_Arr[2][0];
            sx = XYpoint_Arr[0][1] + XYpoint_Arr[1][1];
            sy = XYpoint_Arr[0][0] + XYpoint_Arr[2][0];
          }

          if (dx == 0 || dy == 0) {
              DEBUG("Calibration divide by zero error");
              cnt = 0;
              continue;
          }

          sTP_DEV.fXfac = (float)(sLCD_DIS.LCD_Dis_Column - 2 * Mar_Val) / dx;
          sTP_DEV.fYfac = (float)(sLCD_DIS.LCD_Dis_Page - 2 * Mar_Val) / dy;
          sTP_DEV.iXoff = (sLCD_DIS.LCD_Dis_Column - sTP_DEV.fXfac * sx) / 2;
          sTP_DEV.iYoff = (sLCD_DIS.LCD_Dis_Page - sTP_DEV.fYfac * sy) / 2;
        }''', c)
write_f('src/Display/LCD_Touch.cpp', c)
