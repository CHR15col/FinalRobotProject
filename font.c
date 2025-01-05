#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "font.h"
#include "serial.h"

// Global array to store all character data
CharacterData font_data[MAX_CHARACTERS];

// Debug logging function
void debug_log(const char *format, ...) {
    va_list args;
    va_start(args, format);
    printf("[DEBUG] ");
    vprintf(format, args);
    va_end(args);
}

// Print movement data for debugging
void debug_print_movement(int char_index, int movement_index) {
    if (char_index < 0 || char_index >= MAX_CHARACTERS ||movement_index < 0 || movement_index >= font_data[char_index].num_movements) {
        debug_log("Invalid indices for movement data\n");
        return;
    }
    
    Movement *mov = &font_data[char_index].movements[movement_index];
    debug_log("  Movement %d: X=%d, Y=%d, Pen=%d\n", movement_index, mov->x, mov->y, mov->pen);
}

// Print character data for debugging
void debug_print_character_data(int char_index) {
    if (char_index < 0 || char_index >= MAX_CHARACTERS) {
        debug_log("Invalid character index\n");
        return;
    }

    CharacterData *char_data = &font_data[char_index];
    debug_log("Character ASCII %d with %d movements\n", char_data->ascii_code, char_data->num_movements);
    
    for (int i = 0; i < char_data->num_movements; i++) {
        debug_print_movement(char_index, i);
    }
}

// Print all font data for debugging
void debug_print_font_data(void) {
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        if (font_data[i].num_movements > 0) {
            debug_print_character_data(i);
        }
    }
}

// Load font data from file
int load_font_file(const char *filename) {
    debug_log("Opening font file: %s\n", filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        debug_log("Error: Could not open font file %s\n", filename);
        return -1;
    }

    // Initialize all characters with zero movements
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        font_data[i].ascii_code = i;
        font_data[i].num_movements = 0;
    }

    char line[256];
    int ascii_code, num_movements;
    
    // Read the file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        // Check if this is a header line (starts with 999)
        if (sscanf(line, "999 %d %d", &ascii_code, &num_movements) == 2) {
            debug_log("Reading character ASCII %d with %d movements\n", ascii_code, num_movements);

            if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS) {
                debug_log("Error: Invalid ASCII code %d\n", ascii_code);
                fclose(file);
                return -1;
            }

            font_data[ascii_code].ascii_code = ascii_code;
            font_data[ascii_code].num_movements = num_movements;

            // Read the movement lines for this character
            for (int i = 0; i < num_movements; i++) {
                int x, y, pen;
                if (fgets(line, sizeof(line), file) == NULL || sscanf(line, "%d %d %d", &x, &y, &pen) != 3) {
                    debug_log("Error: Invalid movement data for ASCII %d\n", ascii_code);
                    fclose(file);
                    return -1;
                }
                font_data[ascii_code].movements[i].x = x;
                font_data[ascii_code].movements[i].y = y;
                font_data[ascii_code].movements[i].pen = pen;
                
                debug_log("  Movement %d: X=%d, Y=%d, Pen=%d\n", i, x, y, pen);
            }
        }
    }

    fclose(file);
    debug_log("Font file loaded successfully\n");
    return 0;
}

// Generate G-code for a character
int print_gcode_for_character(int ascii_code, float scale_factor, float x_offset, float y_offset) {
    char buffer[100];
    
    if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS || 
        font_data[ascii_code].num_movements == 0) {
        debug_log("Error: Invalid ASCII code or no movements for character %d\n", ascii_code);
        return -1;
    }

    CharacterData *char_data = &font_data[ascii_code];
    debug_log("Generating G-code for ASCII %d\n", ascii_code);

    for (int i = 0; i < char_data->num_movements; i++) {
        Movement *mov = &char_data->movements[i];
        float scaled_x = mov->x * scale_factor + x_offset;
        float scaled_y = mov->y * scale_factor + y_offset;

        // Change pen state if needed
        if (i == 0 || mov->pen != char_data->movements[i-1].pen) {
            sprintf(buffer, "S%d\n", mov->pen ? 1000 : 0);
            PrintBuffer(buffer);
            WaitForReply();
        }

        // Move to position
        sprintf(buffer, "G%d X%.3f Y%.3f\n", mov->pen ? 1 : 0, scaled_x, scaled_y);
        PrintBuffer(buffer);
        WaitForReply();
    }

    return 0;
}