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


float get_character_width(int ascii_code, float scale_factor) {
    if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS || font_data[ascii_code].num_movements == 0) {
        return 0.0f;
    }
    
    CharacterData *char_data = &font_data[ascii_code];
    float max_x = 0.0f;

    // Find the max positive x coordinate
    for (int i = 0; i < char_data->num_movements; i++) {
        if (char_data->movements[i].x > max_x) {
            max_x = (float)char_data->movements[i].x;
        }
    }

    return max_x * scale_factor;
}


void process_text_file(const char *filename, float scale_factor) {
    DEBUG_PRINT_FILE(filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        DEBUG_LOG("Error: Could not open text file %s\n", filename);
        return;
    }

    float x_offset = 0.0f;
    float y_offset = 0.0f;
    const float MAX_WIDTH = 100.0f;  // Max writing space
    const float BASE_LINE_SPACING = 5.0f;  // Spacing constraint between lines (mm)
    const float TEXT_HEIGHT = 18.0f * scale_factor;  // Scaled height of the font
    const float LINE_SPACING = BASE_LINE_SPACING + TEXT_HEIGHT;  // New line origin includes scaling
    const float WORD_SPACING = scale_factor * 5.0f;
    
    DEBUG_LOG("Line spacing set to: %.3f (base: %.3f + text height: %.3f)\n", LINE_SPACING, BASE_LINE_SPACING, TEXT_HEIGHT);
    
    char word[256];
    while (fscanf(file, "%s", word) != EOF) {
        float word_width = 0.0f;
        
        // Calculate word width before printing
        for (size_t i = 0; i < strlen(word); i++) {
            word_width += get_character_width((int)word[i], scale_factor);
        }
        word_width += (strlen(word) - 1) * WORD_SPACING;  // Add spacing between characters

        // Check to move to next line
        if (x_offset + word_width > MAX_WIDTH) {
            x_offset = 0.0f;
            y_offset -= LINE_SPACING;  // Use line spacing calculation
            DEBUG_LOG("Moving to new line at Y=%.3f\n", y_offset);
        }

        // Print each character in the word
        for (size_t i = 0; i < strlen(word); i++) {
            if (print_gcode_for_character((int)word[i], scale_factor, x_offset, y_offset) == 0) {
                x_offset += get_character_width((int)word[i], scale_factor);
                if (i < strlen(word) - 1) {  // Add spacing between characters, but not after last character
                    x_offset += WORD_SPACING;
                }
            }
        }
        // Add space between words
        x_offset += WORD_SPACING;
    }
    fclose(file);
}