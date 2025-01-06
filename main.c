#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "rs232.h"
#include "serial.h"
#include "font.h"
#include "debug.h"

#define MIN_HEIGHT 4.0f
#define MAX_HEIGHT 10.0f

void SendCommands(char *buffer);
float get_text_height(void);
void initialize_robot(void);

int main() {
    float text_height, scale_factor;
    char buffer[100];
    char text_filename[256];

    DEBUG_LOG("Starting Robot Writer program\n");

    // Initialize serial communication
    if (CanRS232PortBeOpened() == -1) {
        DEBUG_LOG("Error: Unable to open the COM port\n");
        printf("\nUnable to open the COM port (specified in serial.h)\n");
        return -1;
    }

    // Wake up the robot
    DEBUG_LOG("Waking up robot\n");
    printf("\nAbout to wake up the robot\n");
    sprintf(buffer, "\n");
    PrintBuffer(buffer);
    Sleep(100);
    WaitForDollar();
    printf("\nThe robot is now ready to draw\n");

    // Load font file
    if (load_font_file("SingleStrokeFont.txt") == -1) {
        DEBUG_LOG("Error: Failed to load font file\n");
        printf("Failed to load font file\n");
        CloseRS232Port();
        return -1;
    }

    // Get text height from user
    text_height = get_text_height();
    scale_factor = text_height / 18.0f;
    DEBUG_LOG("Using scale factor: %.3f\n", scale_factor);

    // Initialize robot
    initialize_robot();

    // Get text file name from user
    printf("Enter the name of the text file to process: ");
    scanf("%s", text_filename);

    // Process text file
    process_text_file(text_filename, scale_factor);

    // Return to origin and pen up before finishing
    DEBUG_LOG("Returning to origin\n");
    sprintf(buffer, "S0\n");
    SendCommands(buffer);
    sprintf(buffer, "G0 X0 Y0\n");
    SendCommands(buffer);

    // Close the COM port
    CloseRS232Port();
    DEBUG_LOG("COM port closed\n");
    printf("Program completed successfully\n");
    return 0;
}

float get_text_height(void) {
    float height;
    do {
        printf("Enter text height (%.1f-%.1f mm): ", MIN_HEIGHT, MAX_HEIGHT);
        scanf("%f", &height);
        if (height < MIN_HEIGHT || height > MAX_HEIGHT) {
            printf("Invalid height. Please enter a value between %.1f and %.1f mm\n", 
                   MIN_HEIGHT, MAX_HEIGHT);
        }
    } while (height < MIN_HEIGHT || height > MAX_HEIGHT);

    DEBUG_LOG("Text height set to: %.2f mm\n", height);
    return height;
}

void initialize_robot(void) {
    char buffer[100];
    DEBUG_LOG("Initializing robot\n");
    
    sprintf(buffer, "G1 X0 Y0 F1000\n");
    SendCommands(buffer);
    sprintf(buffer, "M3\n");
    SendCommands(buffer);
    sprintf(buffer, "S0\n");
    SendCommands(buffer);
}

void SendCommands(char *buffer) {
    DEBUG_LOG("Sending command: %s", buffer);
    PrintBuffer(buffer);
    WaitForReply();
    Sleep(100);
}