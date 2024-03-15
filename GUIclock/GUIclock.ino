//<App !Start!>
// FILE: [GUIclock.ino]
// Created by GUIslice Builder version: [0.17.b26]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<App !End!>

// This is derived from my MCUFRIEND example and adapted for the GUIslice framework. 
// The main difference from the previous code is that a lot of the parameters
// are now defined in one place (many as global variables).
// MCUFRIEND_clock.ino starting my own example
// This is now a copy of the TFT_Clock example from the TFT_ILI9341 library

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIclock_GSLC.h"


// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access
//<Save_References !Start!>
//<Save_References !End!>

// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
//<Button Callback !Start!>
//<Button Callback !End!>
//<Checkbox Callback !Start!>
//<Checkbox Callback !End!>
//<Keypad Callback !Start!>
//<Keypad Callback !End!>
//<Spinner Callback !Start!>
//<Spinner Callback !End!>
//<Listbox Callback !Start!>
//<Listbox Callback !End!>
//<Draw Callback !Start!>
//<Draw Callback !End!>
//<Slider Callback !Start!>
//<Slider Callback !End!>
//<Tick Callback !Start!>
//<Tick Callback !End!>

bool m_bCircle = false;

// Define an origin for the box.
int16_t   m_nOriginX = 0;
int16_t   m_nOriginY = 0;
// Centre of the clock.
int16_t   m_nCentreX = 160;
int16_t   m_nCentreY = 130;
int16_t   m_nRadius  = 80;

// Variables from MCUFRIEND_clock.ino not renamed.
// Some values have been changed.
float sx = 0, sy = 1, mx = 1, my = 0, hx = -1, hy = 0;    // Saved H, M, S x & y multipliers
float sdeg=0, mdeg=0, hdeg=0;
uint16_t osx=m_nCentreX, osy=m_nCentreY, omx=m_nCentreX, omy=m_nCentreY, ohx=m_nCentreX, ohy=m_nCentreY;  // Saved H, M, S x & y coords
uint16_t x0=0, x1=0, y0=0, y1=0;
uint16_t sR = m_nRadius-15;
uint16_t mR = m_nRadius-20;
uint16_t hR = m_nRadius-30;
uint32_t targetTime = 0;                    // for next 1 second timeout

static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

boolean initial = 1;

// This is being called to replace what is in the box E_ELEM_BOX1
// I have found out how to do this by comparison with ex06_ard_callback.ino
// It is not possible to draw the background except within an element as it gets overwritten.
// The call to this    callback is set up in InitGUIslice_gen and called when it thinks it necessary.
// This now does the static part of the clock with many parameters adjusted to get the location and size correct.
bool CbDrawClock(void* pvGui,void* pvElemRef,gslc_teRedrawType eRedraw)
{
  int nInd;

  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  // Create shorthand variables for the origin, width and height.
  int16_t  nX = pElem->rElem.x + m_nOriginX;
  int16_t  nY = pElem->rElem.y + m_nOriginY;
  int16_t  nW = pElem->rElem.w;
  int16_t  nH = pElem->rElem.h;
  // Clock location and radius
  int16_t  nCx = m_nCentreX; // Circle centre x
  int16_t  nCy = m_nCentreY; // Circle centre y
  int16_t  nR  = m_nRadius;  // Circle centre radius
  int16_t  nRi = nR - 6;     // Radius inside of frame
  int16_t  nRii = nR - 12;   // Radius inside of the lines and dots

  // Replace whatever was in the rectangle before.
  gslc_DrawFillRect(&m_gui,(gslc_tsRect){nX,nY,nW,nH},GSLC_COL_GRAY);
  // Draw frame and background.
  gslc_DrawFillCircle(&m_gui,nCx,nCy,nR,GSLC_COL_GREEN);
  gslc_DrawFillCircle(&m_gui,nCx,nCy,nRi,GSLC_COL_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*nRi+nCx;
    y0 = sy*nRi+nCy;
    x1 = sx*nRii+nCx;
    y1 = sy*nRii+nCy;

    gslc_DrawLine(&m_gui,x0, y0, x1, y1, GSLC_COL_GREEN);
  }

    // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    x0 = sx*nRii+nCx;
    y0 = sy*nRii+nCy;
    // Draw minute markers
    gslc_DrawSetPixel(&m_gui,x0, y0, GSLC_COL_WHITE);

   // Draw main quadrant dots
    if(i==0  || i==180) gslc_DrawFillCircle(&m_gui,x0, y0, 2, GSLC_COL_WHITE);
    if(i==90 || i==270) gslc_DrawFillCircle(&m_gui,x0, y0, 2, GSLC_COL_WHITE);
  }

  gslc_DrawFillCircle(&m_gui,nCx, nCy, 2, GSLC_COL_WHITE);

  // Clear the redraw flag
  gslc_ElemSetRedraw(pGui,pElemRef,GSLC_REDRAW_NONE);

  return true;
}

// Does a tick callback for clock window
// I am not sure how much of the code needs to be here or in loop.
// It turns out for this example there is nothing needed here.
// It could be used for a redraw of the clock.
// See ex06_ard_callback for an example of use.
bool CbTickClock(void* pvGui,void* pvClock)
{
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvClock);

  return true;
}

void setup()
{
  // ------------------------------------------------
  // Initialize
  // ------------------------------------------------
  Serial.begin(115200);
  // Wait for USB Serial 
  //delay(1000);  // NOTE: Some devices require a delay after Serial.begin() before serial port can be used

  gslc_InitDebug(&DebugOut);

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUIslice_gen();


  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);
}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  if (targetTime < millis()) {
    targetTime = millis()+1000;
    ss++;              // Advance second
    if (ss==60) {
      ss=0;
      mm++;            // Advance minute
      if(mm>59) {
        mm=0;
        hh++;          // Advance hour
        if (hh>23) {
          hh=0;
        }
      }
    }

  // Pre-compute hand degrees, x & y coords for a fast screen update
    sdeg = ss*6;                  // 0-59 -> 0-354
    mdeg = mm*6+sdeg*0.01666667;  // 0-59 -> 0-360 - includes seconds
    hdeg = hh*30+mdeg*0.0833333;  // 0-11 -> 0-360 - includes minutes and seconds
    hx = cos((hdeg-90)*0.0174532925);    
    hy = sin((hdeg-90)*0.0174532925);
    mx = cos((mdeg-90)*0.0174532925);    
    my = sin((mdeg-90)*0.0174532925);
    sx = cos((sdeg-90)*0.0174532925);    
    sy = sin((sdeg-90)*0.0174532925);

    if (ss==0 || initial) {
      initial = 0;
      // Erase hour and minute hand positions every minute
      gslc_DrawLine(&m_gui,ohx, ohy, m_nCentreX, m_nCentreY, GSLC_COL_BLACK);
      ohx = hx*hR+m_nCentreX;    
      ohy = hy*hR+m_nCentreY;
      gslc_DrawLine(&m_gui,omx, omy, m_nCentreX, m_nCentreY, GSLC_COL_BLACK);
      omx = mx*mR+m_nCentreX;    
      omy = my*mR+m_nCentreY;
    }

    // Redraw new hand positions, hour and minute hands not erased here to avoid flicker
    gslc_DrawLine(&m_gui,osx, osy, m_nCentreX, m_nCentreY, GSLC_COL_BLACK);
    osx = sx*sR+m_nCentreX;    
    osy = sy*sR+m_nCentreY;
    gslc_DrawLine(&m_gui,osx, osy, m_nCentreX, m_nCentreY, GSLC_COL_RED);
    gslc_DrawLine(&m_gui,ohx, ohy, m_nCentreX, m_nCentreY, GSLC_COL_WHITE);
    gslc_DrawLine(&m_gui,omx, omy, m_nCentreX, m_nCentreY, GSLC_COL_WHITE);
    gslc_DrawLine(&m_gui,osx, osy, m_nCentreX, m_nCentreY, GSLC_COL_RED);

    gslc_DrawFillCircle(&m_gui, m_nCentreX, m_nCentreY, 3, GSLC_COL_RED);

  } 
  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
}

static uint8_t conv2d(const char* p) {
  uint8_t v = 0;
  if ('0' <= *p && *p <= '9')
    v = *p - '0';
  return 10 * v + *++p - '0';
}

