#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#define MAX_CHARACTERS 128
#define MAX_MOVEMENTS 50

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
void debug_log(const char *format, ...);
void debug_print_movement(int char_index, int movement_index);
void debug_print_character_data(int char_index);
void debug_print_font_data(void);
int print_gcode_for_character(int ascii_code, float scale_factor, float x_offset, float y_offset);

#endif // FONT_HANDLER_H