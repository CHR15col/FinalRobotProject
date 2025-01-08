#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include "rs232.h"
#include "serial.h"
#include "font.h"
#include "debug.h"

// Constants
#define BAUD_RATE 115200
#define MIN_HEIGHT 4.0f
#define MAX_HEIGHT 10.0f

// Function prototypes
void SendCommands (char *buffer );
void wake_up_robot(void);
void return_to_origin(void);
float get_text_height(void);
void initialize_robot(void);
void process_text(const char *text_filename, float scale_factor);

int main() {

    float text_height, scale_factor;
    char text_filename[256];

    DEBUG_LOG("Starting Robot Writer program\n");

    // Initialize serial communication
    if (CanRS232PortBeOpened() == -1) {
        DEBUG_LOG("Error: Unable to open the COM port\n");
        printf("\nUnable to open the COM port (specified in serial.h)\n");
        exit (0);
    }

    // Wake up the robot
    wake_up_robot();

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

    // Process the text file
    process_text(text_filename, scale_factor);

    // Return to origin and pen up before finishing
    return_to_origin();

    // Close the COM port
    CloseRS232Port();
    DEBUG_LOG("COM port closed\n");
    printf("Program completed successfully\n");
    return 0;
}

/**
 * Wakes up the robot by sending an initial command.
 */
void wake_up_robot(void) {
    char buffer[100];
    DEBUG_LOG("Waking up robot\n");
    printf("\nAbout to wake up the robot\n");

    sprintf(buffer, "\n");
    PrintBuffer(&buffer[0]);
    Sleep(100);
    WaitForDollar();

    printf("\nThe robot is now ready to draw\n");
}

/**
 * Returns the robot to the origin (0, 0) and lifts the pen.
 */
void return_to_origin(void) {
    char buffer[100];
    DEBUG_LOG("Returning to origin\n");

    sprintf(buffer, "S0\n"); // Pen up
    SendCommands(buffer);

    sprintf(buffer, "G0 X0 Y0\n"); // Move to origin
    SendCommands(buffer);
}

/**
 * Prompts the user to enter a valid text height and returns it.
 * @return The text height entered by the user.
 */
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

/**
 * Initializes the robot by setting the pen to the starting position.
 */
void initialize_robot(void) {
    char buffer[100];
    DEBUG_LOG("Initializing robot\n");

    sprintf(buffer, "G1 X0 Y0 F1000\n"); // Move to origin at a safe speed
    SendCommands(buffer);
    sprintf(buffer, "M3\n"); // Enable spindle (pen down)
    SendCommands(buffer);
    sprintf(buffer, "S0\n"); // Pen up
    SendCommands(buffer);
}

/**
 * Processes the text file and sends G-code commands to the robot.
 * @param text_filename The name of the text file to process.
 * @param scale_factor The scale factor for the text size.
 */
void process_text(const char *text_filename, float scale_factor) {
    DEBUG_LOG("Processing text file: %s\n", text_filename);
    process_text_file(text_filename, scale_factor);
}

void SendCommands (char *buffer )
{
    PrintBuffer (&buffer[0]);
    WaitForReply();
    //Sleep(100); // Can omit this when using the writing robot but has minimal effect
    // getch(); // Omit this once basic testing with emulator has taken place
}