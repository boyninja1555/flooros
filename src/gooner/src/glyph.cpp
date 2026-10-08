#include "glyph.hpp"
#include <cstring>

void Glyph::get(char c, std::uint8_t glyph_ptr[FONTC_HEIGHT])
{
    switch (c)
    {
    case ' ':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'A':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00011000,
                                   0b00111100,
                                   0b00100100,
                                   0b01100110,
                                   0b01000010,
                                   0b01111110,
                                   0b01000010,
                                   0b01000010,
                               },
                    FONTC_HEIGHT);
        break;
    case 'B':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111100,
                                   0b01000010,
                                   0b01000010,
                                   0b01111100,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01111100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'C':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00111100,
                                   0b01100110,
                                   0b01000010,
                                   0b01000000,
                                   0b01000000,
                                   0b01000010,
                                   0b01100110,
                                   0b00111100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'D':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111000,
                                   0b01001100,
                                   0b01000110,
                                   0b01000010,
                                   0b01000010,
                                   0b01000110,
                                   0b01001100,
                                   0b01111000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'E':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111110,
                                   0b01000000,
                                   0b01000000,
                                   0b01111110,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                                   0b01111110,
                               },
                    FONTC_HEIGHT);
        break;
    case 'F':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111110,
                                   0b01000000,
                                   0b01000000,
                                   0b01111110,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'G':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00111100,
                                   0b01100110,
                                   0b01000000,
                                   0b01001110,
                                   0b01000010,
                                   0b01000010,
                                   0b01100110,
                                   0b00111100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'H':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01111110,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                               },
                    FONTC_HEIGHT);
        break;
    case 'I':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111110,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b01111110,
                               },
                    FONTC_HEIGHT);
        break;
    case 'J':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111110,
                                   0b00000100,
                                   0b00000100,
                                   0b00000100,
                                   0b00000100,
                                   0b00000100,
                                   0b01101100,
                                   0b00111000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'K':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01000110,
                                   0b01001100,
                                   0b01011000,
                                   0b01110000,
                                   0b01011000,
                                   0b01001100,
                                   0b01000110,
                                   0b01000110,
                               },
                    FONTC_HEIGHT);
        break;
    case 'L':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                                   0b01111110,
                               },
                    FONTC_HEIGHT);
        break;
    case 'M':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01101100,
                                   0b01111100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'N':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01100010,
                                   0b01110010,
                                   0b01010010,
                                   0b01011010,
                                   0b01001010,
                                   0b01001010,
                                   0b01001110,
                                   0b01000110,
                               },
                    FONTC_HEIGHT);
        break;
    case 'O':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00111100,
                                   0b01100110,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01100110,
                                   0b00111100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'P':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111100,
                                   0b01000110,
                                   0b01000010,
                                   0b01000110,
                                   0b01111100,
                                   0b01000000,
                                   0b01000000,
                                   0b01000000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'Q':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00111100,
                                   0b01100110,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01100110,
                                   0b00111111,
                                   0b00000011,
                               },
                    FONTC_HEIGHT);
        break;
    case 'R':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111100,
                                   0b01000110,
                                   0b01000010,
                                   0b01000110,
                                   0b01111100,
                                   0b01110000,
                                   0b01011100,
                                   0b01000110,
                               },
                    FONTC_HEIGHT);
        break;
    case 'S':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00111100,
                                   0b01100110,
                                   0b01000000,
                                   0b00111100,
                                   0b00000110,
                                   0b00000010,
                                   0b01100110,
                                   0b00111100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'T':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111110,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'U':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b01100110,
                                   0b00111100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'V':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01000010,
                                   0b01000010,
                                   0b01000010,
                                   0b00100100,
                                   0b00100100,
                                   0b00100100,
                                   0b00011000,
                                   0b00011000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'W':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01010100,
                                   0b01101100,
                               },
                    FONTC_HEIGHT);
        break;
    case 'X':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01000010,
                                   0b01000010,
                                   0b00100100,
                                   0b00011000,
                                   0b00011000,
                                   0b00100100,
                                   0b01000010,
                                   0b01000010,
                               },
                    FONTC_HEIGHT);
        break;
    case 'Y':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01000100,
                                   0b01000100,
                                   0b00101000,
                                   0b00101000,
                                   0b00101000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                               },
                    FONTC_HEIGHT);
        break;
    case 'Z':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b01111110,
                                   0b00000010,
                                   0b00000110,
                                   0b00001100,
                                   0b00110000,
                                   0b01100000,
                                   0b01000000,
                                   0b01111110,
                               },
                    FONTC_HEIGHT);
        break;
    case ',':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00001000,
                                   0b00001000,
                                   0b00010000,
                               },
                    FONTC_HEIGHT);
        break;
    case '.':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00010000,
                                   0b00010000,
                               },
                    FONTC_HEIGHT);
        break;
    case '!':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00010000,
                                   0b00000000,
                                   0b00010000,
                                   0b00010000,
                               },
                    FONTC_HEIGHT);
        break;
    case '?':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00111100,
                                   0b01100110,
                                   0b00000110,
                                   0b00001100,
                                   0b00011000,
                                   0b00000000,
                                   0b00010000,
                                   0b00010000,
                               },
                    FONTC_HEIGHT);
        break;
    case ';':
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b00001000,
                                   0b00001000,
                                   0b00000000,
                                   0b00000000,
                                   0b00000000,
                                   0b00001000,
                                   0b00001000,
                                   0b00010000,
                               },
                    FONTC_HEIGHT);
        break;
    default:
        std::memcpy(glyph_ptr, (std::uint8_t[FONTC_HEIGHT]){
                                   0b11111111,
                                   0b11111111,
                                   0b11111111,
                                   0b11111111,
                                   0b11111111,
                                   0b11111111,
                                   0b11111111,
                                   0b11111111,
                               },
                    FONTC_HEIGHT);
        break;
    }
}
