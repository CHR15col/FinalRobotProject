#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#include "debug.h"

#define MAX_CHARACTERS 128
#define MAX_MOVEMENTS 26

// Structure to store movement data
typedef struct {
    int x;
    int y;
    int pen;
} Movement;

// Structure to store character data
typedef struct {
    int ascii_code;
    int num_movements;
    Movement movements[MAX_MOVEMENTS];
} CharacterData;

// Global array to store font data
extern CharacterData font_data[MAX_CHARACTERS];

// Function prototypes
int load_font_file(const char *filename);
int print_gcode_for_character(int ascii_code, float scale_factor, float x_offset, float y_offset);
float get_character_width(int ascii_code, float scale_factor);
void process_text_file(const char *filename, float scale_factor);

#endif // FONT_HANDLER_H