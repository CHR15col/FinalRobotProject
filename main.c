#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "serial.h"
#include "font.h"

#define MIN_HEIGHT 4.0
#define MAX_HEIGHT 10.0

void SendCommands(char *buffer);
float get_text_height(void);
void initialize_robot(void);

int main() {
    float text_height, scale_factor;
    char buffer[100];

    // Initialize serial communication
    if (CanRS232PortBeOpened() == -1) {
        printf("\nUnable to open the COM port (specified in serial.h)\n");
        return -1;
    }

    // Wake up the robot
    printf("\nAbout to wake up the robot\n");
    sprintf(buffer, "\n");
    PrintBuffer(buffer);
    Sleep(100);
    WaitForDollar();
    printf("\nThe robot is now ready to draw\n");

    // Load font file
    if (load_font_file("SingleStrokeFont.txt") == -1) {
        printf("Failed to load font file\n");
        CloseRS232Port();
        return -1;
    }

    // Get text height from user
    text_height = get_text_height();
    scale_factor = text_height / 18.0; // Font height is 18 units in file

    // Initialize robot
    initialize_robot();

    // Test print a character (e.g., 'H')
    print_gcode_for_character('H', scale_factor, 0.0, 0.0);

    // Return to origin and pen up before finishing
    sprintf(buffer, "S0\n");
    SendCommands(buffer);
    sprintf(buffer, "G0 X0 Y0\n");
    SendCommands(buffer);

    // Close the COM port
    CloseRS232Port();
    printf("Com port now closed\n");

    return 0;
}

// Get text height from user with validation
float get_text_height(void) {
    float height;
    do {
        printf("Enter text height (%.1f-%.1f mm): ", MIN_HEIGHT, MAX_HEIGHT);
        scanf("%f", &height);
        if (height < MIN_HEIGHT || height > MAX_HEIGHT) {
            printf("Invalid height. Please enter a value between %.1f and %.1f mm\n", MIN_HEIGHT, MAX_HEIGHT);
        }
    } while (height < MIN_HEIGHT || height > MAX_HEIGHT);
    return height;
}

// Initialize robot with required startup commands
void initialize_robot(void) {
    char buffer[100];
    
    sprintf(buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf(buffer, "M3\n");
    SendCommands(buffer);
    sprintf(buffer, "S0\n");
    SendCommands(buffer);
}

// Send commands to the robot
void SendCommands(char *buffer) {
    PrintBuffer(buffer);
    WaitForReply();
    Sleep(100);
}