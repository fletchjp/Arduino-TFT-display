# Arduino-TFT-display
 Codes for Arduino with TFT display including CBUS codes.
 
 These are codes for the examples on this page in the MERG Knowledgebase

 https://www.merg.org.uk/merg_wiki/doku.php?id=arduino:projects:tftdisplay

 There is a description there of the hardware and software used.
 
 This covers both the display and the touchscreen capability.
 
## MCUFRIEND_clock

 This is the code for Figure 1 using the MCUFRIEND_kbv library.
  
## GUIclock
 
This is the code for Figure 2 using GUIslice Builder to generate the display.

This code will not run on an Arduino UNO and requires a MEGA 1280 or MEGA 2560.

## GUIthird

This is the code for Figures 3 and 4 using GUIslice Builder to generate the display.

This code will not run on an Arduino UNO and requires a MEGA 1280 or MEGA 2560.

## GUICBUSclock

This is the code for Figure 5 using GUIslice Builder to generate the display.

This also has code for CBUS.

This code will not run on an Arduino UNO and requires a MEGA 1280 or MEGA 2560.

# Library Configuration

These codes need to run with a number of different libraries to organise both the display and the touchscreen capability.

There is a problem which is that while at compile time the code can be adapted for the processor e.g. UNO or MEGA, the same cannot be done for the display.

The type of display can be detected at run time.

This means that the library codes have to be configured for the particular display being used. For some libraries there are a set of configuration files. The library code has to be edited to chose the correct one. That has to be changed if a different display is to be used.

It is unusual for a user to have to edit library files and this is normally discouraged as the edits will be lost if the library is updated.

More details to be added.

