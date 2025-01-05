#include <stdio.h>
#include <stdlib.h>
#include "font.h"
#include "serial.h"

// Global array to store all character data
CharacterData font_data[MAX_CHARACTERS];

int load_font_file(const char *filename) {
    DEBUG_LOG("Opening font file: %s\n", filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        DEBUG_LOG("Error: Could not open font file %s\n", filename);
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
            DEBUG_LOG("Reading character ASCII %d with %d movements\n", ascii_code, num_movements);

            if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS) {
                DEBUG_LOG("Error: Invalid ASCII code %d\n", ascii_code);
                fclose(file);
                return -1;
            }

            font_data[ascii_code].ascii_code = ascii_code;
            font_data[ascii_code].num_movements = num_movements;

            // Read the movement lines for this character
            for (int i = 0; i < num_movements; i++) {
                int x, y, pen;
                if (fgets(line, sizeof(line), file) == NULL || sscanf(line, "%d %d %d", &x, &y, &pen) != 3) {
                    DEBUG_LOG("Error: Invalid movement data for ASCII %d\n", ascii_code);
                    fclose(file);
                    return -1;
                }
                font_data[ascii_code].movements[i].x = x;
                font_data[ascii_code].movements[i].y = y;
                font_data[ascii_code].movements[i].pen = pen;
                
                DEBUG_LOG("  Movement %d: X=%d, Y=%d, Pen=%d\n", i, x, y, pen);
            }
        }
    }

    fclose(file);
    DEBUG_LOG("Font file loaded successfully\n");
    return 0;
}

// Generate G-code for a character
int print_gcode_for_character(int ascii_code, float scale_factor, float x_offset, float y_offset) {
    char buffer[100];
    
    if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS || 
        font_data[ascii_code].num_movements == 0) {
        DEBUG_LOG("Error: Invalid ASCII code or no movements for character %d\n", ascii_code);
        return -1;
    }

    CharacterData *char_data = &font_data[ascii_code];
    DEBUG_LOG("Generating G-code for ASCII %d ('%c')\n", ascii_code, (char)ascii_code);

    for (int i = 0; i < char_data->num_movements; i++) {
        Movement *mov = &char_data->movements[i];
        float scaled_x = (float)mov->x * scale_factor + x_offset;
        float scaled_y = (float)mov->y * scale_factor + y_offset;

        DEBUG_PRINT_COORDS(scaled_x, scaled_y);
        DEBUG_PRINT_MOVEMENT(mov->pen);

        // Change pen state if needed

        if (i == 0 || mov->pen != char_data->movements[i-1].pen) {
            sprintf(buffer, "S%d\n", mov->pen ? 1000 : 0);
            PrintBuffer(buffer);
            WaitForReply();
        }

        sprintf(buffer, "G%d X%.3f Y%.3f\n", mov->pen ? 1 : 0, scaled_x, scaled_y);
        PrintBuffer(buffer);
        WaitForReply();
    }

    return 0;
}