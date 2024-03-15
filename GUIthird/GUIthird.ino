//<App !Start!>
// FILE: [GUIthird.ino]
// Created by GUIslice Builder version: [0.17.b26]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<App !End!>

////////////////////////////////////////////////////////////////////////////////////////////////////////
// Element References
// When building a GUI it is possible to specify a name for an element reference for each object.
// The builder mostly does not define them apart from for certain types e.g. the toggle.
// They are needed when the element is to be referenced in the run time code.
// This is NOT clearly explained anywhere I have found in the documentation.
// I needed it here because I want to use the toggle to turn on and off two LEDs (small boxes).
// In my first attempt I built the references myself and the builder deleted them on a rebuild.
// Now I have found out how to get the builder to generate them.
// It is necessary to choose a name which can be made meaningfull to the coder.
// That is in this code.
//////////////////////////////////////////////////////////////////////////////////////////////////////////

// This example turns on and off a red and green "LED" made from a small box.

// This now does the next thing which is   to get one LED to change colour!

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUIthird_GSLC.h"

// ------------------------------------------------
// Program Globals
// ------------------------------------------------

// Save some element references for direct access - npw generated.
//<Save_References !Start!>
gslc_tsElemRef* m_pElemLED1       = NULL;
gslc_tsElemRef* m_pElemLED2       = NULL;
gslc_tsElemRef* m_pElemLED3       = NULL;
gslc_tsElemRef* m_pElemToggle1    = NULL;
//<Save_References !End!>


// Define debug message function
static int16_t DebugOut(char ch) { if (ch == (char)'\n') Serial.println(""); else Serial.write(ch); return 0; }

// ------------------------------------------------
// Callback Methods
// ------------------------------------------------
// Common Button callback
bool CbBtnCommon(void* pvGui,void *pvElemRef,gslc_teTouch eTouch,int16_t nX,int16_t nY)
{
  // Typecast the parameters to match the GUI and element types
  gslc_tsGui*     pGui     = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef = (gslc_tsElemRef*)(pvElemRef);
  gslc_tsElem*    pElem    = gslc_GetElemFromRef(pGui,pElemRef);

  if ( eTouch == GSLC_TOUCH_UP_IN ) {
    // From the element's ID we can determine which button was pressed.
    switch (pElem->nId) {
//<Button Enums !Start!>
      case E_ELEM_BTN1:
        // Output a message when the button is pressed copied in  
        Serial.println("Quit button pressed");
        break;
      case E_ELEM_BTN2:
        // Output a message when the button is pressed copied in
        Serial.println("Don't quit button pressed");
        break;
      case E_ELEM_TOGGLE1:
        // TODO Add code for Toggle button ON/OFF state
        // This code is called when the toggle is switched.
        // Actions are taken depending on the state of the toggle.
        // The LEDs can be accessed using the saved element references.
        // These are generated using the names given when in the Builder
        // In this example LED1 and LED2 turn on and off and LED3 changes colour.
        if (gslc_ElemXTogglebtnGetState(&m_gui, m_pElemToggle1)) {
          Serial.println("Toggle is on");
          // Turn Red LED off and Green LED on
          gslc_ElemSetVisible(&m_gui,m_pElemLED1,false);
          gslc_ElemSetVisible(&m_gui,m_pElemLED2,true);
          gslc_ElemSetCol(&m_gui,m_pElemLED3,GSLC_COL_GRAY,GSLC_COL_GREEN,GSLC_COL_RED);
        } else { 
          // Turn Red LED on and Green LED off
          Serial.println("Toggle is off");
          gslc_ElemSetVisible(&m_gui,m_pElemLED1,true);
          gslc_ElemSetVisible(&m_gui,m_pElemLED2,false);
          gslc_ElemSetCol(&m_gui,m_pElemLED3,GSLC_COL_GRAY,GSLC_COL_RED,GSLC_COL_GREEN);
        }
        break;
      case E_ELEM_LED1:
        break;
      case E_ELEM_LED2:
        break;
      case E_ELEM_BOX3:
        break;
//<Button Enums !End!>
      default:
        break;
    }
  }
  return true;
}
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

bool CbTickScanner(void* pvGui,void* pvScope)
{
  gslc_tsGui*     pGui      = (gslc_tsGui*)(pvGui);
  gslc_tsElemRef* pElemRef  = (gslc_tsElemRef*)(pvScope);
  gslc_tsElem*    pElem     = gslc_GetElemFromRef(pGui,pElemRef);

  //TODO add your custom code here 

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

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{

  // ------------------------------------------------
  // Update GUI Elements
  // ------------------------------------------------
  
  //TODO - Add update code for any text, gauges, or sliders


  // ------------------------------------------------
  // Periodically call GUIslice update function
  // ------------------------------------------------
  gslc_Update(&m_gui);
    
}

