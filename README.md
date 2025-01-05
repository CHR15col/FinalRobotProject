# Robot Writer v6 Project
Computer Engineering and Mechatronics project for controlling a writing robot.

## Current Status
- set up of font parsing header file - font.h

 ## Requirements:

   # Structural:
 - Character data (ASCII decimal number and number of movements)
 - Movement data (x and y coordinate and pen up/down state)
 - Array to store font data

   # Main functional declarations:
 - load_font_file() 
 - print_gcode_for_character()
   # Debug functional declarations (testing purposes):
 - debug_log()
 - print_movement()
 - print_character_data()
 - print_font_data()


## Next Steps
- Implement font file parsing
- Add text file processing
- Develop G-code generation