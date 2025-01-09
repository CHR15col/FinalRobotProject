# Robot Writer v6 Project
Computer Engineering and Mechatronics project for controlling a writing robot.



# System Manual:
## Key Features:
  - Text height configuration (4.0mm - 10.0mm)
  - Serial communication with robot (115200 baud rate)
  - Single-stroke font processing
  - Automated text-to-G-code conversion

## Sample Output:
  - When running, the program:

  - Initializes serial communication
  - Prompts for text height
  - Requests text file name
  - Processes text and sends commands to robot
  - Returns to origin upon completion

## File Description:
main.c
  - Primary control file containing program entry point and core control functions:

  - Program initialization
  - User interaction
  - Robot control sequence
  - Serial communication handling

- Configuration points:

  - BAUD_RATE: Serial communication speed (default: 115200)
  - MIN_HEIGHT/MAX_HEIGHT: Text height limits (4.0mm - 10.0mm)

## Process Text File:
  - The program processes the input text file (Test.txt) using the process_text_file function
  - Handles word wrapping, line spacing, and G-code generation for the robot
  - After processing and G-code generation, return the pen to origin (0,0), pen-up 

## Function Declaration:
int main():
  - Purpose: Program entry point and main control flow
  - Parameters: None
  - Returns: 0 on success, -1 on failure
  - Description: Orchestrates the entire program flow

void wake_up_robot(void)
  - Purpose: Initializes robot communication
  - Parameters: None
  - Returns: void
  - Description: Sends initial commands to prepare robot

void return_to_origin(void)
  - Purpose: Resets robot position
  - Parameters: None
  - Returns: void
  - Description: Moves robot to (0,0) with pen up

float get_text_height(void)
  - Purpose: User input handler for text height
  - Parameters: None
  - Returns: float (validated text height)
  - Description: Prompts and validates user height input

void initialize_robot(void)
  - Purpose: Sets initial robot state
  - Parameters: None
  - Returns: void
  - Description: Configures initial position, pen state and spindle

void process_text(const char *text_filename, float scale_factor)
  - Purpose: Text processing controller
  - Parameters:
    - text_filename (input): File to process
    - scale_factor (input): Scaling value for text size
  - Returns: void
  - Description: Manages text file processing

void SendCommands(char *buffer)
  - Purpose: Robot command transmission
  - Parameters:
   - buffer (input): Command string to send
  - Returns: void
  - Description: Handles command transmission and acknowledgment

## Data Structures:
Local Variables
  - text_height (float): Stores user-specified text height
  - scale_factor (float): Calculated scaling value for font
  - text_filename (char[256]): Stores input file name
  - buffer (char[100]): Command string buffer

## Debug Mode (toggle in debug.h)
When DEBUG is defined (debug.h):
  - Outputs detailed logging via DEBUG_LOG
  - Provides execution trace information
  - Shows command transmission details

## Serial Mode (toggle in serial.c)
  - Controls robot communication
  - Manages command transmission and acknowledgment
  - Handles timing and synchronization

## Configuration Notes
  - Serial port settings defined in serial.h
  - Debug output controlled by debug.h
  - Font configuration in font.h
  - Timing parameters can be adjusted in SendCommands()

This manual provides essential information for maintaining and developing the Robot Writer system. For implementation of font handling and text processing information, refer to the font.c and font.h documentation.

# Program logic flow:
Serial_Mode - ON

|-- Initialise Robot
        - Wake up robot
        - Set initial state (pen up, origin position)
    - Get User Input
        - Prompt for text height
        - Prompt for text file name
    - Load Font Data
        - Open font file
        - Validate font file format
        - Store character movements in memory
    - Process Text File
        - Open text file
        - Read word by word
           - Calculate word width
              - Update print position (handle line wrapping)
              - Print word
                    - Generate G-code for each character
                    - Scale and position character
                    - Send G-code to robot
    - Return to Origin
        - Move robot to (0,0) with pen up
    - End Program
        - Close files and terminate

