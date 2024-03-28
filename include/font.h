// https://github.com/fantasticmao/font6x8, made smaller because why not

// 8x8 monochrome bitmap font for rendering
// =========================================
//
// A collection of header files containing a 8x8 bitmap font.
//
// font8x8.h contains all available characters
// font8x8_basic.h contains unicode points U+0000 - U+007F
// font8x8_latin.h contains unicode points U+0000 - U+00FF
//
// Author: Daniel Hepper <daniel@hepper.net>
// License: Public Domain
//
// Encoding
// ========
// Every character in the font is encoded row-wise in 8 bytes.
//
// The least significant bit of each byte corresponds to the first pixel in a
//  row.
//
// The character 'A' (0x41 / 65) is encoded as
// { 0x0C, 0x1E, 0x33, 0x33, 0x3F, 0x33, 0x33, 0x00}
//
//     0x0C => 0000 1100 => ..XX....
//     0X1E => 0001 1110 => .XXXX...
//     0x33 => 0011 0011 => XX..XX..
//     0x33 => 0011 0011 => XX..XX..
//     0x3F => 0011 1111 => xxxxxx..
//     0x33 => 0011 0011 => XX..XX..
//     0x33 => 0011 0011 => XX..XX..
//     0x00 => 0000 0000 => ........
//
// To access the nth pixel in a row, right-shift by n.
//
//                          . . X X . . . .
//                          | | | | | | | |
//     (0x0C >> 0) & 1 == 0-+ | | | | | | |
//     (0x0C >> 1) & 1 == 0---+ | | | | | |
//     (0x0C >> 2) & 1 == 1-----+ | | | | |
//     (0x0C >> 3) & 1 == 1-------+ | | | |
//     (0x0C >> 4) & 1 == 0---------+ | | |
//     (0x0C >> 5) & 1 == 0-----------+ | |
//     (0x0C >> 6) & 1 == 0-------------+ |
//     (0x0C >> 7) & 1 == 0---------------+
//
// Renderer
// ========
// To visualize the font, a simple renderer is included in render.c
//
//     $ gcc render.c -o render
//     $ ./render 65
//       XX
//      XXXX
//     XX  XX
//     XX  XX
//     XXXXXX
//     XX  XX
//     XX  XX
//
// Credits
// =======
// These header files are directly derived from an assembler file fetched from:
// http://dimensionalrift.homelinux.net/combuster/mos3/?p=viewsource&file=/modules/gfx/font8_8.asm
//
// Original header:
//
// ; Summary: font8_8.asm
// ; 8x8 monochrome bitmap fonts for rendering
// ;
// ; Author:
// ;     Marcel Sondaar
// ;     International Business Machines (public domain VGA fonts)
// ;
// ; License:
// ;     Public Domain
// ;

// 6x8 font
// #define FONT_WIDTH 6
// #define FONT_HEIGHT 8
//
// const char font[11][FONT_WIDTH] = {
//     {0x00, 0x00, 0x1E, 0x21, 0x21, 0x1E}, // 0x30 (0)
//     {0x00, 0x00, 0x00, 0x22, 0x3F, 0x20}, // 0x31 (1)
//     {0x00, 0x00, 0x22, 0x31, 0x29, 0x26}, // 0x32 (2)
//     {0x00, 0x00, 0x12, 0x21, 0x25, 0x1A}, // 0x33 (3)
//     {0x00, 0x00, 0x0C, 0x0A, 0x3F, 0x08}, // 0x34 (4)
//     {0x00, 0x00, 0x17, 0x25, 0x25, 0x19}, // 0x35 (5)
//     {0x00, 0x00, 0x1C, 0x26, 0x25, 0x18}, // 0x36 (6)
//     {0x00, 0x00, 0x01, 0x39, 0x05, 0x03}, // 0x37 (7)
//     {0x00, 0x00, 0x1A, 0x25, 0x25, 0x1A}, // 0x38 (8)
//     {0x00, 0x00, 0x06, 0x09, 0x29, 0x1E}, // 0x39 (9)
//     {0x00, 0x00, 0x00, 0x14, 0x00, 0x00}, // 0x3A (:)
// };

// 8x8 font
#define FONT_WIDTH 8
#define FONT_HEIGHT 8
const char font[11][FONT_WIDTH] = {
    {0x3E, 0x63, 0x73, 0x7B, 0x6F, 0x67, 0x3E, 0x00}, // U+0030 (0)
    {0x0C, 0x0E, 0x0C, 0x0C, 0x0C, 0x0C, 0x3F, 0x00}, // U+0031 (1)
    {0x1E, 0x33, 0x30, 0x1C, 0x06, 0x33, 0x3F, 0x00}, // U+0032 (2)
    {0x1E, 0x33, 0x30, 0x1C, 0x30, 0x33, 0x1E, 0x00}, // U+0033 (3)
    {0x38, 0x3C, 0x36, 0x33, 0x7F, 0x30, 0x78, 0x00}, // U+0034 (4)
    {0x3F, 0x03, 0x1F, 0x30, 0x30, 0x33, 0x1E, 0x00}, // U+0035 (5)
    {0x1C, 0x06, 0x03, 0x1F, 0x33, 0x33, 0x1E, 0x00}, // U+0036 (6)
    {0x3F, 0x33, 0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x00}, // U+0037 (7)
    {0x1E, 0x33, 0x33, 0x1E, 0x33, 0x33, 0x1E, 0x00}, // U+0038 (8)
    {0x1E, 0x33, 0x33, 0x3E, 0x30, 0x18, 0x0E, 0x00}, // U+0039 (9)
    {0x00, 0x0C, 0x0C, 0x00, 0x00, 0x0C, 0x0C, 0x00}, // U+003A (:)
};
