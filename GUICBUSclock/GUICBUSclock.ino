//<App !Start!>
// FILE: [GUICBUSclock.ino]
// Created by GUIslice Builder version: [0.17.b26]
//
// GUIslice Builder Generated File
//
// For the latest guides, updates and support view:
// https://github.com/ImpulseAdventure/GUIslice
//
//<App !End!>

///////////////////////////////////////////////////////////////////////////////////////////////
// GUICBUSclock.ino
// This is the code for Figure 5 on this page.
// https://www.merg.org.uk/merg_wiki/doku.php?id=arduino:projects:tftdisplay
// This code is TOO LARGE for an Arduino UNO.
// It can run on an Arduino MEGA 1280 or Arduino MEGA 2560.
/////////////////////////////////////////////////////////////
// This is a first example of a CBUS capable module, based on GUIclock.ino.
// It adds buttons which are programmed to send events to CBUS.
// There is no incoming event processing.
// Much more could be done with multiple screens and popups to handle incoming events.
/////////////////////////////////////////////////////////////

// ------------------------------------------------
// Headers to include
// ------------------------------------------------
#include "GUICBUSclock_GSLC.h"
// 3rd party libraries
#include <Streaming.h>
// Define for either 1280 or 2560 MEGA
#if defined(ARDUINO_AVR_MEGA2560) || defined(ARDUINO_AVR_MEGA)
#define ARDUINO_MEGA
#endif

// CBUS library header files
#include <CBUS2515.h>               // CAN controller and CBUS class
#include <CBUSswitch.h>             // pushbutton switch
#include <CBUSLED.h>                // CBUS LEDs
#include <CBUSconfig.h>             // module configuration
#include <CBUSParams.h>             // CBUS parameters
#include <cbusdefs.h>               // MERG CBUS constants

// ------------------------------------------------
// Program Globals
// ------------------------------------------------
// constants
const byte VER_MAJ = 1;             // code major version
const char VER_MIN = 'a';           // code minor version
const byte VER_BETA = 0;            // code beta sub-version
const byte MODULE_ID = 81;          // CBUS module type CANshield
const byte CAN_INT_PIN = 19; //Changed for a MEGA2560 or MEGA 1280
const byte CAN_CS_PIN = 10;

const unsigned long CAN_OSC_FREQ = 16000000UL;  // Oscillator frequency on the CAN Shield

// CBUS objects
CBUSConfig modconfig;               // configuration object
CBUS2515 CBUS(&modconfig);          // CBUS object
CBUSLED ledGrn, ledYlw;             // two LED objects
CBUSSwitch pb_switch;               // switch object

////////////DEFINE MODULE/////////////////////////////////////////////////
/// Use these values for the CBUS outputs from the display shield buttons
int button = -1;
int prevbutton = -1;

// module name, must be 7 characters, space padded.
unsigned char mname[7] = { 'T', 'F', 'T', 'G', 'U', 'I', ' ' };

// forward function declarations
void eventhandler(byte, CANFrame *);
void processSerialInput(void);
void printConfig(void);

//
/// setup CBUS - runs once at power on from setup()
//
void setupCBUS() {

  // set config layout parameters
  modconfig.EE_NVS_START = 10;
  modconfig.EE_NUM_NVS = 10;
  modconfig.EE_EVENTS_START = 50;
  modconfig.EE_MAX_EVENTS = 32;
  modconfig.EE_NUM_EVS = 1;
  modconfig.EE_BYTES_PER_EVENT = (modconfig.EE_NUM_EVS + 4);

  // initialise and load configuration
  modconfig.setEEPROMtype(EEPROM_INTERNAL);
  modconfig.begin();

  Serial << F("> mode = ") << ((modconfig.FLiM) ? "FLiM" : "SLiM") << F(", CANID = ") << modconfig.CANID;
  Serial << F(", NN = ") << modconfig.nodeNum << endl;

  // show code version and copyright notice
  printConfig();


  // set module parameters
  CBUSParams params(modconfig);
  params.setVersion(VER_MAJ, VER_MIN, VER_BETA);
  params.setModuleId(MODULE_ID);
  params.setFlags(PF_FLiM | PF_COMBI);

  // assign to CBUS
  CBUS.setParams(params.getParams());
  CBUS.setName(mname);


  // register our CBUS event handler, to receive event messages of learned events
  CBUS.setEventHandler(eventhandler);

  // set CBUS LEDs to indicate the current mode
  CBUS.indicateMode(modconfig.FLiM);

  // configure and start CAN bus and CBUS message processing
  CBUS.setNumBuffers(2, 2);      // more buffers = more memory used, fewer = less
  CBUS.setOscFreq(CAN_OSC_FREQ);   // select the crystal frequency of the CAN module
  CBUS.setPins(CAN_CS_PIN, CAN_INT_PIN);   // select pins for CAN bus CE and interrupt connections

  if (!CBUS.begin()) {
    Serial << F("> error starting CBUS") << endl;
  }
}




// Save some element references for direct access
//<Save_References !Start!>
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
        Serial.println("Button 1 pressed");
        button = 1;
        break;
      case E_ELEM_BTN3:
        Serial.println("Button 2 pressed");
        button = 2;
        break;
      case E_ELEM_BTN5:
        Serial.println("Button 3 pressed");
        button = 3;
        break;
//<Button Enums !End!>
      default:
        button = -1;
        break;
    }
    if (button > 0 && button != prevbutton) {
      Serial << F("Button ") << button << F(" changed") << endl; 
      byte opCode = OPC_ACON;
      sendEvent(opCode, button);
      prevbutton = button;
    }
  }
  return true;
}

/////////////////////////////////////////////////////////////////////////
/// Clock code from GUIclock project
/////////////////////////////////////////////////////////////////////////
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
// Variables renamed to avoid problem with Arduino DUE
uint16_t m_nx0=0, m_nx1=0, m_ny0=0, m_ny1=0;
uint16_t sR = m_nRadius-15;
uint16_t mR = m_nRadius-20;
uint16_t hR = m_nRadius-30;
uint32_t targetTime = 0;                    // for next 1 second timeout

static uint8_t conv2d(const char* p); // Forward declaration needed for IDE 1.6.x
uint8_t hh=conv2d(__TIME__), mm=conv2d(__TIME__+3), ss=conv2d(__TIME__+6);  // Get H, M, S from compile time

boolean initial = 1;

// I have found out how to do this by comparison with ex06_ard_callback.ino
// It is not possible to draw the background except within an element as it gets overwritten.
// The call to this callback is set up in InitGUIslice_gen and called when it thinks it necessary.
// This now does the static part of the clock with many parameters adjusted to get the location and size correct.
// Changed to use pGui instead of &m_gui to refer to the GUI.
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
  gslc_DrawFillRect(pGui,(gslc_tsRect){nX,nY,nW,nH},GSLC_COL_GRAY);
  // Draw frame and background.
  gslc_DrawFillCircle(pGui,nCx,nCy,nR,GSLC_COL_GREEN);
  gslc_DrawFillCircle(pGui,nCx,nCy,nRi,GSLC_COL_BLACK);

  // Draw 12 lines
  for(int i = 0; i<360; i+= 30) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    m_nx0 = sx*nRi+nCx;
    m_ny0 = sy*nRi+nCy;
    m_nx1 = sx*nRii+nCx;
    m_ny1 = sy*nRii+nCy;

    gslc_DrawLine(pGui,m_nx0, m_ny0, m_nx1, m_ny1, GSLC_COL_GREEN);
  }

    // Draw 60 dots
  for(int i = 0; i<360; i+= 6) {
    sx = cos((i-90)*0.0174532925);
    sy = sin((i-90)*0.0174532925);
    m_nx0 = sx*nRii+nCx;
    m_ny0 = sy*nRii+nCy;
    // Draw minute markers
    gslc_DrawSetPixel(pGui,m_nx0, m_ny0, GSLC_COL_WHITE);

   // Draw main quadrant dots
    if(i==0  || i==180) gslc_DrawFillCircle(pGui,m_nx0, m_ny0, 2, GSLC_COL_WHITE);
    if(i==90 || i==270) gslc_DrawFillCircle(pGui,m_nx0, m_ny0, 2, GSLC_COL_WHITE);
  }

  gslc_DrawFillCircle(pGui,nCx, nCy, 2, GSLC_COL_WHITE);

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
  Serial.begin(115200); while (!Serial) {}
  Serial.println(F(""));
  Serial.println(F("GUIslice CBUS test"));
  Serial.println( __FILE__ );

  setupCBUS();



  gslc_InitDebug(&DebugOut);

  // ------------------------------------------------
  // Create graphic elements
  // ------------------------------------------------
  InitGUIslice_gen();

  // -----------------------------------
  // Start up display on main page
  gslc_SetPageCur(&m_gui,E_PG_MAIN);

  Serial << F("> ready") << endl << endl;

}

// -----------------------------------
// Main event loop
// -----------------------------------
void loop()
{
  //
  /// do CBUS message, switch and LED processing
  //

  CBUS.process();

  //
  /// process console commands
  //

  processSerialInput();

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

//
/// user-defined event processing function
/// called from the CBUS library when a learned event is received
/// it receives the event table index and the CAN frame
//

void eventhandler(byte index, CANFrame *msg) {

  // as an example, display the opcode and the first EV of this event

  Serial << F("> event handler: index = ") << index << F(", opcode = 0x") << _HEX(msg->data[0]) << endl;
  Serial << F("> EV1 = ") << modconfig.getEventEVval(index, 1) << endl;
  return;
}
//
/// print code version config details and copyright notice
//


void printConfig(void) {

  // code version
  Serial << F("> code version = ") << VER_MAJ << VER_MIN << F(" beta ") << VER_BETA << endl;
  Serial << F("> compiled on ") << __DATE__ << F(" at ") << __TIME__ << F(", compiler ver = ") << __cplusplus << endl;
#ifdef ARDUINO_MEGA
#ifdef ARDUINO_AVR_MEGA2560
  Serial << F("> Running on an Arduino MEGA2560") << endl;
#else
  Serial << F("> Running on an Arduino MEGA1280") << endl;
#endif
#elif defined(ARDUINO_AVR_NANO)
  Serial << F("> Running on an Arduino NANO") << endl;
#elif defined(ARDUINO_AVR_UNO)
  Serial << F("> Running on an Arduino UNO") << endl;
#else
  Serial << F("> Running on something else") << endl;
#endif
  Serial << F("> CS Pin ") << CAN_CS_PIN << endl;
  Serial << F("> INT Pin ") << CAN_INT_PIN << endl; 
  // copyright
  Serial << F("> © Duncan Greenwood (MERG M5767) 2019") << endl;
  Serial << F("> © Martin Da Costa (MERG M6223) 2023") << endl;
  Serial << F("> © John Fletcher (MERG M6777) 2023") << endl;
  return;
}

// Send an event routine according to Module Switch
bool sendEvent(byte opCode, unsigned int eventNo)
{
  CANFrame msg;
  msg.id = modconfig.CANID;
  msg.len = 5;
  msg.data[0] = opCode;
  msg.data[1] = highByte(modconfig.nodeNum);
  msg.data[2] = lowByte(modconfig.nodeNum);
  msg.data[3] = highByte(eventNo);
  msg.data[4] = lowByte(eventNo);

  bool success = CBUS.sendMessage(&msg);
  if (success) {
    Serial << F("> sent CBUS message with Event Number ") << eventNo << endl;
  } else {
    Serial << F("> error sending CBUS message with ")  << eventNo << endl;
  }
  return success;
}

//
/// command interpreter for serial console input
//

void processSerialInput(void) {

  byte uev = 0;
  char msgstr[32], dstr[32];

  if (Serial.available()) {

    char c = Serial.read();

    switch (c) {

    case 'n':

      // node config
      printConfig();

      // node identity
      Serial << F("> CBUS node configuration") << endl;
      Serial << F("> mode = ") << (modconfig.FLiM ? "FLiM" : "SLiM") << F(", CANID = ") << modconfig.CANID << F(", node number = ") << modconfig.nodeNum << endl;
      Serial << endl;
      break;

    case 'e':

      // EEPROM learned event data table
      Serial << F("> stored events ") << endl;
      Serial << F("  max events = ") << modconfig.EE_MAX_EVENTS << F(" EVs per event = ") << modconfig.EE_NUM_EVS << F(" bytes per event = ") << modconfig.EE_BYTES_PER_EVENT << endl;

      for (byte j = 0; j < modconfig.EE_MAX_EVENTS; j++) {
        if (modconfig.getEvTableEntry(j) != 0) {
          ++uev;
        }
      }

      Serial << F("  stored events = ") << uev << F(", free = ") << (modconfig.EE_MAX_EVENTS - uev) << endl;
      Serial << F("  using ") << (uev * modconfig.EE_BYTES_PER_EVENT) << F(" of ") << (modconfig.EE_MAX_EVENTS * modconfig.EE_BYTES_PER_EVENT) << F(" bytes") << endl << endl;

      Serial << F("  Ev#  |  NNhi |  NNlo |  ENhi |  ENlo | ");

      for (byte j = 0; j < (modconfig.EE_NUM_EVS); j++) {
        sprintf(dstr, "EV%03d | ", j + 1);
        Serial << dstr;
      }

      Serial << F("Hash |") << endl;

      Serial << F(" --------------------------------------------------------------") << endl;

      // for each event data line
      for (byte j = 0; j < modconfig.EE_MAX_EVENTS; j++) {

        if (modconfig.getEvTableEntry(j) != 0) {
          sprintf(dstr, "  %03d  | ", j);
          Serial << dstr;

          // for each data byte of this event
          for (byte e = 0; e < (modconfig.EE_NUM_EVS + 4); e++) {
            sprintf(dstr, " 0x%02hx | ", modconfig.readEEPROM(modconfig.EE_EVENTS_START + (j * modconfig.EE_BYTES_PER_EVENT) + e));
            Serial << dstr;
          }

          sprintf(dstr, "%4d |", modconfig.getEvTableEntry(j));
          Serial << dstr << endl;
        }
      }

      Serial << endl;

      break;

    // NVs
    case 'v':

      // note NVs number from 1, not 0
      Serial << "> Node variables" << endl;
      Serial << F("   NV   Val") << endl;
      Serial << F("  --------------------") << endl;

      for (byte j = 1; j <= modconfig.EE_NUM_NVS; j++) {
        byte v = modconfig.readNV(j);
        sprintf(msgstr, " - %02d : %3hd | 0x%02hx", j, v, v);
        Serial << msgstr << endl;
      }

      Serial << endl << endl;

      break;

    // CAN bus status
    case 'c':

      CBUS.printStatus();
      break;

    case 'h':
      // event hash table
      modconfig.printEvHashTable(false);
      break;

    case 'y':
      // reset CAN bus and CBUS message processing
      CBUS.reset();
      break;

    case '*':
      // reboot
      modconfig.reboot();
      break;

    case 'm':
      // free memory
      Serial << F("> free SRAM = ") << modconfig.freeSRAM() << F(" bytes") << endl;
      break;

    case 'r':
      // renegotiate
      CBUS.renegotiate();
      break;

    case 'z':
        // Reset module, clear EEPROM
        static bool ResetRq = false;
        static unsigned long ResWaitTime;
        if (!ResetRq) {
          // start timeout timer
          Serial << F(">Reset & EEPROM wipe requested. Press 'z' again within 2 secs to confirm") << endl;
          ResWaitTime = millis();
          ResetRq = true;
        } else {
          // This is a confirmed request
          // 2 sec timeout
          if (ResetRq && ((millis() - ResWaitTime) > 2000)) {
            Serial << F(">timeout expired, reset not performed") << endl;
            ResetRq = false;
          } else {
            //Request confirmed within timeout
            Serial << F(">RESETTING AND WIPING EEPROM") << endl;
            modconfig.resetModule();
            ResetRq = false;
          }
        }
        break;


    case '\r':
    case '\n':
      Serial << endl;
      break;

    default:
      // Serial << F("> unknown command ") << c << endl;
      break;
    }
  }
}
