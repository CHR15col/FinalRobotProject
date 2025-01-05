# Robot Writer v6 Project
Computer Engineering and Mechatronics project for controlling a writing robot.

# Current Status
- implement font.h into font parsing source code - font.c

 ## font.h functions:

   # Main functional declarations:
   - load_font_file() 
   - print_gcode_for_character()
   # Debug functional declarations (testing purposes):
   - debug_log()
   - print_movement()
   - print_character_data()
   - print_font_data()

# font.c outline:
  - Declare global array function - font_data[]

## Debugging
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
- Develop test system in main.c to validate parsing
- Add text file processing
- Develop G-code generation