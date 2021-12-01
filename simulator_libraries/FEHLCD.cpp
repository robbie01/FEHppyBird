#include "FEHLCD.h"
#include "FEHSD.h"
#include "FEHUtility.h"
#include "FEHRandom.h"
#include "tigr.h"
#include <iostream>

#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define WINDOW_WIDTH LCD_WIDTH // TODO: Consider changing the actual window width and height to have a border around the "screen"
#define WINDOW_HEIGHT LCD_HEIGHT

#define CHAR_HEIGHT 17
#define CHAR_WIDTH 12

unsigned char FEHLCD::fontData[] = {
    0x00, 0x00, 0x00, 0x00, 0x00, // (space)
    0x00, 0x00, 0x5F, 0x00, 0x00, // !
    0x00, 0x07, 0x00, 0x07, 0x00, // "
    0x14, 0x7F, 0x14, 0x7F, 0x14, // #
    0x24, 0x2A, 0x7F, 0x2A, 0x12, // $
    0x23, 0x13, 0x08, 0x64, 0x62, // %
    0x36, 0x49, 0x55, 0x22, 0x50, // &
    0x00, 0x05, 0x03, 0x00, 0x00, // '
    0x00, 0x1C, 0x22, 0x41, 0x00, // (
    0x00, 0x41, 0x22, 0x1C, 0x00, // )
    0x08, 0x2A, 0x1C, 0x2A, 0x08, // *
    0x08, 0x08, 0x3E, 0x08, 0x08, // +
    0x00, 0x50, 0x30, 0x00, 0x00, // ,
    0x08, 0x08, 0x08, 0x08, 0x08, // -
    0x00, 0x60, 0x60, 0x00, 0x00, // .
    0x20, 0x10, 0x08, 0x04, 0x02, // /
    0x3E, 0x51, 0x49, 0x45, 0x3E, // 0
    0x00, 0x42, 0x7F, 0x40, 0x00, // 1
    0x42, 0x61, 0x51, 0x49, 0x46, // 2
    0x21, 0x41, 0x45, 0x4B, 0x31, // 3
    0x18, 0x14, 0x12, 0x7F, 0x10, // 4
    0x27, 0x45, 0x45, 0x45, 0x39, // 5
    0x3C, 0x4A, 0x49, 0x49, 0x30, // 6
    0x01, 0x71, 0x09, 0x05, 0x03, // 7
    0x36, 0x49, 0x49, 0x49, 0x36, // 8
    0x06, 0x49, 0x49, 0x29, 0x1E, // 9
    0x00, 0x36, 0x36, 0x00, 0x00, // :
    0x00, 0x56, 0x36, 0x00, 0x00, // ;
    0x00, 0x08, 0x14, 0x22, 0x41, // <
    0x14, 0x14, 0x14, 0x14, 0x14, // =
    0x41, 0x22, 0x14, 0x08, 0x00, // >
    0x02, 0x01, 0x51, 0x09, 0x06, // ?
    0x32, 0x49, 0x79, 0x41, 0x3E, // @
    0x7E, 0x11, 0x11, 0x11, 0x7E, // A
    0x7F, 0x49, 0x49, 0x49, 0x36, // B
    0x3E, 0x41, 0x41, 0x41, 0x22, // C
    0x7F, 0x41, 0x41, 0x22, 0x1C, // D
    0x7F, 0x49, 0x49, 0x49, 0x41, // E
    0x7F, 0x09, 0x09, 0x01, 0x01, // F
    0x3E, 0x41, 0x41, 0x51, 0x32, // G
    0x7F, 0x08, 0x08, 0x08, 0x7F, // H
    0x00, 0x41, 0x7F, 0x41, 0x00, // I
    0x20, 0x40, 0x41, 0x3F, 0x01, // J
    0x7F, 0x08, 0x14, 0x22, 0x41, // K
    0x7F, 0x40, 0x40, 0x40, 0x40, // L
    0x7F, 0x02, 0x04, 0x02, 0x7F, // M
    0x7F, 0x04, 0x08, 0x10, 0x7F, // N
    0x3E, 0x41, 0x41, 0x41, 0x3E, // O
    0x7F, 0x09, 0x09, 0x09, 0x06, // P
    0x3E, 0x41, 0x51, 0x21, 0x5E, // Q
    0x7F, 0x09, 0x19, 0x29, 0x46, // R
    0x46, 0x49, 0x49, 0x49, 0x31, // S
    0x01, 0x01, 0x7F, 0x01, 0x01, // T
    0x3F, 0x40, 0x40, 0x40, 0x3F, // U
    0x1F, 0x20, 0x40, 0x20, 0x1F, // V
    0x7F, 0x20, 0x18, 0x20, 0x7F, // W
    0x63, 0x14, 0x08, 0x14, 0x63, // X
    0x03, 0x04, 0x78, 0x04, 0x03, // Y
    0x61, 0x51, 0x49, 0x45, 0x43, // Z
    0x00, 0x00, 0x7F, 0x41, 0x41, // [
    0x02, 0x04, 0x08, 0x10, 0x20, // "\"
    0x41, 0x41, 0x7F, 0x00, 0x00, // ]
    0x04, 0x02, 0x01, 0x02, 0x04, // ^
    0x40, 0x40, 0x40, 0x40, 0x40, // _
    0x00, 0x01, 0x02, 0x04, 0x00, // `
    0x20, 0x54, 0x54, 0x54, 0x78, // a
    0x7F, 0x48, 0x44, 0x44, 0x38, // b
    0x38, 0x44, 0x44, 0x44, 0x20, // c
    0x38, 0x44, 0x44, 0x48, 0x7F, // d
    0x38, 0x54, 0x54, 0x54, 0x18, // e
    0x08, 0x7E, 0x09, 0x01, 0x02, // f
    0x08, 0x14, 0x54, 0x54, 0x3C, // g
    0x7F, 0x08, 0x04, 0x04, 0x78, // h
    0x00, 0x44, 0x7D, 0x40, 0x00, // i
    0x20, 0x40, 0x44, 0x3D, 0x00, // j
    0x00, 0x7F, 0x10, 0x28, 0x44, // k
    0x00, 0x41, 0x7F, 0x40, 0x00, // l
    0x7C, 0x04, 0x18, 0x04, 0x78, // m
    0x7C, 0x08, 0x04, 0x04, 0x78, // n
    0x38, 0x44, 0x44, 0x44, 0x38, // o
    0x7C, 0x14, 0x14, 0x14, 0x08, // p
    0x08, 0x14, 0x14, 0x18, 0x7C, // q
    0x7C, 0x08, 0x04, 0x04, 0x08, // r
    0x48, 0x54, 0x54, 0x54, 0x20, // s
    0x04, 0x3F, 0x44, 0x40, 0x20, // t
    0x3C, 0x40, 0x40, 0x20, 0x7C, // u
    0x1C, 0x20, 0x40, 0x20, 0x1C, // v
    0x3C, 0x40, 0x30, 0x40, 0x3C, // w
    0x44, 0x28, 0x10, 0x28, 0x44, // x
    0x0C, 0x50, 0x50, 0x50, 0x3C, // y
    0x44, 0x64, 0x54, 0x4C, 0x44, // z
    0x00, 0x08, 0x36, 0x41, 0x00, // {
    0x00, 0x00, 0x7F, 0x00, 0x00, // |
    0x00, 0x41, 0x36, 0x08, 0x00, // }
    0x08, 0x08, 0x2A, 0x1C, 0x08, // ->
    0x08, 0x1C, 0x2A, 0x08, 0x08  // <-
};

bool initialized = false;

FEHLCD LCD;
Tigr *screen;
int scale;

FEHLCD::FEHLCD()
{
    Initialize();

    _maxlines = 14;
    _maxcols = 26;

    _currentline = 0;
    _currentchar = 0;

    _orientation = North;
}

void FEHLCD::Initialize()
{
    if (initialized == false)
    {
        initialized = true;

        _Initialize();
    }
}

void FEHLCD::_Initialize()
{
    _width = LCD_WIDTH;
    _height = LCD_HEIGHT;
    _forecolor = WHITE;
    _backcolor = BLACK;

    screen = tigrWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Proteus Simulator", TIGR_FIXED & TIGR_RETINA);

    Clear();

    // FEHLCD::_Initialize() will run at the beginning of the student's program.
    // Since there is no other init function that the students call at the beginning
    // of their program, ResetTime() must be called here to initialize the timer
    // in TimeNow() and similar functions in FEHUtility.
    ResetTime();

    // Also seed the random
    Random.Seed();
}

bool FEHLCD::Touch(float *x_pos, float *y_pos)
{
    int x_int, y_int;
    bool touched = Touch(&x_int, &y_int);
    *x_pos = x_int;
    *y_pos = y_int;

    return touched;
}

bool FEHLCD::Touch(int *x_pos, int *y_pos)
{
    Update();

    int mouseButton;
    tigrMouse(screen, x_pos, y_pos, &mouseButton);

    return (mouseButton & 0x01) == 1;
}

void FEHLCD::ClearBuffer()
{
}

void FEHLCD::PrintImage(int x, int y)
{
}

void FEHLCD::PrintLogo(int x, int y)
{
}

void FEHLCD::SetOrientation(FEHLCDOrientation orientation)
{
    _orientation = orientation;
}

void FEHLCD::Clear(FEHLCDColor color)
{
    unsigned int htmlColor = ConvertFEHColorTo24Bit(color);
    Clear(htmlColor);
}

void FEHLCD::Clear(unsigned int color)
{
    SetBackgroundColor(color);
    _Clear();
}

void FEHLCD::Clear()
{
    _Clear();
}

void FEHLCD::Update()
{
    tigrUpdate(screen);

    if (tigrClosed(screen)) {
        SD.FCloseAll();
        exit(0);
    }
}

void FEHLCD::SetFontColor(FEHLCDColor color)
{
    unsigned int htmlColor = ConvertFEHColorTo24Bit(color);
    SetFontColor(htmlColor);
}

void FEHLCD::SetFontColor(unsigned int color)
{
    // Currently takes in a 24-bit color as input
    _forecolor = color;
}

void FEHLCD::SetBackgroundColor(FEHLCDColor color)
{
    unsigned int htmlColor = ConvertFEHColorTo24Bit(color);
    SetBackgroundColor(htmlColor);
}

void FEHLCD::SetBackgroundColor(unsigned int color)
{
    // Currently takes in a 24-bit color as input
    _backcolor = color;
}

void Swap(int *a, int *b)
{
    int c = *a;
    *a = *b;
    *b = c;
}

/*************************
*   DRAWING FUNCTIONS    *
*************************/

// DrawPixel takes in Proteus coordinates up to (320, 240)
void FEHLCD::DrawPixel(int x, int y)
{
    // Force X and Y to be positive
    while (x < 0)
    {
        x += _width;
    }

    while (y < 0)
    {
        y += _height;
    }

    // Force X and Y to be within the LCD boundaries
    x = x % _width;
    y = y % _height;

    TPixel color = tigrRGB((char)(_forecolor >> 16), (char)(_forecolor >> 8), (char)_forecolor);
    tigrPlot(screen, x, y, color);
}

void FEHLCD::DrawHorizontalLine(int y, int x1, int x2)
{
    if (x2 < x1) {
        Swap(&x1, &x2);
    }
    for (int i = x1; i <= x2; i++)
    {
        DrawPixel(i, y);
    }
}

void FEHLCD::DrawVerticalLine(int x, int y1, int y2)
{
    if (y2 < y1) {
        Swap(&y1, &y2);
    }
    for (int i = y1; i <= y2; i++)
    {
        DrawPixel(x, i);
    }
}

void FEHLCD::DrawLine(int x1, int y1, int x2, int y2)
{
    if (x1 == x2) {
        DrawVerticalLine(x1, y1, y2);
        return;
    }
    if (y1 == y2) {
        DrawHorizontalLine(y1, x1, x2);
        return;
    }
    // Using a float to be more precise, will cast the end result to an int
    float slope = (y2 - y1) / (x2 - x1);

    int xVal, yVal, startX, endX, startY, endY;

    // Makes the lower of the two input X values be the startX, and the higher be the endX
    startX = (x1 <= x2) ? x1 : x2;
    endX = (x1 <= x2) ? x2 : x1;

    for (xVal = startX; xVal <= endX; xVal++)
    {
        // For lines with a slope > 1, there will sometimes be multiple Y values for the same X value after the pixels
        // are truncated when casted to ints. These variables and the loop below ensure that all pixels are plotted
        startY = (int)(slope * (xVal - x1) + y1);
        endY = (int)(slope * (xVal - x1 + 1) + y1);

        // Make sure to begin at the lower Y value
        if (endY < startY)
        {
            Swap(&startY, &endY);
        }

        // Draw all of the Y values for a given X value
        for (yVal = startY; yVal <= endY; yVal++)
        {
            DrawPixel(xVal, yVal);
        }
    }
}

void FEHLCD::DrawRectangle(int x, int y, int width, int height)
{
    DrawHorizontalLine(y, x, x + width - 1);
    DrawHorizontalLine(y + height - 1, x, x + width - 1);
    DrawVerticalLine(x, y, y + height - 1);
    DrawVerticalLine(x + width - 1, y, y + height - 1);
}

void FEHLCD::FillRectangle(int x, int y, int width, int height)
{
    for (int currentX = x; currentX < x + width; currentX++)
    {
        for (int currentY = y; currentY < y + height; currentY++)
        {
            DrawPixel(currentX, currentY);
        }
    }
}

void FEHLCD::DrawCircle(int x0, int y0, int r)
{
    // This alogorithm is from wikipedia
    // It's called the "midpoint circle algorithm"
    // or the "Bresenham's circle algorithm"
    // http://en.wikipedia.org/wiki/Midpoint_circle_algorithm
    // See the page for further details
    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;

    DrawPixel(x0, y0 + r);
    DrawPixel(x0, y0 - r);
    DrawPixel(x0 + r, y0);
    DrawPixel(x0 - r, y0);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        DrawPixel(x0 + x, y0 + y);
        DrawPixel(x0 - x, y0 + y);
        DrawPixel(x0 + x, y0 - y);
        DrawPixel(x0 - x, y0 - y);
        DrawPixel(x0 + y, y0 + x);
        DrawPixel(x0 - y, y0 + x);
        DrawPixel(x0 + y, y0 - x);
        DrawPixel(x0 - y, y0 - x);
    }
}

void FEHLCD::FillCircle(int x0, int y0, int r)
{
    // This algorithm is a variant on DrawCircle.
    // Rather than draw the points around the circle,
    // We connect them with a series of lines
    // to fill in the circle.

    int f = 1 - r;
    int ddF_x = 1;
    int ddF_y = -2 * r;
    int x = 0;
    int y = r;

    DrawVerticalLine(x0, y0 - r, y0 + r);
    DrawHorizontalLine(y0, x0 - r, x0 + r);

    while (x < y)
    {
        if (f >= 0)
        {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        DrawHorizontalLine(y0 + x, x0 - y, x0 + y);
        DrawHorizontalLine(y0 - x, x0 - y, x0 + y);
        DrawVerticalLine(x0 + x, y0 - y, y0 + y);
        DrawVerticalLine(x0 - x, y0 - y, y0 + y);
    }
}

// Write information at a specific Pixel on the screen
void FEHLCD::WriteAt(const char *str, int x, int y)
{
    int i = 0;
    while (str[i] != '\0')
    {
        WriteCharAt(x, y, str[i]);
        x += CHAR_WIDTH;
        i++;
    }
}

void FEHLCD::WriteAt(int i, int x, int y)
{
    char num[50];
    sprintf(num, "%d", i);
    WriteAt(num, x, y);
}

void FEHLCD::WriteAt(float f, int x, int y)
{
    char num[50];
    int d, r;
    d = (int)f;
    r = (int)((f - d) * 1000);
    if (f < 0.)
        r = r * -1;
    if (f < 0. && f > -1.)
        sprintf(num, "-%d.%03d", d, r);
    else
        sprintf(num, "%d.%03d", d, r);
    WriteAt(num, x, y);
}

void FEHLCD::WriteAt(double d, int x, int y)
{
    WriteAt((float)d, x, y);
}

void FEHLCD::WriteAt(bool b, int x, int y)
{
    if (b)
    {
        WriteAt("true", x, y);
    }
    else
    {
        WriteAt("false", x, y);
    }
}

void FEHLCD::WriteAt(char c, int x, int y)
{
    WriteCharAt(x, y, c);
}

// Write to Row, Column

void FEHLCD::WriteRC(const char *str, int row, int col)
{
    int x, y;

    y = row * 17;
    x = col * 12;
    WriteAt(str, x, y);
}

void FEHLCD::WriteRC(int i, int row, int col)
{
    int x, y;

    y = row * 17;
    x = col * 12;
    WriteAt(i, x, y);
}

void FEHLCD::WriteRC(float f, int row, int col)
{
    int x, y;

    y = row * 17;
    x = col * 12;
    WriteAt(f, x, y);
}

void FEHLCD::WriteRC(double d, int row, int col)
{
    int x, y;

    y = row * 17;
    x = col * 12;
    WriteAt(d, x, y);
}

void FEHLCD::WriteRC(bool b, int row, int col)
{
    int x, y;

    y = row * 17;
    x = col * 12;
    WriteAt(b, x, y);
}

void FEHLCD::WriteRC(char c, int row, int col)
{
    int x, y;

    y = row * 17;
    x = col * 12;
    WriteAt(c, x, y);
}

// Write to the screen
void FEHLCD::Write(const char *str)
{
    int i = 0;
    CheckLine();
    while (str[i] != '\0')
    {
        WriteChar(_currentline, _currentchar, str[i]);
        NextChar();
        i++;
    }
}

void FEHLCD::Write(int i)
{
    char num[50];
    sprintf(num, "%d", i);
    Write(num);
}

void FEHLCD::Write(float f)
{
    char num[50];
    int d, r;
    if (f >= 0)
    {
        d = (int)f;
        r = (int)((f - d) * 1000);
        sprintf(num, "%d.%03d", d, r);
    }
    else
    {
        f *= -1;
        d = (int)f;
        r = (int)((f - d) * 1000);
        sprintf(num, "-%d.%03d", d, r);
    }
    Write(num);
}

void FEHLCD::Write(double d)
{
    Write((float)d);
}

void FEHLCD::Write(bool b)
{
    if (b)
    {
        Write("true");
    }
    else
    {
        Write("false");
    }
}

void FEHLCD::Write(char c)
{
    CheckLine();
    WriteChar(_currentline, _currentchar, c);
    NextChar();
}

// Write to the screeen and advance to next line
void FEHLCD::WriteLine(const char *str)
{
    CheckLine();
    Write(str);
    NextLine();
}

void FEHLCD::WriteLine(int i)
{
    CheckLine();
    Write(i);
    NextLine();
}

void FEHLCD::WriteLine(float f)
{
    CheckLine();
    Write(f);
    NextLine();
}

void FEHLCD::WriteLine(double d)
{
    CheckLine();
    Write(d);
    NextLine();
}

void FEHLCD::WriteLine(bool b)
{
    CheckLine();
    Write(b);
    NextLine();
}

void FEHLCD::WriteLine(char c)
{
    CheckLine();
    Write(c);
    NextLine();
}

typedef struct regColVal
{
    uint32_t BVal;
    uint32_t CVal;
    uint32_t DVal;
} RegisterColorValues;

void FEHLCD::TS_SPI_Init()
{
}

int FEHLCD::abs(int n)
{
    if (n < 0)
    {
        return -n;
    }
    else
    {
        return n;
    }
}

void FEHLCD::_Clear()
{
    // Currently takes in a 24-bit color as input
    TPixel rgbValues = tigrRGB((char)(_backcolor >> 16), (char)(_backcolor >> 8), (char)_backcolor);
    tigrClear(screen, rgbValues);
}

void FEHLCD::_RepeatColor()
{
}

void FEHLCD::_BackPixel()
{
}

void FEHLCD::_ForePixel()
{
}

void FEHLCD::SetRegisterColorValues()
{
}

void FEHLCD::WriteChar(int row, int col, char c)
{
    WriteCharAt(2 + col * CHAR_WIDTH, 3 + row * CHAR_HEIGHT, c);
}

void FEHLCD::WriteCharAt(int x, int y, char c)
{
    // Force X and Y to be positive
    while (x < 0)
    {
        x += _width;
    }

    while (y < 0)
    {
        y += _height;
    }

    // Force X and Y to be within the LCD boundaries
    x = x % _width;
    y = y % _height;

    // Increment the line if c is the newline character
    if (c == '\n')
    {
        CheckLine();
        NextLine();
        return;
    }

    // Force the input character to be within the supported character list. c = 32 is a space.
    if (c > 125 || c < 32)
    {
        c = 32;
    }

    // Shift all of the input character values down by 32 so that they can be used as indexes of the fontData array
    c -= 32;

    // Look up the bitmap data out of the fontData table
    // Each entry in the fontData table corresponds
    // to a column of pixels in the 5x7 bitmapped character
    unsigned char charData[5];
    charData[0] = fontData[5 * c];
    charData[1] = fontData[5 * c + 1];
    charData[2] = fontData[5 * c + 2];
    charData[3] = fontData[5 * c + 3];
    charData[4] = fontData[5 * c + 4];

    // Plot the character data pixel-by-pixel. Each character will be doubled in size from the fontData input,
    // becoming 10x14 instead of 5x7. There will be 2 additional columns of blank space to the left of each character
    // and 3 additional rows of blank space below, making each character 12x17 pixels.

    // 2 blank columns
    x += 2;

    for (int col = 0; col < 5; col++)
    {
        for (int row = 0; row < 7; row++)
        {
            // If the current pixel is a 1 in the fontData bitmap
            if (((charData[col] >> row) & 0x01) == 1)
            {
                // Draw a 2x2 rectangle to represent each pixel since sizes are doubled
                FillRectangle(x + col * 2, y + row * 2, 2, 2);
            }
        }
    }
}

void FEHLCD::WriteIndex(unsigned char index)
{
}

void FEHLCD::WriteParameter(unsigned char param)
{
}

void FEHLCD::RepeatColor()
{
}

unsigned int FEHLCD::ConvertFEHColorTo24Bit(FEHLCDColor color)
{
    unsigned int htmlColor;

    switch (color)
    {
    case Black:
        htmlColor = BLACK;
        break;
    case White:
        htmlColor = WHITE;
        break;
    case Red:
        htmlColor = RED;
        break;
    case Green:
        htmlColor = GREEN;
        break;
    case Yellow:
        htmlColor = YELLOW;
        break;
    case Blue:
        htmlColor = BLUE;
        break;
    case Scarlet:
        htmlColor = SCARLET;
        break;
    case Gray:
        htmlColor = GRAY;
        break;
    default:
        htmlColor = BLACK;
    }

    return htmlColor;
}

unsigned int FEHLCD::Convert24BitColorTo16Bit(unsigned int color)
{
    unsigned char r = (color & 0xFF0000u) >> 16;
    unsigned char g = (color & 0x00FF00u) >> 8;
    unsigned char b = (color & 0x0000FFu);
    return ConvertRGBColorTo16Bit(r, g, b);
}

unsigned int FEHLCD::ConvertRGBColorTo16Bit(unsigned char r, unsigned char g, unsigned char b)
{
    unsigned int ru = r >> 2;
    unsigned int gu = g >> 2;
    unsigned int bu = b >> 2;
    return (ru << 12) | (gu << 6) | bu;
}

void FEHLCD::NextLine()
{
    if (_currentchar > 0)
    {
        _currentline++;
        _currentchar = 0;
    }
}

void FEHLCD::CheckLine()
{
    if (_currentline >= _maxlines)
    {
        _currentline = 0;
        _Clear();
    }
}

void FEHLCD::NextChar()
{
    _currentchar++;
    if (_currentchar == _maxcols)
    {
        NextLine();
        CheckLine();
    }
}

void FEHLCD::SetDrawRegion(int x, int y, int width, int height)
{
}

/*
    FEHIcon
*/
namespace FEHIcon
{
    Icon::Icon()
    {
    }

    /* Icon function to set position, size, label, and color */
    void Icon::SetProperties(char name[20], int start_x, int start_y, int w, int h, unsigned int c, unsigned int tc)
    {
        strcpy(label, name);
        x_start = start_x;
        y_start = start_y;
        width = w;
        height = h;
        x_end = x_start + width;
        y_end = y_start + height;
        color = c;
        textcolor = tc;
        set = 0;
    }

    /* Icon function to draw it and write label */
    void Icon::Draw()
    {
        LCD.SetFontColor(color);
        LCD.DrawRectangle(x_start, y_start, width, height);
        LCD.SetFontColor(textcolor);
        LCD.WriteAt(label, x_start + ((width - (strlen(label) * 12)) / 2), y_start + ((height - 17) / 2)); // equation to center text inside the icon
    }

    /* Icon function to make the icon selected and set */
    void Icon::Select()
    {
        LCD.SetFontColor(color);
        LCD.DrawRectangle(x_start + 1, y_start + 1, width - 2, height - 2);
        LCD.DrawRectangle(x_start + 2, y_start + 2, width - 4, height - 4);
        LCD.DrawRectangle(x_start + 3, y_start + 3, width - 6, height - 6);
        set = 1;
    }

    /* Icon function to make the icon deselected and not set */
    void Icon::Deselect()
    {
        LCD.SetFontColor(BLACK);
        LCD.DrawRectangle(x_start + 3, y_start + 3, width - 6, height - 6);
        LCD.DrawRectangle(x_start + 2, y_start + 2, width - 4, height - 4);
        LCD.DrawRectangle(x_start + 1, y_start + 1, width - 2, height - 2);
        set = 0;
    }

    /* Icon function to see if it has been pressed */
    int Icon::Pressed(float x, float y, int mode)
    {
        if (x >= x_start && x <= x_end && y >= y_start && y <= y_end)
        {
            LCD.Touch(&x, &y);
            if (x >= x_start && x <= x_end && y >= y_start && y <= y_end) // check twice to avoid buggy touch screen issues
            {
                if (!mode) // if mode is 0, then alternate selecting and deselecting as it is pressed again and again; otherwise, the icon does not select and deselect
                {
                    if (!set)
                    {
                        Select();
                    }
                    else if (set)
                    {
                        Deselect();
                    }
                }
                return 1;
            }
        }
        return 0;
    }

    /* Icon function to wait while it is pressed */
    int Icon::WhilePressed(float xi, float yi)
    {
        float x = xi, y = yi;
        while (Pressed(x, y, 1))
        {
            LCD.Touch(&x, &y);
        }

        return 0;
    }

    /* Icon function to change the label of an icon with a string */
    void Icon::ChangeLabelString(const char new_label[])
    {
        if (strcmp(label, new_label))
        {
            strcpy(label, new_label);
            LCD.SetFontColor(BLACK);
            LCD.FillRectangle(x_start + 1, y_start + 1, width - 2, height - 2);
            Draw();
        }
    }

    /* Icon function to change the label of an icon with a float */
    void Icon::ChangeLabelFloat(float val)
    {
        int length_i = strlen(label);
        int d, r;
        /* Convert float to string so it can be auto-centered in icon */
        if (val >= 0)
        {
            d = (int)val;
            r = (int)((val - d) * 1000);
            sprintf(label, "%d.%03d", d, r);
        }
        else
        {
            val *= -1;
            d = (int)val;
            r = (int)((val - d) * 1000);
            sprintf(label, "-%d.%03d", d, r);
        }
        /* If the new label is not the same length as the old one, then erase the old one so that it does not show up behind the new one */
        if ((int) strlen(label) != length_i)
        {
            LCD.SetFontColor(BLACK);
            LCD.FillRectangle(x_start + 1, y_start + 1, width - 2, height - 2);
        }
        Draw();
    }

    /* Icon function to change the label of an icon with a int */
    void Icon::ChangeLabelInt(int val)
    {
        int length_i = strlen(label);

        /* Convert int to string so it can be auto-centered in icon */
        sprintf(label, "%d", val);

        /* If the new label is not the same length as the old one, then erase the old one so that it does not show up behind the new one */
        if ((int) strlen(label) != length_i)
        {
            LCD.SetFontColor(BLACK);
            LCD.FillRectangle(x_start + 1, y_start + 1, width - 2, height - 2);
        }
        Draw();
    }

    /* Function to draw an array of icons in a given space and size and label them */
    void DrawIconArray(Icon icon[], int rows, int cols, int top, int bot, int left, int right, char labels[][20], unsigned int col, unsigned int txtcol)
    {
        int xs = left;
        int ys = top;
        float total_w = (320. - left - right);
        float total_h = (240. - top - bot);
        int w = total_w / cols;
        int h = total_h / rows;
        int nx, ny, N = 0;
        for (ny = 1; ny <= rows; ny++)
        {
            for (nx = 1; nx <= cols; nx++)
            {
                icon[N].SetProperties(labels[N], xs, ys, w, h, col, txtcol);
                icon[N].Draw();
                N = N + 1;
                xs = xs + w;
            }
            ys = ys + h;
            xs = left;
        }
    }

} // ns FEHIcon