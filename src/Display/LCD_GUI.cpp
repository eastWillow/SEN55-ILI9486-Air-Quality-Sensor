/*****************************************************************************
  | File      	:	LCD_GUI.c
  | Author      :   Waveshare team
  | Function    :	Achieve drawing: draw points, lines, boxes, circles and
                    their size, solid dotted line, solid rectangle hollow
					rectangle, solid circle hollow circle.
  | Info        :
    Achieve display characters: Display a single character, string, number
    Achieve time display: adaptive size display time minutes and seconds
  ----------------
  |	This version:   V1.0
  | Date        :   2017-08-16
  | Info        :   Basic version

******************************************************************************/
#include "LCD_GUI.h"

#include <stdio.h>

#include "Debug.h"

extern LCD_DIS sLCD_DIS;
/******************************************************************************
  function:	Coordinate conversion
******************************************************************************/
static void GUI_Swap(POINT &Point1, POINT &Point2)
{
  POINT Temp;
  Temp = Point1;
  Point1 = Point2;
  Point2 = Temp;
}

/******************************************************************************
  function:	Clear screen
******************************************************************************/
void GUI_Clear(COLOR Color)
{
  LCD_Clear(Color);
}

/******************************************************************************
  function:	Draw Point(Xpoint, Ypoint) Fill the color
  parameter:
	Xpoint		:   The x coordinate of the point
	Ypoint		:   The y coordinate of the point
	Color		:   Set color
	Dot_Pixel	:	point size
******************************************************************************/
void GUI_DrawPoint(POINT Xpoint, POINT Ypoint, COLOR Color,
                   DOT_PIXEL Dot_Pixel, DOT_STYLE DOT_STYLE)
{
  if (Xpoint > sLCD_DIS.LCD_Dis_Column || Ypoint > sLCD_DIS.LCD_Dis_Page) {
    DEBUG("GUI_DrawPoint Input exceeds the normal display range");
    return;
  }

  int16_t XDir_Start;
  int16_t YDir_Start;
  int16_t XDir_End;
  int16_t YDir_End;

  if (DOT_STYLE == DOT_STYLE_DFT) {
    XDir_Start = static_cast<int16_t>(Xpoint) - Dot_Pixel;
    YDir_Start = static_cast<int16_t>(Ypoint) - Dot_Pixel;
    XDir_End = static_cast<int16_t>(Xpoint) + Dot_Pixel - 1;
    YDir_End = static_cast<int16_t>(Ypoint) + Dot_Pixel - 1;
  } else {
    XDir_Start = static_cast<int16_t>(Xpoint) - 1;
    YDir_Start = static_cast<int16_t>(Ypoint) - 1;
    XDir_End = static_cast<int16_t>(Xpoint) + Dot_Pixel - 1;
    YDir_End = static_cast<int16_t>(Ypoint) + Dot_Pixel - 1;
  }

  if (XDir_Start < 0) {
    XDir_Start = 0;
  }
  if (YDir_Start < 0) {
    YDir_Start = 0;
  }

  if (XDir_End > sLCD_DIS.LCD_Dis_Column) {
    XDir_End = sLCD_DIS.LCD_Dis_Column;
  }
  if (YDir_End > sLCD_DIS.LCD_Dis_Page) {
    YDir_End = sLCD_DIS.LCD_Dis_Page;
  }

  if (XDir_Start < XDir_End && YDir_Start < YDir_End) {
    LCD_SetArealColor(XDir_Start, YDir_Start, XDir_End, YDir_End, Color);
  }
}

/******************************************************************************
  function:	Draw a line of arbitrary slope
  parameter:
	Xstart ：Starting x point coordinates
	Ystart ：Starting x point coordinates
	Xend   ：End point x coordinate
	Yend   ：End point y coordinate
	Color  ：The color of the line segment
******************************************************************************/
void GUI_DrawLine(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                  COLOR Color, LINE_STYLE Line_Style, DOT_PIXEL Dot_Pixel)
{
  if (Xstart > sLCD_DIS.LCD_Dis_Column || Ystart > sLCD_DIS.LCD_Dis_Page ||
      Xend > sLCD_DIS.LCD_Dis_Column || Yend > sLCD_DIS.LCD_Dis_Page) {
    DEBUG("GUI_DrawLine Input exceeds the normal display range");
    return;
  }

  if (Line_Style == LINE_SOLID && (Xstart == Xend || Ystart == Yend)) {
    POINT min_X = Xstart < Xend ? Xstart : Xend;
    POINT max_X = Xstart > Xend ? Xstart : Xend;
    POINT min_Y = Ystart < Yend ? Ystart : Yend;
    POINT max_Y = Ystart > Yend ? Ystart : Yend;

    POINT X1 = (min_X >= Dot_Pixel) ? (min_X - Dot_Pixel) : 0;
    POINT Y1 = (min_Y >= Dot_Pixel) ? (min_Y - Dot_Pixel) : 0;
    POINT X2 = max_X + Dot_Pixel - 1;
    POINT Y2 = max_Y + Dot_Pixel - 1;

    if (X2 > sLCD_DIS.LCD_Dis_Column) X2 = sLCD_DIS.LCD_Dis_Column;
    if (Y2 > sLCD_DIS.LCD_Dis_Page) Y2 = sLCD_DIS.LCD_Dis_Page;

    if (X1 < X2 && Y1 < Y2) {
      LCD_SetArealColor(X1, Y1, X2, Y2, Color);
    }
    return;
  }

  POINT Xpoint = Xstart;
  POINT Ypoint = Ystart;
  int dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
  int dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;

  // Increment direction, 1 is positive, -1 is counter;
  int XAddway = Xstart < Xend ? 1 : -1;
  int YAddway = Ystart < Yend ? 1 : -1;

  //Cumulative error
  int Esp = dx + dy;
  char Line_Style_Temp = 0;

  for (;;) {
    Line_Style_Temp++;
    //Painted dotted line, 2 point is really virtual
    if (Line_Style == LINE_DOTTED && Line_Style_Temp % 3 == 0) {
      //DEBUG("LINE_DOTTED");
      GUI_DrawPoint(Xpoint, Ypoint, LCD_BACKGROUND, Dot_Pixel, DOT_STYLE_DFT);
      Line_Style_Temp = 0;
    } else {
      GUI_DrawPoint(Xpoint, Ypoint, Color, Dot_Pixel, DOT_STYLE_DFT);
    }
    if (2 * Esp >= dy) {
      if (Xpoint == Xend) break;
      Esp += dy;
      Xpoint += XAddway;
    }
    if (2 * Esp <= dx) {
      if (Ypoint == Yend) break;
      Esp += dx;
      Ypoint += YAddway;
    }
  }
}

/******************************************************************************
  function:	Draw a rectangle
  parameter:
	Xstart ：Rectangular  Starting x point coordinates
	Ystart ：Rectangular  Starting x point coordinates
	Xend   ：Rectangular  End point x coordinate
	Yend   ：Rectangular  End point y coordinate
	Color  ：The color of the Rectangular segment
	Filled : Whether it is filled--- 1 solid 0：empty
******************************************************************************/
void GUI_DrawRectangle(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                       COLOR Color, DRAW_FILL Filled, DOT_PIXEL Dot_Pixel)
{
  if (Xstart > sLCD_DIS.LCD_Dis_Column || Ystart > sLCD_DIS.LCD_Dis_Page ||
      Xend > sLCD_DIS.LCD_Dis_Column || Yend > sLCD_DIS.LCD_Dis_Page) {
    DEBUG("Input exceeds the normal display range");
    return;
  }

  if (Xstart > Xend)
    GUI_Swap(Xstart, Xend);
  if (Ystart > Yend)
    GUI_Swap(Ystart, Yend);

  if (Filled ) {
    LCD_SetArealColor(Xstart, Ystart, Xend, Yend, Color);
  } else {
    GUI_DrawLine(Xstart, Ystart, Xend, Ystart, Color , LINE_SOLID, Dot_Pixel);
    GUI_DrawLine(Xstart, Ystart, Xstart, Yend, Color , LINE_SOLID, Dot_Pixel);
    GUI_DrawLine(Xend, Yend, Xend, Ystart, Color , LINE_SOLID, Dot_Pixel);
    GUI_DrawLine(Xend, Yend, Xstart, Yend, Color , LINE_SOLID, Dot_Pixel);
  }
}

/******************************************************************************
  function:	Use the 8-point method to draw a circle of the
				specified size at the specified position.
  parameter:
	X_Center  ：Center X coordinate
	Y_Center  ：Center Y coordinate
	Radius    ：circle Radius
	Color     ：The color of the ：circle segment
	Filled    : Whether it is filled: 1 filling 0：Do not
******************************************************************************/
void GUI_DrawCircle(POINT X_Center, POINT Y_Center, LENGTH Radius,
                    COLOR Color, DRAW_FILL  Draw_Fill , DOT_PIXEL Dot_Pixel)
{
  if (X_Center > sLCD_DIS.LCD_Dis_Column || Y_Center >= sLCD_DIS.LCD_Dis_Page) {
    DEBUG("GUI_DrawCircle Input exceeds the normal display range");
    return;
  }

  //Draw a circle from(0, R) as a starting point
  int16_t XCurrent, YCurrent;
  XCurrent = 0;
  YCurrent = Radius;

  //Cumulative error,judge the next point of the logo
  int16_t Esp = 3 - (Radius << 1 );

  if (Draw_Fill == DRAW_FULL) {
    while (XCurrent <= YCurrent ) { //Realistic circles
      for (int16_t sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
        GUI_DrawPoint(X_Center + XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );//1
        GUI_DrawPoint(X_Center - XCurrent, Y_Center + sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );//2
        GUI_DrawPoint(X_Center - sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );//3
        GUI_DrawPoint(X_Center - sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );//4
        GUI_DrawPoint(X_Center - XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );//5
        GUI_DrawPoint(X_Center + XCurrent, Y_Center - sCountY, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );//6
        GUI_DrawPoint(X_Center + sCountY, Y_Center - XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );//7
        GUI_DrawPoint(X_Center + sCountY, Y_Center + XCurrent, Color, DOT_PIXEL_DFT, DOT_STYLE_DFT );
      }
      if (Esp < 0 )
        Esp += 4 * XCurrent + 6;
      else {
        Esp += 10 + 4 * (XCurrent - YCurrent );
        YCurrent --;
      }
      XCurrent ++;
    }
  } else { //Draw a hollow circle
    while (XCurrent <= YCurrent ) {
      GUI_DrawPoint(X_Center + XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//1
      GUI_DrawPoint(X_Center - XCurrent, Y_Center + YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//2
      GUI_DrawPoint(X_Center - YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//3
      GUI_DrawPoint(X_Center - YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//4
      GUI_DrawPoint(X_Center - XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//5
      GUI_DrawPoint(X_Center + XCurrent, Y_Center - YCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//6
      GUI_DrawPoint(X_Center + YCurrent, Y_Center - XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//7
      GUI_DrawPoint(X_Center + YCurrent, Y_Center + XCurrent, Color, Dot_Pixel, DOT_STYLE_DFT );//0

      if (Esp < 0 )
        Esp += 4 * XCurrent + 6;
      else {
        Esp += 10 + 4 * (XCurrent - YCurrent );
        YCurrent --;
      }
      XCurrent ++;
    }
  }
}

/******************************************************************************
  function:	Show English characters
  parameter:
	Xpoint           ：X coordinate
	Ypoint           ：Y coordinate
	Acsii_Char       ：To display the English characters
	Font             ：A structure pointer that displays a character size
	Color_Background : Select the background color of the English character
	Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void GUI_DisChar(POINT Xpoint, POINT Ypoint, const char Acsii_Char,
                 sFONT* Font, COLOR Color_Background, COLOR Color_Foreground)
{
  if (Font == NULL) return;
  POINT Page, Column;

  if (Xpoint > sLCD_DIS.LCD_Dis_Column || Ypoint > sLCD_DIS.LCD_Dis_Page) {
    DEBUG("GUI_DisChar Input exceeds the normal display range");
    return;
  }

  // Security: Prevent out-of-bounds array access for non-printable characters
  char safe_char = Acsii_Char;
  if (safe_char < ' ' || safe_char > '~') {
    safe_char = '?';
  }

  uint32_t Char_Offset = (safe_char - ' ') * Font->Height * (Font->Width / 8 + (Font->Width % 8 ? 1 : 0));
  const unsigned char *ptr = &Font->table[Char_Offset];

  // If the background color is different from FONT_BACKGROUND, fill the background area first
  if (FONT_BACKGROUND != Color_Background) {
    LCD_SetArealColor(Xpoint, Ypoint, Xpoint + Font->Width, Ypoint + Font->Height, Color_Background);
  }

  for (Page = 0; Page < Font->Height; Page ++ ) {
    int16_t startCol = -1;
    for (Column = 0; Column < Font->Width; Column ++ ) {

      bool active = false;
      #ifdef ARDUINO
      active = pgm_read_byte(ptr) & (0x80 >> (Column % 8));
      #else
      active = *ptr & (0x80 >> (Column % 8));
      #endif

      if (active) {
        if (startCol == -1) {
          startCol = Column;
        }
      } else {
        if (startCol != -1) {
          GUI_DrawLine(Xpoint + startCol, Ypoint + Page, Xpoint + Column - 1, Ypoint + Page, Color_Foreground, LINE_SOLID, DOT_PIXEL_DFT);
          startCol = -1;
        }
      }

      //One pixel is 8 bits
      if (Column % 8 == 7)
        ptr++;
    }/* Write a line */

    if (startCol != -1) {
      GUI_DrawLine(Xpoint + startCol, Ypoint + Page, Xpoint + Font->Width - 1, Ypoint + Page, Color_Foreground, LINE_SOLID, DOT_PIXEL_DFT);
    }

    if (Font->Width % 8 != 0)
      ptr++;
  }/* Write all */
}

/******************************************************************************
  function:	Display the string
  parameter:
	Xstart           ：X coordinate
	Ystart           ：Y coordinate
	pString          ：The first address of the English string to be displayed
	Font             ：A structure pointer that displays a character size
	Color_Background : Select the background color of the English character
	Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void GUI_DisString_EN(POINT Xstart, POINT Ystart, const char * pString,
                      sFONT* Font, COLOR Color_Background, COLOR Color_Foreground )
{
  if (pString == NULL || Font == NULL) return;
  POINT Xpoint = Xstart;
  POINT Ypoint = Ystart;

  if (Xstart > sLCD_DIS.LCD_Dis_Column || Ystart > sLCD_DIS.LCD_Dis_Page) {
    DEBUG("GUI_DisString_EN Input exceeds the normal display range");
    return;
  }

  while (* pString != '\0') {
    //if X direction filled , reposition to(Xstart,Ypoint),Ypoint is Y direction plus the height of the character
    if ((Xpoint + Font->Width ) > sLCD_DIS.LCD_Dis_Column ) {
      Xpoint = Xstart;
      Ypoint += Font->Height;
    }

    // If the Y direction is full, reposition to(Xstart, Ystart)
    if ((Ypoint  + Font->Height ) > sLCD_DIS.LCD_Dis_Page ) {
      Xpoint = Xstart;
      Ypoint = Ystart;
    }
    GUI_DisChar(Xpoint, Ypoint, * pString, Font, Color_Background, Color_Foreground);

    //The next character of the address
    pString ++;

    //The next word of the abscissa increases the font of the broadband
    Xpoint += Font->Width;
  }
}

#define  ARRAY_LEN 255

void GUI_IntToStr(int32_t Nummber, uint8_t* Str_Array)
{
  if (Str_Array == NULL) return;
  snprintf(reinterpret_cast<char*>(Str_Array), ARRAY_LEN, "%ld", static_cast<long>(Nummber));
}

/******************************************************************************
  function:	Display the string
  parameter:
	Xstart           ：X coordinate
	Ystart           : Y coordinate
	Nummber          : The number displayed
	Font             ：A structure pointer that displays a character size
	Color_Background : Select the background color of the English character
	Color_Foreground : Select the foreground color of the English character
******************************************************************************/
void GUI_DisNum(POINT Xpoint, POINT Ypoint, int32_t Nummber,
                sFONT* Font, COLOR Color_Background, COLOR Color_Foreground )
{
  if (Font == NULL) return;
  uint8_t Str_Array[ARRAY_LEN] = {0};
  uint8_t *pStr = Str_Array;

  if (Xpoint > sLCD_DIS.LCD_Dis_Column || Ypoint > sLCD_DIS.LCD_Dis_Page) {
    DEBUG("GUI_DisNum Input exceeds the normal display range");
    return;
  }

  GUI_IntToStr(Nummber, Str_Array);

  //show
  GUI_DisString_EN(Xpoint, Ypoint, reinterpret_cast<const char*>(pStr), Font, Color_Background, Color_Foreground );
}



/******************************************************************************
  function:	Display the bit map,1 byte = 8bit = 8 points
  parameter:
	Xpoint ：X coordinate
	Ypoint : Y coordinate
	pMap   : Pointing to the picture
	Width  ：Bitmap Width
	Height : Bitmap Height
  note:
	This function is suitable for bitmap, because a 16-bit data accounted for 16 points
******************************************************************************/
void GUI_Disbitmap(POINT Xpoint, POINT Ypoint, const unsigned char *pMap,
                   POINT Width, POINT Height)
{
  if (pMap == NULL) return;
  POINT i, j, byteWidth = (Width + 7) / 8;
  for (j = 0; j < Height; j++) {
    int16_t startCol = -1;
    for (i = 0; i < Width; i ++) {
      if (*(pMap + j * byteWidth + i / 8) & (128 >> (i & 7))) {
        if (startCol == -1) {
          startCol = i;
        }
      } else {
        if (startCol != -1) {
          GUI_DrawLine(Xpoint + startCol, Ypoint + j, Xpoint + i - 1, Ypoint + j, WHITE, LINE_SOLID, DOT_PIXEL_DFT);
          startCol = -1;
        }
      }
    }
    if (startCol != -1) {
      GUI_DrawLine(Xpoint + startCol, Ypoint + j, Xpoint + Width - 1, Ypoint + j, WHITE, LINE_SOLID, DOT_PIXEL_DFT);
    }
  }
}

/******************************************************************************
  function:	Display the Gray map,1 byte = 8bit = 2 points
  parameter:
	Xpoint ：X coordinate
	Ypoint : Y coordinate
	pMap   : Pointing to the picture
	Width  ：Bitmap Width
	Height : Bitmap Height
  note:
	This function is suitable for bitmap, because a 4-bit data accounted for 1 points
	Please use the Image2lcd generated array
******************************************************************************/
void GUI_DisGrayMap(POINT Xpoint, POINT Ypoint, const unsigned char *pBmp)
{
  if (pBmp == NULL) return;
  //Get the Map header Gray, width, height
  char Gray;
  Gray = *(pBmp + 1);
  POINT Height, Width;
  Width = (*(pBmp + 3) << 8) | (*(pBmp + 2));
  Height = (*(pBmp + 5) << 8) | (*(pBmp + 4));

  if (Gray == 0x04) { //Sixteen gray levels
    pBmp = pBmp + 6;
    for (POINT j = 0; j < Height; j++) {
      int16_t startCol = -1;
      COLOR currentColor = 0;
      for (POINT i = 0; i < Width / 2; i++) {
        COLOR c1 = ~(*pBmp >> 4);
        COLOR c2 = ~*pBmp;

        // Pixel 1
        if (startCol == -1) {
          startCol = i * 2;
          currentColor = c1;
        } else if (c1 != currentColor) {
          GUI_DrawLine(Xpoint + startCol, Ypoint + j, Xpoint + (i * 2) - 1, Ypoint + j, currentColor, LINE_SOLID, DOT_PIXEL_DFT);
          startCol = i * 2;
          currentColor = c1;
        }

        // Pixel 2
        if (c2 != currentColor) {
          GUI_DrawLine(Xpoint + startCol, Ypoint + j, Xpoint + (i * 2 + 1) - 1, Ypoint + j, currentColor, LINE_SOLID, DOT_PIXEL_DFT);
          startCol = i * 2 + 1;
          currentColor = c2;
        }
        pBmp++;
      }
      if (startCol != -1) {
        GUI_DrawLine(Xpoint + startCol, Ypoint + j, Xpoint + Width - 1, Ypoint + j, currentColor, LINE_SOLID, DOT_PIXEL_DFT);
      }
    }
  } else {
    DEBUG("Does not support type");
    return;
  }
}

sFONT *GUI_GetFontSize(POINT Dx, POINT Dy)
{
  if (Dx >= Font24.Width && Dy >= Font24.Height) {
    return &Font24;
  } else if (Dx >= Font20.Width && Dy >= Font20.Height) {
    return &Font20;
  } else if (Dx >= Font16.Width && Dy >= Font16.Height) {
    return &Font16;
  } else if (Dx >= Font12.Width && Dy >= Font12.Height) {
    return &Font12;
  } else if (Dx >= Font8.Width && Dy >= Font8.Height) {
    return &Font8;
  }

  DEBUG("Please change the display area size, or add a larger font to modify");
  return NULL;
}
/******************************************************************************
  function:	According to the display area adaptive display time
  parameter:
		xStart :   X direction Start coordinates
		Ystart :   Y direction Start coordinates
		Xend   :   X direction end coordinates
		Yend   :   Y direction end coordinates
		pTime  :   Pointer to the definition of the structure
		Color  :   Set show color
  note:
******************************************************************************/

void GUI_Showtime(POINT Xstart, POINT Ystart, POINT Xend, POINT Yend,
                  DEV_TIME *pTime, COLOR Color)
{
  if (pTime == NULL) return;
  const uint8_t value[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
  sFONT *Font = NULL;

  //According to the display area adaptive font size
  POINT Dx = (Xend - Xstart) / 7;//Determine the spacing between characters
  POINT Dy = Yend - Ystart;      //determine the font size
  Font = GUI_GetFontSize(Dx, Dy);

  // Time carry logic
  if (pTime->Sec >= 60) {
    pTime->Sec = 0;
    pTime->Min++;
    if (pTime->Min >= 60) {
      pTime->Min = 0;
      pTime->Hour++;
      if (pTime->Hour >= 24) {
        pTime->Hour = 0;
      }
    }
  }

  // Partial clear to prevent flickering
  if ((pTime->Sec % 10) > 0) {
    LCD_SetArealColor(Xstart + Dx * 6, Ystart, Xend, Yend, WHITE); // xx:xx:x0
  } else if ((pTime->Sec / 10) > 0) {
    LCD_SetArealColor(Xstart + Dx * 5, Ystart, Xend, Yend, WHITE); // xx:xx:00
  } else if ((pTime->Min % 10) > 0) {
    LCD_SetArealColor(Xstart + Dx * 3 + Dx / 2, Ystart, Xend, Yend, WHITE); // xx:x0:00
  } else if ((pTime->Min / 10) > 0) {
    LCD_SetArealColor(Xstart + Dx * 2 + Dx / 2, Ystart, Xend, Yend, WHITE); // xx:00:00
  } else if ((pTime->Hour % 10) > 0) {
    LCD_SetArealColor(Xstart + Dx, Ystart, Xend, Yend, WHITE); // x0:00:00
  } else {
    LCD_SetArealColor(Xstart, Ystart, Xend, Yend, WHITE); // 00:00:00
  }

  //Write data into the cache
  GUI_DisChar(Xstart                           , Ystart, value[pTime->Hour / 10], Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx                      , Ystart, value[pTime->Hour % 10], Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx  + Dx / 4 + Dx / 2   , Ystart, ':'                    , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 2 + Dx / 2         , Ystart, value[pTime->Min / 10] , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 3 + Dx / 2         , Ystart, value[pTime->Min % 10] , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 4 + Dx / 2 - Dx / 4, Ystart, ':'                    , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 5                  , Ystart, value[pTime->Sec / 10] , Font, FONT_BACKGROUND, Color);
  GUI_DisChar(Xstart + Dx * 6                  , Ystart, value[pTime->Sec % 10] , Font, FONT_BACKGROUND, Color);
}

