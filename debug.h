#ifndef DEBUG_H
#define DEBUG_H


//#define DEBUG_MODE  // Comment out to disable debugging

#ifdef DEBUG_MODE
    #include <stdio.h>
    #define DEBUG_LOG(format, ...) printf("[DEBUG] " format, ##__VA_ARGS__)
    #define DEBUG_PRINT_CHAR(c) printf("[DEBUG] Character '%c' (ASCII %d)\n", c, (int)c)
    #define DEBUG_PRINT_COORDS(x, y) printf("[DEBUG] Coordinates: X=%.3f, Y=%.3f\n", x, y)
    #define DEBUG_PRINT_MOVEMENT(pen) printf("[DEBUG] Pen state: %s\n", pen ? "DOWN" : "UP")
    #define DEBUG_PRINT_FILE(filename) printf("[DEBUG] Processing file: %s\n", filename)
#else
    #define DEBUG_LOG(format, ...)
    #define DEBUG_PRINT_CHAR(c)
    #define DEBUG_PRINT_COORDS(x, y)
    #define DEBUG_PRINT_MOVEMENT(pen)
    #define DEBUG_PRINT_FILE(filename)
#endif

#endif // DEBUG_H