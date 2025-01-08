/**
 * @file font_handler.h
 * @brief Header file for font handling and text processing functionality
 * 
 * This file contains declarations for structures and functions that handle
 * font loading, character processing, and text-to-GCode conversion for the
 * Robot Writer system.
 */

#ifndef FONT_HANDLER_H
#define FONT_HANDLER_H

#include "debug.h"

/**
 * @brief Maximum number of ASCII characters supported
 */
#define MAX_CHARACTERS 128

/**
 * @brief Maximum number of movements per character
 * @note Adjust if font requires more complex characters
 */
#define MAX_MOVEMENTS 26

/**
 * @brief Text formatting constants for layout control
 */
#define BASE_LINE_SPACING 5.0f      // Base spacing between lines in mm
#define WORD_SPACING_FACTOR 15.0f   // Multiplier for space between words
#define MAX_LINE_WIDTH 100.0f       // Maximum width of text line in mm

/**
 * @brief Structure to store a single movement command
 * 
 * Represents one movement in a character's drawing sequence,
 * including position and pen state.
 */
typedef struct {
    int x;      // X-coordinate of the movement
    int y;      // Y-coordinate of the movement
    int pen;    // Pen state (1 = down, 0 = up)
} Movement;

/**
 * @brief Structure to store complete character data
 * 
 * Contains all information needed to draw a single character,
 * including its ASCII code and sequence of movements.
 */
typedef struct {
    int ascii_code;                     // ASCII code of the character
    int num_movements;                  // Number of movements in the character
    Movement movements[MAX_MOVEMENTS];   // Array of movement commands
} CharacterData;

/**
 * @brief Loads font data from a specified file
 * 
 * @param filename Path to the font file
 * @return int 0 on success, -1 on failure (file not found, invalid format)
 */
int load_font_file(const char *filename);

/**
 * @brief Generates and sends G-code for a single character
 * 
 * @param ascii_code ASCII code of the character to print
 * @param scale_factor Scaling factor for character size
 * @param x_offset X-position offset for character placement
 * @param y_offset Y-position offset for character placement
 * @return int 0 on success, -1 on invalid character or movement error
 */
int print_gcode_for_character(int ascii_code, float scale_factor, float x_offset, float y_offset);

/**
 * @brief Calculates the width of a character at given scale
 * 
 * @param ascii_code ASCII code of the character
 * @param scale_factor Scaling factor for character size
 * @return float Width of the character in mm, 0.0 if invalid
 */
float get_character_width(int ascii_code, float scale_factor);

/**
 * @brief Processes a text file and converts it to G-code
 * 
 * @param filename Path to the text file to process
 * @param scale_factor Scaling factor for text size
 */
void process_text_file(const char *filename, float scale_factor);

/**
 * @brief Helper function to calculate total width of a word
 * 
 * @param word String containing the word to measure
 * @param scale_factor Scaling factor for text size
 * @return float Total width of the word in mm
 */
float calculate_word_width(const char* word, float scale_factor);

/**
 * @brief Initializes the font data structure
 * 
 * Sets up initial state of font data array before loading font file
 */
void initialize_font_data(void);

/**
 * @brief Processes movement data for a single character
 * 
 * @param file Pointer to open font file
 * @param ascii_code ASCII code of character being processed
 * @param num_movements Number of movements to process
 * @return int 0 on success, -1 on error
 */
int process_character_movements(FILE* file, int ascii_code, int num_movements);

/**
 * @brief Updates print position for text layout
 * 
 * @param x_offset Pointer to current X position
 * @param y_offset Pointer to current Y position
 * @param word_width Width of current word
 * @param line_spacing Spacing between lines
 * @param max_width Maximum line width
 */
void update_print_position(float* x_offset, float* y_offset, float word_width, 
                         float line_spacing, float max_width);

/**
 * @brief Prints a single word at the specified position
 * 
 * @param word String containing the word to print
 * @param scale_factor Scaling factor for text size
 * @param x_offset Pointer to current X position (updated after printing)
 * @param y_offset Pointer to current Y position
 */
void print_word(const char* word, float scale_factor, float* x_offset, float* y_offset);

#endif // FONT_HANDLER_H