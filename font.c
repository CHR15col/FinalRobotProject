#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

    // Read font file by line 
    while (fgets(line, sizeof(line), file) != NULL) {
        // Look for header line
        if (sscanf(line, "999 %d %d", &ascii_code, &num_movements) == 2) {
            DEBUG_LOG("Reading character ASCII %d with %d movements\n", ascii_code, num_movements);

            // Validate ASCII code range 
            if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS) {
                DEBUG_LOG("Error: Invalid ASCII code %d\n", ascii_code);
                fclose(file);
                return -1;
            }
            
            // Store character info
            font_data[ascii_code].ascii_code = ascii_code;
            font_data[ascii_code].num_movements = num_movements;

            // Read character movement data
            for (int i = 0; i < num_movements; i++) {
                if (fgets(line, sizeof(line), file) == NULL) {
                    DEBUG_LOG("Error: Unexpected end of file\n");
                    fclose(file);
                    return -1;
                }

                int x, y, pen;
                if (sscanf(line, "%d %d %d", &x, &y, &pen) != 3) {
                    DEBUG_LOG("Error: Invalid movement data format\n");
                    fclose(file);
                    return -1;
                }

                // Store movement data
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


int print_gcode_for_character(int ascii_code, float scale_factor, float x_offset, float y_offset) {
    char buffer[100];
    
    if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS || font_data[ascii_code].num_movements == 0) {
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


void process_text_file(const char *filename, float scale_factor) {
    DEBUG_PRINT_FILE(filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        DEBUG_LOG("Error: Could not open text file %s\n", filename);
        return;
    }

    const float TEXT_HEIGHT = 18.0f * scale_factor; 
    const float BASE_LINE_SPACING = 5.0f;  // Base spacing between lines
    const float LINE_SPACING = BASE_LINE_SPACING + TEXT_HEIGHT;  // Total line spacing
    const float MAX_WIDTH = 100.0f;  
    const float WORD_SPACING = scale_factor * 15.0f;  // Scalable space between words
    
    float x_offset = 0.0f;
    float y_offset = -TEXT_HEIGHT;  // Now start at negative text height
    
    DEBUG_LOG("Initial position: X=%.3f, Y=%.3f\n", x_offset, y_offset);
    DEBUG_LOG("Text height: %.3f, Line spacing: %.3f\n", TEXT_HEIGHT, LINE_SPACING);

    char word[256];
    while (fscanf(file, "%s", word) != EOF) {
        float word_width = 0.0f;

        // Calculate word width using last movement X coordinates
        for (size_t i = 0; i < strlen(word); i++) {
            float char_width = get_last_movement_x((int)word[i], scale_factor);
            word_width += char_width;
        }

        // Check if word fits on current line
        if (x_offset + word_width > MAX_WIDTH) {
            x_offset = 0.0f;
            y_offset -= LINE_SPACING;
            DEBUG_LOG("New line: Y=%.3f\n", y_offset);
        }

        // Print each character in word
        for (size_t i = 0; i < strlen(word); i++) {
            print_gcode_for_character((int)word[i], scale_factor, x_offset, y_offset);
            
            // Update x_offset using last movement X coordinate
            x_offset += get_last_movement_x((int)word[i], scale_factor);
            DEBUG_LOG("Character '%c' width: %.3f, New X offset: %.3f\n", 
                     word[i], get_last_movement_x((int)word[i], scale_factor), x_offset);
        }

        // Add word spacing
        x_offset += WORD_SPACING;
        DEBUG_LOG("Added word spacing: %.3f, New X offset: %.3f\n", WORD_SPACING, x_offset);
    }

    fclose(file);
}

// Get the X coordinate of the last movement
float get_last_movement_x(int ascii_code, float scale_factor) {
    if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS || 
        font_data[ascii_code].num_movements == 0) {
        return 0.0f;
    }
    
    // Get the last movement's X coordinate
    int last_idx = font_data[ascii_code].num_movements - 1;
    return font_data[ascii_code].movements[last_idx].x * scale_factor;
}