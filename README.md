# Robot Writer v6 Project
Computer Engineering and Mechatronics project for controlling a writing robot.

# Current Status
- added two function prototypes to font.h:
  - get_character_width():
    - Will allow identification of max positive x coord using scaling logic 
  - process_text_file() 
    - This function should cover the majority of the plan
    - Open text file
    - Read file by word 
    - calculate word width by using get_character_width()
    - check if new line is needed -> if yes, apply new line logic
    - print each character in the word applying offset logic by using print_gcode_for_character()
    - add space between words


## Plan:
  1. Modify the Program to Read Text from a File
     - Prompt the user to input the name of the text file.
     - Open the file and read its contents.
     - Process the text word by word.
  2. Implement Word Wrapping
     - Ensure that words fit within the 100mm width constraint.
     - If a word exceeds the remaining space on the current line, move it to the next line.
  3. Generate G-Code for Each Word
     - For each character in the word:
     - Retrieve its font data from memory.
     - Scale the movements based on the user-defined height.
     - Apply the X-offset for each character.
     - For each word:
       - Apply the Y-offset if the word is moved to a new line.
  4. Handle Line Breaks
     - Handle LF (ASCII 10) and CR (ASCII 13) characters in the text file to move to the next line.
  5. Finalise the Program

# main.c outline:
  - Define max/min height
  - Declare SendCommands(), get_text_height() and initialize_robot()

## main():
  1. Initialize serial communication
  2. Wake up robot
  3. Load font file
  4. Get text height from user
  5. Initialize robot
  6. Print a character test ('H')
  7. Return to origin and pen up
  8. Close com port
  9. End

## Functions:
  - Define get_text_height() - Gets text height from user with validation
  - Define initialize_robot() - Follows the skeleton file, sends required startup commands
  - Define SendCommands() - Sends commands to the robot


# font.c outline:
  - Declare global array function - font_data[]

## Debugging functions:
  - Define debug_log function to print "[DEBUG] comment":
     - The use of a log will allow one function to be used with different args and formats
     - As the function is used, progressive comments can be printed to display debug info
  - Define debug_print_movement() - Print movement data for debugging
  - Define debug_print_character_data() - Print character data for debugging
  - Define debug_print_font_data() - Print all font data for debugging

## Font Parsing (including debugging):
  1. Load font data from file
     2. Initiallise all characters with 0 movements
     3. Read file by line
        4. Check for header line (999)
           5. Read movement lines for character
  6. Close file

## Generate G-code for character (including debugging):
  1. define print_gcode_for_character() 
     2. Identify character ASCII number
     3. Identify scale factor and offsets
        4. Change pen state if needed
        5. Move to position
  6. End
  
  
## Next Steps
- Add text file processing