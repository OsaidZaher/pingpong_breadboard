
// display.h
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include <stdint.h>

// Display dimensions (matches game constants)
#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 160

// Common colors in RGB565 format
#define BLACK 0x0000
#define WHITE 0xFFFF
#define RED 0xF800
#define GREEN 0x07E0
#define BLUE 0x001F
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define GRAY 0x8410

// Function prototypes
void display_begin(void);

// Basic drawing functions
void putPixel(uint16_t x, uint16_t y, uint16_t colour);
void drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour);
void drawRectangle(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour);
void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t colour);
void drawCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour);
void fillCircle(uint16_t x0, uint16_t y0, uint16_t radius, uint16_t colour);

// Image handling
void putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, 
             const uint16_t *Image, int hOrientation, int vOrientation);

// Text functions
void printText(const char *Text, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
void printNumber(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);


// Color conversion
uint16_t RGBToWord(uint16_t R, uint16_t G, uint16_t B);

#endif // _DISPLAY_H_