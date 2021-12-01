#ifndef FEHLCD_H
#define FEHLCD_H

// #include "spi.h"
// #include "derivative.h"
#include <stdint.h>
#include <cstring>
#include "LCDColors.h"

class FEHLCD
{
public:
    typedef enum
    {
        Black = 0,
        White,
        Red,
        Green,
        Yellow,
        Blue,
        Scarlet,
        Gray
    } FEHLCDColor;

    typedef enum
    {
        North = 0,
        South,
        East,
        West
    } FEHLCDOrientation;

    FEHLCD();

    bool Touch(float *x_pos, float *y_pos);
    bool Touch(int *x_pos, int *y_pos);

    void ClearBuffer();

    void PrintImage(int x, int y);
    void PrintLogo(int x, int y);
    void Initialize();
    void SetOrientation(FEHLCDOrientation orientation);

    void Clear(FEHLCDColor color);
    void Clear(unsigned int color);
    void Clear();

    void Update();

    void SetFontColor(FEHLCDColor color);
    void SetFontColor(unsigned int color);
    void SetBackgroundColor(FEHLCDColor color);
    void SetBackgroundColor(unsigned int color);

    // Drawing Functions
    void DrawPixel(int x, int y);
    void DrawHorizontalLine(int y, int x1, int x2);
    void DrawVerticalLine(int x, int y1, int y2);
    void DrawLine(int x1, int y1, int x2, int y2);
    void DrawRectangle(int x, int y, int width, int height);
    void FillRectangle(int x, int y, int width, int height);
    void DrawCircle(int x0, int y0, int r);
    void FillCircle(int x0, int y0, int r);

    // Write information at a specific Pixel on the screen
    void WriteAt(const char *str, int x, int y);
    void WriteAt(int i, int x, int y);
    void WriteAt(float f, int x, int y);
    void WriteAt(double d, int x, int y);
    void WriteAt(bool b, int x, int y);
    void WriteAt(char c, int x, int y);

    // Write to Row, Column

    void WriteRC(const char *str, int row, int col);
    void WriteRC(int i, int row, int col);
    void WriteRC(float f, int row, int col);
    void WriteRC(double d, int row, int col);
    void WriteRC(bool b, int row, int col);
    void WriteRC(char c, int row, int col);

    // Write to the screen
    void Write(const char *str);
    void Write(int i);
    void Write(float f);
    void Write(double d);
    void Write(bool b);
    void Write(char c);

    // Write to the screeen and advance to next line
    void WriteLine(const char *str);
    void WriteLine(int i);
    void WriteLine(float f);
    void WriteLine(double d);
    void WriteLine(bool b);
    void WriteLine(char c);

private:
    typedef struct regColVal
    {
        uint32_t BVal;
        uint32_t CVal;
        uint32_t DVal;
    } RegisterColorValues;

    void TS_SPI_Init();

    int abs(int);

    void _Initialize();
    void _Clear();
    void _RepeatColor();
    void _BackPixel();
    void _ForePixel();
    void SetRegisterColorValues();

    void WriteChar(int row, int col, char c);
    void WriteCharAt(int x, int y, char c);

    void WriteIndex(unsigned char index);
    void WriteParameter(unsigned char param);
    void RepeatColor();

    unsigned int ConvertFEHColorTo24Bit(FEHLCDColor color);
    unsigned int Convert24BitColorTo16Bit(unsigned int color);
    unsigned int ConvertRGBColorTo16Bit(unsigned char r, unsigned char g, unsigned char b);

    void NextLine();
    void CheckLine();
    void NextChar();
    void SetDrawRegion(int x, int y, int width, int height);

    FEHLCDOrientation _orientation;

    int _maxlines;
    int _maxcols;
    int _width;
    int _height;
    int _currentline;
    int _currentchar;
    unsigned int _forecolor;
    unsigned int _backcolor;
    // RegisterColorValues foreRegisterValues, backRegisterValues;

    // int lastx;
    // int lasty;

    static unsigned char fontData[];
};

namespace FEHIcon
{
    /* Class definition for software icons */
    class Icon
    {
    private:
        int x_start, x_end;
        int y_start, y_end;
        int width;
        int height;
        unsigned int color;
        unsigned int textcolor;
        char label[200];
        int set;

    public:
        Icon();
        void SetProperties(char name[20], int start_x, int start_y, int w, int h, unsigned int c, unsigned int tc);
        void Draw();
        void Select();
        void Deselect();
        int Pressed(float x, float y, int mode);
        int WhilePressed(float xi, float yi);
        void ChangeLabelString(const char new_label[20]);
        void ChangeLabelFloat(float val);
        void ChangeLabelInt(int val);
    };

    /* Function prototype for drawing an array of icons in a rows by cols array with top, bot, left, and right margins from edges of screen, labels for each icon from top left across each row to the bottom right, and color for the rectangle and the text color */
    void DrawIconArray(Icon icon[], int rows, int cols, int top, int bot, int left, int right, char labels[][20], unsigned int col, unsigned int txtcol);
}

extern FEHLCD LCD;

#endif // FEHLCD_H
