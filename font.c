// font.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "rs232.h"
#include "font.h"
#include "serial.h"


CharacterData font_data[MAX_CHARACTERS];

// Initialize font data array
void initialize_font_data(void) {
    for (int i = 0; i < MAX_CHARACTERS; i++) {
        font_data[i].ascii_code = i;
        font_data[i].num_movements = 0;
    }
}

// Process character movement data from file
int process_character_movements(FILE* file, int ascii_code, int num_movements) {
    char line[256];
    
    for (int i = 0; i < num_movements; i++) {
        if (fgets(line, sizeof(line), file) == NULL) {
            DEBUG_LOG("Error: Unexpected end of file\n");
            return -1;
        }

        int x, y, pen;
        if (sscanf(line, "%d %d %d", &x, &y, &pen) != 3) {
            DEBUG_LOG("Error: Invalid movement data format\n");
            return -1;
        }

        font_data[ascii_code].movements[i].x = x;
        font_data[ascii_code].movements[i].y = y;
        font_data[ascii_code].movements[i].pen = pen;
        
        DEBUG_LOG("  Movement %d: X=%d, Y=%d, Pen=%d\n", i, x, y, pen);
    }
    return 0;
}

int load_font_file(const char *filename) {
    DEBUG_LOG("Opening font file: %s\n", filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        DEBUG_LOG("Error: Could not open font file %s\n", filename);
        return -1;
    }

    initialize_font_data();

    char line[256];
    int ascii_code, num_movements;

    while (fgets(line, sizeof(line), file) != NULL) {
        if (sscanf(line, "999 %d %d", &ascii_code, &num_movements) == 2) {
            DEBUG_LOG("Reading character ASCII %d with %d movements\n", ascii_code, num_movements);

            if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS) {
                DEBUG_LOG("Error: Invalid ASCII code %d\n", ascii_code);
                fclose(file);
                return -1;
            }
            
            font_data[ascii_code].ascii_code = ascii_code;
            font_data[ascii_code].num_movements = num_movements;

            if (process_character_movements(file, ascii_code, num_movements) != 0) {
                fclose(file);
                return -1;
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
            SendCommands(buffer);
        }

        sprintf(buffer, "G%d X%.3f Y%.3f\n", mov->pen ? 1 : 0, scaled_x, scaled_y);
        SendCommands(buffer);
    }
    return 0;
}

float calculate_word_width(const char* word, float scale_factor) {
    float word_width = 0.0f;
    for (size_t i = 0; i < strlen(word); i++) {
        word_width += get_character_width((int)word[i], scale_factor);
    }
    return word_width;
}

void update_print_position(float* x_offset, float* y_offset, float word_width, float line_spacing, float max_width) {
    if (*x_offset + word_width > max_width) {
        *x_offset = 0.0f;
        *y_offset -= line_spacing;
        DEBUG_LOG("New line: Y=%.3f\n", *y_offset);
    }
}

void print_word(const char* word, float scale_factor, float* x_offset, float* y_offset) {
    for (size_t i = 0; i < strlen(word); i++) {
        print_gcode_for_character((int)word[i], scale_factor, *x_offset, *y_offset);
        *x_offset += get_character_width((int)word[i], scale_factor);
        DEBUG_LOG("Character '%c' width: %.3f, New X offset: %.3f\n", 
                 word[i], get_character_width((int)word[i], scale_factor), *x_offset);
    }
}

void process_text_file(const char *filename, float scale_factor) {
    DEBUG_PRINT_FILE(filename);
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        DEBUG_LOG("Error: Could not open text file %s\n", filename);
        return;
    }

    const float TEXT_HEIGHT = 18.0f * scale_factor;
    const float LINE_SPACING = BASE_LINE_SPACING + TEXT_HEIGHT;
    const float WORD_SPACING = scale_factor * WORD_SPACING_FACTOR;
    
    float x_offset = 0.0f;
    float y_offset = -TEXT_HEIGHT;
    
    DEBUG_LOG("Initial position: X=%.3f, Y=%.3f\n", x_offset, y_offset);
    DEBUG_LOG("Text height: %.3f, Line spacing: %.3f\n", TEXT_HEIGHT, LINE_SPACING);

    char word[256];
    while (fscanf(file, "%s", word) != EOF) {
        float word_width = calculate_word_width(word, scale_factor);
        update_print_position(&x_offset, &y_offset, word_width, LINE_SPACING, MAX_LINE_WIDTH);
        print_word(word, scale_factor, &x_offset, &y_offset);
        x_offset += WORD_SPACING;
        DEBUG_LOG("Added word spacing: %.3f, New X offset: %.3f\n", WORD_SPACING, x_offset);
    }

    fclose(file);
}

float get_character_width(int ascii_code, float scale_factor) {
    if (ascii_code < 0 || ascii_code >= MAX_CHARACTERS || 
        font_data[ascii_code].num_movements == 0) {
        return 0.0f;
    }
    
    int last_idx = font_data[ascii_code].num_movements - 1;
    return font_data[ascii_code].movements[last_idx].x * scale_factor;
}