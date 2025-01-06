# Robot Writer v6 Project
Computer Engineering and Mechatronics project for controlling a writing robot.

# Current Status
  - main.c logic implemented using functions in font.c
  - debug.h updated to carryout approprate checks throughout program


# main.c outline:
## Initialisation:
  - Initialise any required variables and settings
  - Include debug.h

## Load Font File:
  - Attempts to load the font file (SingleStrokeFont.txt) using the load_font_file function
  - If the font file cannot be loaded, the program exits with an error message

## User Input for Text Height:
  - The user is prompted to input the desired text height (in mm)
  - The input is validated to ensure it falls within the allowed range (4mm to 10mm)
  - The scale factor is calculated based on the ratio of the input height to the default font height (18 units)

## Process Text File:
  - The program processes the input text file (Test.txt) using the process_text_file function
  - Handles word wrapping, line spacing, and G-code generation for the robot
  - After processing and G-code generation, return the pen to origin (0,0), pen-up 

# main.c flowchart:

A[Start] --> B[Load font file]
    B --> C{Font file loaded?}
    C -- No --> D[Exit with error]
    C -- Yes --> E[Prompt user for text height]
    E --> F{Valid height?}
    F -- No --> G[Exit with error]
    F -- Yes --> H[Calculate scale factor]
    H --> I[Process text file]
    I --> J[Return pen to origin]
    J --> K[Print completion message]
    K --> L[Exit]



# font.c outline:
  - Declare global array function - font_data[]

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

## Calculate scaled character width
  1. Check if ASCII code is within valid range 
    2. Return 0 if invalid
  3. Iterate through all movements for the character
    4. Track maximum x-coordinate reached
    5. Scale the x-coordinates using scale_factor
      6. Return the maximum scaled x-coordinate

## text file processing
  1. Open text file
  2. Set up positioning variables and constants
  3. Calculate line spacing based on text height
  4. Read word by word from file
    5. Calculate total width of current word
    6. Check if word fits on current line
      7. Handle line wrapping if needed
  8. Process each character in the word
    9. Generate G-code for each character
    10. Update x offset for character spacing
  11. Add word spacing after each word

  
## Next Steps
streamline code where possible