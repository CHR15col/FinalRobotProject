# Robot Writer v6 Project
Computer Engineering and Mechatronics project for controlling a writing robot.

# Current Status
- An initial trial of a main.c to test g-code generation and communicate commands to robot

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