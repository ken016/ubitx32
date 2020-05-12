//Firmware Version
//+ : This symbol identifies the firmware. 
//    It was originally called 'CEC V1.072' but it is too long to waste the LCD window.
//    I do not want to make this Firmware users's uBITX messy with my callsign.
//    Putting one alphabet in front of 'v' has a different meaning.
//    So I put + in the sense that it was improved one by one based on Original Firmware.
//    This firmware hass been gradually changed based on the original firmware created by Farhan, Jack, Jerry and others.
  
#define FIRMWARE_VERSION_INFO F("+v32-1.200")  
#define BITX

/**
 Cat Suppoort uBITX CEC Version 
 This firmware has been gradually changed based on the original firmware created by Farhan, Jack, Jerry and others.
 Most features(TX, Frequency Range, Ham Band, TX Control, CW delay, start Delay... more) have been added by KD8CEC.
 My wish is to keep the original author's Comment as long as the meaning does not change much, even if the code looks a bit long.
 Ian KD8CEC

 Original source comment            -------------------------------------------------------------
 * This source file is under General Public License version 3.
 * 
 * This version uses a built-in Si5351 library
 * Most source code are meant to be understood by the compilers and the computers. 
 * Code that has to be hackable needs to be well understood and properly documented. 
 * Donald Knuth coined the term Literate Programming to indicate code that is written be 
 * easily read and understood.
 * 
 * The Raduino is a small board that includes the Arduin Nano, a 16x2 LCD display and
 * an Si5351a frequency synthesizer. This board is manufactured by Paradigm Ecomm Pvt Ltd
 * 
 * To learn more about Arduino you may visit www.arduino.cc. 
 * 
 * The Arduino works by starts executing the code in a function called setup() and then it 
 void 
 * repeatedly keeps calling loop() forever. All the initialization code is kept in setup()
 * and code to continuously sense the tuning knob, the function button, transmit/receive,
 * etc is all in the loop() function. If you wish to study the code top down, then scroll
 * to the bottom of this file and read your way up.
 * 
 * Below are the libraries to be included for building the Raduino 
 * The EEPROM library is used to store settings like the frequency memory, caliberation data, 
 * callsign etc .
 *
 *  The main chip which generates upto three oscillators of various frequencies in the
 *  Raduino is the Si5351a. To learn more about Si5351a you can download the datasheet 
 *  from www.silabs.com although, strictly speaking it is not a requirment to understand this code. 
 *  Instead, you can look up the Si5351 library written by xxx, yyy. You can download and 
 *  install it from www.url.com to complile this file.
 *  The Wire.h library is used to talk to the Si5351 and we also declare an instance of 
 *  Si5351 object to control the clocks.
 */
#include "defines.h"              
#include "variables.h"                // include b
#include "commontexts.h"              
#include "ajaxcode.h"                  // include
#include <WiFi.h>
#include <WebServer.h>
#include <Update.h>
#include <ESP8266FtpServer.h>
#include <WiFiUdp.h>
#include <NTPClient.h>                // Local
#include "Time.h"                     // Local 
#include "TimeLib.h"                  // Localc
#include "FS.h"
#include "SPIFFS.h"
#include <HTTPClient.h>
#include <EEPROM.h>
#include <TFT_eSPI.h>     // Graphics and font library for ILI9341 driver chip


TFT_eSPI tft=TFT_eSPI();  // Invoke library, pins defined in User_Setup.h
FtpServer ftpSrv;   //set #define FTP_DEBUG in ESP8266FtpServer.h to see ftp verbose on serial
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "europe.pool.ntp.org");

#include "ubitx.h"
#include "ubitx_eemap.h"

/**
 * The uBITX is an upconnversion transceiver. The first IF is at 45 MHz.
 * The first IF frequency is not exactly at 45 Mhz but about 5 khz lower,
 * this shift is due to the loading on the 45 Mhz crystal filter by the matching
 * L-network used on it's either sides.
 * The first oscillator works between 48 Mhz and 75 MHz. The signal is subtracted
 * from the first oscillator to arriive at 45 Mhz IF. Thus, it is inverted : LSB becomes USB
 * and USB becomes LSB.
 * The second IF of 12 Mhz has a ladder crystal filter. If a second oscillator is used at 
 * 57 Mhz, the signal is subtracted FROM the oscillator, inverting a second time, and arrives 
 * at the 12 Mhz ladder filter thus doouble inversion, keeps the sidebands as they originally were.
 * If the second oscillator is at 33 Mhz, the oscilaltor is subtracated from the signal, 
 * thus keeping the signal's sidebands inverted. The USB will become LSB.
 * We use this technique to switch sidebands. This is to avoid placing the lsbCarrier close to
 * 12 MHz where its fifth harmonic beats with the arduino's 16 Mhz oscillator's fourth harmonic
 */

// the second oscillator should ideally be at 57 MHz, however, the crystal filter's center frequency 
// is shifted down a little due to the loading from the impedance matching L-networks on either sides

//For Test  //45005000
  //#define SECOND_OSC_USB (56064200l)
  //#define SECOND_OSC_LSB (33945800l) 

/*
  //For Test //4500000
  #define SECOND_OSC_USB (56059200l)
  #define SECOND_OSC_LSB (33940800l) 
*/

/*
  //For Test // V1.121  44991500(LSB), 44998500 (USB), abs : 7k
  #define SECOND_OSC_USB (56057700l)
  #define SECOND_OSC_LSB (33932300l) 
*/

  //==============================================================================================================================
  //For Test // V1.200 V1.122  45002500 (LSB), 45002000 (USB) (Change Default BFO Frequency 11056xxx, adjust bfo and ifshift ), abs: 0.5k
  //Best, Test 3 uBITX V5
  //Last Value, If more data is collected, it can be changed to a better value.
  #define SECOND_OSC_USB (56058700l)
  #define SECOND_OSC_LSB (33945800l) 

  //Not used, Just comment (Default)
  #define INIT_USB_FREQ   (11056500l)
  //-----------------------------------------------------------------------------------------------------------------------------
  

// limits the tuning and working range of the ubitx between 3 MHz and 30 MHz
#define LOWEST_FREQ  (3000000l)
#define HIGHEST_FREQ (30000000l)

//When the frequency is moved by the dial, the maximum value by KD8CEC
#define LOWEST_FREQ_DIAL  (3000l)
#define HIGHEST_FREQ_DIAL (60000000l)

int8_t meter_reading = 0; // a -1 on meter makes it invisible
unsigned long vfoA_eeprom, vfoB_eeprom; //for protect eeprom life

//for store the mode in eeprom
// byte vfoA_mode=0, vfoB_mode = 0;          //0: default, 1:not use, 2:LSB, 3:USB, 4:CW, 5:AM, 6:FM
byte vfoA_mode_eeprom, vfoB_mode_eeprom;  //for protect eeprom life

//KD8CEC
//for AutoSave and protect eeprom life
byte saveIntervalSec = 10;  //second
unsigned long saveCheckTime = 0;
unsigned long saveCheckFreq = 0;

//sideTonePitch + sideToneSub = sideTone
byte sideTonePitch=0;
byte sideToneSub = 0;

//DialLock
byte isDialLock = 0;  //000000[0]vfoB [0]vfoA 0Bit : A, 1Bit : B
byte isTxType = 0;    //000000[0 - isSplit] [0 - isTXStop]


//CW ADC Range
int cwAdcSTFrom = 0;
int cwAdcSTTo = 0;
int cwAdcDotFrom = 0;
int cwAdcDotTo = 0;
int cwAdcDashFrom = 0;
int cwAdcDashTo = 0;
int cwAdcBothFrom = 0;
int cwAdcBothTo = 0;
bool Iambic_Key = true;
#define IAMBICB 0x10 // 0 for Iambic A, 1 for Iambic B
unsigned char keyerControl = IAMBICB;

int shiftDisplayAdjustVal = 0;  //

//Variables for auto cw mode
byte isCWAutoMode = 0;          //0 : none, 1 : CW_AutoMode_Menu_Selection, 2 : CW_AutoMode Sending
byte cwAutoTextCount = 0;       //cwAutoText Count
byte beforeCWTextIndex = 255;   //when auto cw start, always beforeCWTextIndex = 255, (for first time check)
byte cwAutoDialType = 0;        //0 : CW Text Change, 1 : Frequency Tune

#define AUTO_CW_RESERVE_MAX 3
byte autoCWSendReserv[AUTO_CW_RESERVE_MAX]; //Reserve CW Auto Send
byte autoCWSendReservCount = 0;             //Reserve CW Text Cound
byte sendingCWTextIndex = 0;                //cw auto seding Text Index

WebServer server(conf.webPort);


#include "basicfunctions.h"            // include
#include "htmlFunctions.h"             // include

boolean txCAT = false;        //turned on if the transmitting due to a CAT command
boolean txTFT = false;        //turned on if the transmitting due to a TFT command
byte inTx = 0;                //it is set to 1 if in transmit mode (whatever the reason : cw, ptt or cat)
char keyDown = 0;             //in cw mode, denotes the carrier is being transmitted
byte menuOn = 0;              //set to 1 when the menu is being displayed, if a menu item sets it to zero, the menu is exited
                             
//byte advancedFreqOption1;     //255 : Bit0: use IFTune_Value, Bit1 : use Stored enabled SDR Mode, Bit2~Bit3 : dynamic sdr frequency,  bit 7: IFTune_Value Reverse for DIY uBITX
//byte attLevel = 0;            //ATT : RF Gain Control (Receive) <-- IF1 Shift, 0 : Off, ShiftValue is attLevel * 100; attLevel 150 = 15K
//byte if1TuneValue = 0;        //0 : OFF, IF1 + if1TuneValue * 100; // + - 12500;
//byte sdrModeOn = 0;           //SDR MODE ON / OFF
//unsigned long SDR_Center_Freq; //

unsigned long beforeIdle_ProcessTime = 0; //for check Idle time
byte line2DisplayStatus = 0;  //0:Clear, 1 : menu, 1: DisplayFrom Idle, 
char lcdMeter[17];

//Current ADC Value for S.Meter, and S Meter Level
int currentSMeter = 0;
byte isIFShift = 0;     //1 = ifShift, 2 extend

//Use Custom Filter
char isCustomFilter = 0;
char isCustomFilter_A7 = 0;
char CustFilters[7][2];
                              
/* Below are the basic functions that control the uBitx. Understanding the functions before 
 * you start hacking around */

//-1 : not found, 0 ~ 9 : Hamband index
byte getIndexHambanBbyFreq(unsigned long f)
{
  f = f / 1000;
  for (byte i = 0; i < conf.useHamBandCount; i++)
    if (conf.hamBandRange[i][0] <= f && f < conf.hamBandRange[i][1])
      return i;
  return -1;
}

// when Band change step = just hamband,  moveDirection : 1 = next, -1 : prior
void setNextHamBandFreq(unsigned long f, int moveDirection)
{
  unsigned long resultFreq = 0;
  byte loadMode = 0;
  byte findedIndex = getIndexHambanBbyFreq(f);
  if (findedIndex == -1) { findedIndex=3; }   //out of hamband
  else if (moveDirection==1)
    {
    if (findedIndex < conf.useHamBandCount-1)
      findedIndex++;
    else
      findedIndex=0;
    }
  else if (moveDirection==-1)
    {
    if (findedIndex > 0)
      findedIndex--;
    else
      findedIndex=conf.useHamBandCount-1;
    }
  else
    findedIndex = -1;
  if (findedIndex == -1) conf.actualBand=99; else conf.actualBand=findedIndex;
  //loadMode = (byte)(resultFreq >> 30);
  //resultFreq = resultFreq & 0x3FFFFFFF;
  loadMode = (byte)(resultFreq >> 29);
  resultFreq = resultFreq & 0x1FFFFFFF;
  
  if ((resultFreq / 1000) < conf.hamBandRange[(unsigned char)findedIndex][0] || (resultFreq / 1000) > conf.hamBandRange[(unsigned char)findedIndex][1])
    resultFreq = (unsigned long)(conf.hamBandRange[(unsigned char)findedIndex][0]) * 1000;

  byteToMode(loadMode, 1);
  conf.isUSB=(resultFreq>=10000000)?1:0;
  Serial2.print("resultfreq:"); Serial2.print(resultFreq);
  Serial2.print("  isUSB:"); Serial2.println(conf.isUSB);
  if (conf.actualBand != 99)
    setFrequency(conf.freqbyband[conf.actualBand][conf.vfoActive==VFO_A?0:1]);
  else
    setFrequency(resultFreq);
  saveconf();
}

void saveBandFreqByIndex(unsigned long f, unsigned long mode, char bandIndex) {
  if (bandIndex >= 0)
    saveconf();
}

/*KD8CEC
  When using the basic delay of the Arduino, the program freezes.
  When the delay is used, the program will generate an error because it is not communicating, 
  so Create a new delay function that can do background processing. */
unsigned long delayBeforeTime = 0;
byte delay_background(unsigned delayTime, byte fromType){ //fromType : 4 autoCWKey -> Check Paddle
  delayBeforeTime = millis();
  while (millis() - delayBeforeTime <= delayTime) {
    if (fromType == 4)
    {
      //CHECK PADDLE
      if (getPaddle() != 0) //Interrupt : Stop cw Auto mode by Paddle -> Change Auto to Manual
        return 1;
      //Check PTT while auto Sending
      autoSendPTTCheck();
      Check_Cat(3);
    }
    else
      {
      //Background Work      
      Check_Cat(fromType);
      }
  }

  return 0;
}
 

/**
 * Select the properly tx harmonic filters
 * The four harmonic filters use only three relays
 * the four LPFs cover 30-21 Mhz, 18 - 14 Mhz, 7-10 MHz and 3.5 to 5 Mhz
 * Briefly, it works like this, 
 * - When KT1 is OFF, the 'off' position routes the PA output through the 30 MHz LPF
 * - When KT1 is ON, it routes the PA output to KT2. Which is why you will see that
 *   the KT1 is on for the three other cases.
 * - When the KT1 is ON and KT2 is off, the off position of KT2 routes the PA output
 *   to 18 MHz LPF (That also works for 14 Mhz) 
 * - When KT1 is On, KT2 is On, it routes the PA output to KT3
 * - KT3, when switched on selects the 7-10 Mhz filter
 * - KT3 when switched off selects the 3.5-5 Mhz filter
 * See the circuit to understand this
 */

void setTXFilters(unsigned long freq){
  
    if (freq > 21000000L){  // the default filter is with 35 MHz cut-off
      digitalWrite(TX_LPF_A, 0);
      digitalWrite(TX_LPF_B, 0);
      digitalWrite(TX_LPF_C, 0);
    }
    else if (freq >= 14000000L){ //thrown the KT1 relay on, the 30 MHz LPF is bypassed and the 14-18 MHz LPF is allowd to go through
      digitalWrite(TX_LPF_A, 1);
      digitalWrite(TX_LPF_B, 0);
      digitalWrite(TX_LPF_C, 0);
    }
    else if (freq > 7000000L){
      digitalWrite(TX_LPF_A, 0);
      digitalWrite(TX_LPF_B, 1);
      digitalWrite(TX_LPF_C, 0);    
    }
    else {
      digitalWrite(TX_LPF_A, 0);
      digitalWrite(TX_LPF_B, 0);
      digitalWrite(TX_LPF_C, 1);    
    }
}

void setFrequency(unsigned long f){
  setTXFilters(f);
  //alternative to reduce the intermod spur
  if (conf.isUSB){
    if (conf.cwMode) { si5351bx_setfreq(2, conf.firstIF  + f + conf.sideTone);   }
    else             { si5351bx_setfreq(2, conf.firstIF  + f);    }
    si5351bx_setfreq(1, conf.firstIF + conf.usbCarrier);
    }
  else{       // LSB
    if (conf.cwMode) { si5351bx_setfreq(2, conf.firstIF  + f + conf.sideTone);  }
    else             { si5351bx_setfreq(2, conf.firstIF + f);    }
    si5351bx_setfreq(1, conf.firstIF - conf.usbCarrier);
    }
  conf.frequency=f;
  byte findedIndex = getIndexHambanBbyFreq(f);
  if (findedIndex == -1) 
    {
    conf.actualBand=99;
    }
    { 
    conf.actualBand=f;
    conf.freqbyband[findedIndex][conf.vfoActive==VFO_A?0:1]=f; 
    }   
  saveconf();
}

/* startTx is called by the PTT, cw keyer and CAT protocol to
 * put the uBitx in tx mode. It takes care of rit settings, sideband settings
 * Note: In cw mode, doesnt key the radio, only puts it in tx mode */
void startTx(byte txMode, byte isDisplayUpdate){
  //Check Hamband only TX //Not found Hamband index by now frequency
  if (conf.tuneTXType >= 100 && getIndexHambanBbyFreq(conf.ritOn ? conf.ritTxFrequency :  conf.frequency) == -1) {
    //no message
    return;
    }
  if ((isTxType & 0x01) != 0x01)
    {
    Serial2.println("TX ON");
    // digitalWrite(TX_RX, 1);
    }
  inTx = 1;
  
  if (conf.ritOn){
    //save the current as the rx frequency
    conf.ritRxFrequency = conf.frequency;
    setFrequency(conf.ritTxFrequency);
    }
  else 
    {
    if (conf.splitOn == 1) 
      {
      FrequencyToVFO(1);  //Save current Frequency and Mode to eeprom
      if (conf.vfoActive == VFO_B) 
        {
        conf.vfoActive = VFO_A;
        conf.frequency = conf.vfoA;
        byteToMode(conf.vfoA_mode, 0);
        }
      else if (conf.vfoActive == VFO_A)
        {
        conf.vfoActive = VFO_B;
        conf.frequency = conf.vfoB;
        byteToMode(conf.vfoB_mode, 0);
        }
      }

    setFrequency(conf.frequency);
    } //end of else

  SetCarrierFreq();

  if (txMode == TX_CW){
    //turn off the second local oscillator and the bfo
    si5351bx_setfreq(0, 0);
    si5351bx_setfreq(1, 0);

    //shif the first oscillator to the tx frequency directly
    //the key up and key down will toggle the carrier unbalancing
    //the exact cw frequency is the tuned frequency + sidetone

    if (conf.cwMode == 0)
      {
      if (conf.isUSB)
        si5351bx_setfreq(2, conf.frequency + conf.sideTone);
      else
        si5351bx_setfreq(2, conf.frequency - conf.sideTone); 
      }
    else if (conf.cwMode == 1) //CWL
      {
      si5351bx_setfreq(2, conf.frequency - conf.sideTone); 
      }
    else  //CWU
      {
      si5351bx_setfreq(2, conf.frequency + conf.sideTone);
      }
  }
  //reduce latency time when begin of CW mode
  if (isDisplayUpdate == 1)
    updateDisplay(true,false);
}

void stopTx(void){
  inTx = 0;
  Serial2.println("TX OFF");
  // digitalWrite(TX_RX, 0);           //turn off the tx
  SetCarrierFreq();
  if (conf.ritOn)
    setFrequency(conf.ritRxFrequency);
  else
    {
    if (conf.splitOn == 1) {
      //vfo Change
      if (conf.vfoActive == VFO_B){
        conf.vfoActive = VFO_A;
        conf.frequency = conf.vfoA;
        byteToMode(conf.vfoA_mode, 0);
        }
      else if (conf.vfoActive == VFO_A){
        conf.vfoActive = VFO_B;
        conf.frequency = conf.vfoB;
        byteToMode(conf.vfoB_mode, 0);
        }
    }
    setFrequency(conf.frequency);
    } //end of else
  updateDisplay(true,false);
}

void ritEnable(unsigned long f){
  if (conf.ritOn==0){
    conf.ritOn = 1;
    conf.ritTxFrequency = f;
    }
}

void ritDisable(){
  if (conf.ritOn==1){
    conf.ritOn = 0;
    setFrequency(conf.ritTxFrequency);
    }
}

/** Basic User Interface Routines. These check the front panel for any activity */

/**
 * The PTT is checked only if we are not already in a cw transmit session
 * If the PTT is pressed, we shift to the ritbase if the rit was on
 * flip the T/R line to T and update the display to denote transmission */

void checkPTT(){  
  //we don't check for ptt when transmitting cw
 /** Serial2.print("checkPTT");
  Serial2.print("  cwTimeout:"); Serial2.print(conf.cwTimeout);
  Serial2.print("  inTx:"); Serial2.print(inTx);
  Serial2.print("  PTT:"); Serial2.println(digitalRead(PTT));**/
  
  if (conf.cwTimeout > 0) return;
  if (digitalRead(PTT) == 0 && inTx == 0)
    {
    Serial2.println("PTT ON");
    startTx(TX_SSB, 1);  
    delay(50); //debounce the PTT
    }
  if (digitalRead(PTT) == 1 && inTx == 1)
    {
    Serial2.println("PTT OFF");
    stopTx(); 
    }
}

void checkButton(){
 byte currentBandIndex = -1;
  
 //only if the button is pressed
 int keyStatus = getBtnStatus();
 if (keyStatus == -1) return;
// delay(50);

 keyStatus = getBtnStatus();   //will be remove 3 lines
 if (keyStatus == -1) return;
 if (keyStatus == FKEY_PRESS)  //Menu Key
   {
    Serial2.println("DoMenu");
    //for touch screen
   doMenu();
   }
  //wait for the button to go up again
  // while(keyStatus == getBtnStatus()) { delay(10); Check_Cat(0);  }
  Check_Cat(0);
  //delay(50);//debounce
}

/************************************
Replace function by KD8CEC
prevent error controls applied Threshold for reduct errors,  dial Lock, dynamic Step
 *************************************/
byte threshold = 2;  //noe action for count
unsigned long lastEncInputtime = 0;
int encodedSumValue = 0;
unsigned long lastTunetime = 0; //if continous moving, skip threshold processing
byte lastMovedirection = 0;     //0 : stop, 1 : cw, 2 : ccw
#define encodeTimeOut 1000
   
void doTuningWithThresHold(){
  int s = 0;
  unsigned long prev_freq;
  if ((conf.vfoActive == VFO_A && ((isDialLock & 0x01) == 0x01)) ||
    (conf.vfoActive == VFO_B && ((isDialLock & 0x02) == 0x02)))
    return;
  s = enc_read();
  //if time is exceeded, it is recognized as an error,
  //ignore exists values, because of errors
  if (s == 0) {
    if (encodedSumValue != 0 && (millis() - encodeTimeOut) > lastEncInputtime)
      encodedSumValue = 0;
    lastMovedirection = 0;
    return;
    }
  lastEncInputtime = millis();
  //for check moving direction
  encodedSumValue += (s > 0 ? 1 : -1);
  //check threshold and operator actions (hold dial speed = continous moving, skip threshold check)
  //not use continues changing by Threshold
  //if ((lastTunetime < (millis() - skipThresholdTime)) && ((encodedSumValue *  encodedSumValue) <= (threshold * threshold)))
  if (((encodedSumValue *  encodedSumValue) <= (threshold * threshold)))
    return;
  lastTunetime = millis();
  //Valid Action without noise
  encodedSumValue = 0;
  prev_freq = conf.frequency;
//  conf.frequency += (conf.arTuneStep[conf.tuneStepIndex -1] * s);  //appield weight (s is speed) //if want need more increase size, change step size
  conf.frequency += (conf.arTuneStep[conf.tuneStepIndex] * s);  //applied weight (s is speed) //if want need more increase size, change step size
   
  //if (prev_freq < 10000000 && conf.frequency > 10000000) conf.isUSB = 1;
  //if (prev_freq > 10000000 && conf.frequency < 10000000) conf.isUSB = 0;
  if (conf.vfoActive=VFO_A) conf.frequencyA=conf.frequency; else conf.frequencyB=conf.frequency;
  setFrequency(conf.frequency);
  displayFreq();
}

/* RIT only steps back and forth by 100 hz at a time */
void doRIT(){
  int knob = enc_read();
  unsigned long old_freq = conf.frequency;
  if (conf.tuneStepIndex<6) conf.tuneStepIndex=6;  // step=100,10,1 Hz.
  if (knob < 0) conf.frequency -= (conf.arTuneStep[conf.tuneStepIndex]);  
  else if (knob > 0) conf.frequency += (conf.arTuneStep[conf.tuneStepIndex]);  
  if (old_freq != conf.frequency){
    setFrequency(conf.frequency);
    displayFreq();
    }
}

/* save Frequency and mode to eeprom for Auto Save with protected eeprom cycle, by kd8cec */
void storeFrequencyAndMode(byte saveType)
{
  //freqType : 0 Both (vfoA and vfoB), 1 : vfoA, 2 : vfoB
  if (saveType == 0 || saveType == 1) //vfoA
    {
      if (conf.vfoA != vfoA_eeprom) {
        saveconf();
        vfoA_eeprom = conf.vfoA;
        }
      
      if (conf.vfoA_mode != vfoA_mode_eeprom) {
        saveconf();
        vfoA_mode_eeprom = conf.vfoA_mode;
        }
    }
  
  if (saveType == 0 || saveType == 2) //vfoB
      {
      if (conf.vfoB != vfoB_eeprom) {
        saveconf();
        vfoB_eeprom = conf.vfoB;
        }
      
      if (conf.vfoB_mode != vfoB_mode_eeprom) {
          saveconf();
          vfoB_mode_eeprom = conf.vfoB_mode;
          }
      }
}

//calculate step size from 1 byte, compatible uBITX Manager, by KD8CEC
unsigned int byteToSteps(byte srcByte) {
    byte powerVal = (byte)(srcByte >> 6);
    unsigned int baseVal = srcByte & 0x3F;

    if (powerVal == 1)      return baseVal * 10;
    else if (powerVal == 2) return baseVal * 100;
    else if (powerVal == 3) return baseVal * 1000;
    else                    return baseVal;
}

/* The settings are read from EEPROM. The first time around, the values may not be 
 * present or out of range, in this case, some intelligent defaults are copied into the 
 * variables. */
void initSettings(){
 // readconf();
   ////////////////////////////////////////////////////
   // Iniciar aquí los valores de variables nuevas

  // hasta aquí los valores de variables nuevas 
   ////////////////////////////////////////////////////
  // showSettings();
  // Serial2.println("Reiniciando valores");
  // saveconf(); // para iniciar fichero 
   
  readconf();
  if (conf.cwKeyType == 0) Iambic_Key = false;
  else    
    {
    Iambic_Key = true;
    if (conf.cwKeyType == 1) keyerControl &= ~IAMBICB; else keyerControl |= IAMBICB;   
    }
  byte findedValidValueCount = 0;
    
  //Check Value Range and default Set for new users
  if ((3 < conf.tuneTXType && conf.tuneTXType < 100) || 103 < conf.tuneTXType || conf.useHamBandCount < 1 || findedValidValueCount < 5)
    {
    conf.tuneTXType = 2;
    //if empty band Information, auto insert default region 2 frequency range
    //This part is made temporary for people who have difficulty setting up, so can remove it when you run out of memory.
    conf.useHamBandCount = 10;
    }
  
  //CW Key ADC Range ======= adjust set value for reduce cw keying error
  //by KD8CEC
  unsigned int tmpMostBits = 0;
  tmpMostBits = EEPROM.read(CW_ADC_MOST_BIT1);
  cwAdcSTFrom = EEPROM.read(CW_ADC_ST_FROM)   | ((tmpMostBits & 0x03) << 8);
  cwAdcSTTo = EEPROM.read(CW_ADC_ST_TO)       | ((tmpMostBits & 0x0C) << 6);
  cwAdcDotFrom = EEPROM.read(CW_ADC_DOT_FROM) | ((tmpMostBits & 0x30) << 4);
  cwAdcDotTo = EEPROM.read(CW_ADC_DOT_TO)     | ((tmpMostBits & 0xC0) << 2);
  
  tmpMostBits = EEPROM.read(CW_ADC_MOST_BIT2);
  cwAdcDashFrom = EEPROM.read(CW_ADC_DASH_FROM) | ((tmpMostBits & 0x03) << 8);
  cwAdcDashTo = EEPROM.read(CW_ADC_DASH_TO)     | ((tmpMostBits & 0x0C) << 6);
  cwAdcBothFrom = EEPROM.read(CW_ADC_BOTH_FROM) | ((tmpMostBits & 0x30) << 4);
  cwAdcBothTo = EEPROM.read(CW_ADC_BOTH_TO)     | ((tmpMostBits & 0xC0) << 2);

  //Enable / Diable Check for CW Display Cofiguration Group 
  if ((conf.commonOption0 & 0x80) != 0x00)
    {
    //Adjust CW Mode Freq
    shiftDisplayAdjustVal = (conf.isShiftDisplayCWFreq & 0x3F) * 10;
    //check Minus
    if ((conf.isShiftDisplayCWFreq & 0x40) == 0x40)
      shiftDisplayAdjustVal = shiftDisplayAdjustVal * -1;
 
   //Shift Display Check (Default : 0)
    conf.isShiftDisplayCWFreq = conf.isShiftDisplayCWFreq & 0x80 == 0?1:0;
    }

   //Stored IF Shift Option
  if ((conf.commonOption0 & 0x40) != 0x00)
    {
    isIFShift = conf.ifShiftValue != 0;
    }

  //default Value (for original hardware)
  if (cwAdcSTFrom >= cwAdcSTTo)
    {
    cwAdcSTFrom = 0;
    cwAdcSTTo = 50;
    }

  if (cwAdcBothFrom >= cwAdcBothTo)
  {
    cwAdcBothFrom = 51;
    cwAdcBothTo = 300;
    }
  
  if (cwAdcDotFrom >= cwAdcDotTo)
    {
    cwAdcDotFrom = 301;
    cwAdcDotTo = 600;
    }
  if (cwAdcDashFrom >= cwAdcDashTo)
    {
    cwAdcDashFrom = 601;
    cwAdcDashTo = 800;
    }
  //end of CW Keying Variables
  
  if (conf.cwDelayTime < 1 || conf.cwDelayTime > 250)  conf.cwDelayTime = 60;

  if (conf.vfoA_mode < 2)conf.vfoA_mode = 2;
  if (conf.vfoB_mode < 2) conf.vfoB_mode = 3;

  //original code with modified by kd8cec
  if (conf.usbCarrier > 11060000l || conf.usbCarrier < 11048000l)
//    conf.usbCarrier = 11052000l;   // original
    conf.usbCarrier = 11056000l;   // EA4GZI

  if (conf.cwmCarrier > 11060000l || conf.cwmCarrier < 11048000l)
//    conf.cwmCarrier = 11052000l;    // original
    conf.cwmCarrier = 11056000l;    // EA4GZI
    
  if (conf.vfoA > 35000000l || 3500000l > conf.vfoA) {
     conf.vfoA = 7150000l;
     conf.vfoA_mode = 2; //LSB
    }
  
  if (conf.vfoB > 35000000l || 3500000l > conf.vfoB) {
    conf.vfoB = 14150000l;  
    conf.vfoB_mode = 3; //USB
    }
  //end of original code section

  //for protect eeprom life by KD8CEC
  vfoA_eeprom = conf.vfoA;
  vfoB_eeprom = conf.vfoB;
  vfoA_mode_eeprom = conf.vfoA_mode;
  vfoB_mode_eeprom = conf.vfoB_mode;

  if (conf.sideTone < 100 || 2000 < conf.sideTone) conf.sideTone = 800;
  if (conf.cwSpeed < 10 || 1000 < conf.cwSpeed) conf.cwSpeed = 100;

  if (conf.sideTone < 300 || conf.sideTone > 1000) {
    sideTonePitch = 0;
    sideToneSub = 0;;
    }
  else{
    sideTonePitch = (conf.sideTone - 300) / 50;
    sideToneSub = conf.sideTone % 50;
    }
  initButtons();
}

void initPorts(){
  // analogReference(DEFAULT);   //EA4GZI no existe en ESP32
  //??
  pinMode(ENC_A, INPUT_PULLUP);   // en ESP32 necesita pullup externo
  pinMode(ENC_B, INPUT_PULLUP);   // en ESP32 necesita pullup externo
  pinMode(FBUTTON, INPUT_PULLUP); // en ESP32 necesita pullup externo
  
  pinMode(PTT, INPUT_PULLUP);     // en ESP32 necesita pullup externo
  pinMode(ANALOG_KEYER, INPUT_PULLUP);
  pinMode(ANALOG_SMETER, INPUT); //by KD8CEC

  pinMode(CW_TONE, OUTPUT);    digitalWrite(CW_TONE, 0);
  pinMode(TX_RX,OUTPUT);       digitalWrite(TX_RX, 0);
  pinMode(TX_LPF_A, OUTPUT);   digitalWrite(TX_LPF_A, 0);
  pinMode(TX_LPF_B, OUTPUT);   digitalWrite(TX_LPF_B, 0);
  pinMode(TX_LPF_C, OUTPUT);   digitalWrite(TX_LPF_C, 0);
  pinMode(CW_KEY, OUTPUT);     digitalWrite(CW_KEY, 0);
}

void initWiFi() {
  if(conf.wifimode>2) { conf.wifimode=2; }
  if(conf.wifimode==0) { WiFi.mode(WIFI_STA); }
  else if(conf.wifimode==1) { WiFi.mode(WIFI_AP); }
  else if(conf.wifimode==2) { WiFi.mode(WIFI_AP_STA); }

  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  Serial2.print("WiFiMode:"); Serial2.println(conf.wifimode);
  Serial2.print("AP MAC:"); Serial2.println(WiFi.softAPmacAddress());
 
  if((conf.wifimode==1) || (conf.wifimode==2) || (conf.wifimode==12)) { // AP o AP+STA
    WiFi.channel(conf.canalAP);
    WiFi.softAP(conf.ssidAP, conf.passAP, conf.canalAP, false);
    Serial2.print("Canal:");Serial2.println(WiFi.channel());
 //   Serial2.print("IP:");Serial2.println(WiFi.softAPIP()();
  }

  if((conf.wifimode==0) || (conf.wifimode==2) || (conf.wifimode==12)) { // STA o AP+STA
    Serial2.print("Static IP:");Serial2.println(conf.staticIP?"YES":"NO");
    if(conf.staticIP == 1) { WiFi.config(conf.EEip, conf.EEgw, conf.EEmask, conf.EEdns, conf.EEdns2); }
    for(byte i=0; i<4; i++) {Serial2.print(conf.EEip[i]); Serial2.print(".");}
    WiFi.begin(conf.ssidSTA, conf.passSTA, true);
    byte cont = 0;
    Serial2.println();
    Serial2.print("Conectando:");  Serial2.print(conf.ssidSTA);
    Serial2.print("/");  Serial2.print(conf.passSTA);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("Conectando:",0,180);  tft.drawString(conf.ssidSTA,140,180);
    while((!WiFi.isConnected()) && (cont++ < 20))  { delay(500); Serial2.print("."); }
    tft.drawString("                          ",0,180);
    Serial2.println();
    Serial2.print("STA IP: ");  Serial2.println(WiFi.localIP());
    Serial2.print("STA MAC: ");  Serial2.println(WiFi.macAddress());
    Serial2.print("Subnet Mask: ");  Serial2.println(WiFi.subnetMask());
    Serial2.print("Gateway IP: "); Serial2.println(WiFi.gatewayIP());
    Serial2.print("DNS: ");  Serial2.println(WiFi.dnsIP());
  }
}

void initTime() {
  ///// Time
  timeClient.begin();
  if(WiFi.isConnected()) {
    timeClient.setTimeOffset(3600);
    if(timeClient.update()==1)
      {countfaulttime=0; setTime(timeClient.getEpochTime()); }
    else {
      Serial2.print("timeclient.update:");   Serial2.println(timeClient.update());
    }
  }
}

void initWebserver() { server.begin();  Serial2.println("HTTP server started"); }
void initFTP() {ftpSrv.begin("admin","admin");  }  //username, password for ftp.  set ports in ESP8266FtpServer.h  (default 21, 50009 for PASV)
void handleWebclient()  { server.handleClient(); }
void handleFTP()  { if(conf.ftpenable) { ftpSrv.handleFTP(); } }
void initSPIFSS(boolean testfiles, boolean format) {
  if(SPIFFS.begin(format)) { Serial2.println("SPIFSS OK"); } else  { Serial2.println("SPIFSS ERROR"); }
  if(testfiles) {
    File dir=SPIFFS.open("/");
    File file=dir.openNextFile();
    while(file) { Serial2.print(file.name()); Serial2.print(b); Serial2.println(file.size()); file=dir.openNextFile(); }
  }
}

void setup()
{
  TFT_Init();
  Serial2.begin(115200, SERIAL_8N2, RXD2, TXD2);   // Init TFT
  Serial2.println();
  DisplayVersionInfo(FIRMWARE_VERSION_INFO);
  Init_Cat(38400, SERIAL_8N1);    Serial2.println("Init_Cat");
  initSPIFSS(false,true);  Serial2.println("initSPIFSS");
  initSettings();  Serial2.println("initSettings");
  initPorts();     Serial2.println("initPorts");

  if (conf.userCallsignLength > 0 && ((conf.userCallsignLength & 0x80) == 0x80)) 
    {
    conf.userCallsignLength = conf.userCallsignLength & 0x7F;
    Serial2.print("Callsign:");  Serial2.println(conf.userCallsignLength);
    }
  else {
    Serial2.print("uBITX v0.20");  Serial2.println(conf.userCallsignLength);
    }
  byteToMode(conf.vfoA_mode, 0);  Serial2.println("byteToMode");
  initOscillators();  Serial2.println("initOscillators");
  conf.frequency = conf.vfoA;
  saveCheckFreq = conf.frequency;  //for auto save frequency
  setFrequency(conf.vfoA);  Serial2.print("setFrequency:"); Serial2.println(conf.vfoA);
  updateDisplay(true,false);
#ifdef ENABLE_FACTORYALIGN
  if (btnDown())    {
    factory_alignment();    Serial2.println("ENABLE_FACTORYALIGN");   }
#endif
  if (conf.autoWiFi) 
    {
    initWiFi();
    initFTP();
    initHTML();
    initTime();
    initWebserver();
    checkMyIP();
    }
  Serial2.println("END SETUP");  Serial2.println("----------------------------");
}

//Auto save Frequency and Mode with Protected eeprom life by KD8CEC
void checkAutoSaveFreqMode()
{
  //when tx or ritOn, disable auto save
  if (inTx || conf.ritOn)
    return;

  //detect change frequency
  if (saveCheckFreq != conf.frequency)
  {
    saveCheckTime = millis();
    saveCheckFreq = conf.frequency;
    }
  else if (saveCheckTime != 0)
    {
    //check time for Frequency auto save
    if (millis() - saveCheckTime > saveIntervalSec * 1000)
      {
      FrequencyToVFO(1);
      saveCheckTime = 0;  //for reduce cpu use rate
      }
    }
}

void task1() {
  tini=millis();
  countfaulttime++;   // si se hace mayor que TempDesactPrg,desactiva ejecucion programas dependientes de fecha
  displayStatus();
  mact1=millis();
  if (digitalRead(CW_TONE)==0)
    {
    digitalWrite(CW_TONE, 1);
    digitalWrite(TX_RX, 1);
    digitalWrite(TX_LPF_A, 1);
    digitalWrite(TX_LPF_B, 1);
    digitalWrite(TX_LPF_C, 1);
    digitalWrite(CW_KEY, 1);
    }
  else
    {
    digitalWrite(CW_TONE, 0);
    digitalWrite(TX_RX, 0);
    digitalWrite(TX_LPF_A, 0);
    digitalWrite(TX_LPF_B, 0);
    digitalWrite(TX_LPF_C, 0);
    digitalWrite(CW_KEY, 0);
    }
}

void loop(){ 
  handleFTP();
  handleWebclient();
  if (isCWAutoMode == 0)    //when CW AutoKey Mode, disable this process
    {
    if ((!txCAT) && (!txTFT)) { checkPTT(); }  
    checkButton(); 
    handletfttouch();
    }
/** else
   {
    controlAutoCW();     Serial2.println("controlAutoCW");
   }**/

// cwKeyer();   Serial2.println("cwKeyer");
  //tune only when not tranmsitting 
  if (tftpage==0)
    {
    if (!inTx)
      {
      if (isCWAutoMode == 0 || cwAutoDialType == 1)
        {
        if (conf.ritOn) {  doRIT();    }
        else if (!inEntN) { doTuningWithThresHold();   }
        }
      if (isCWAutoMode == 0 && beforeIdle_ProcessTime < millis() - 250) {
        idle_process();     
        checkAutoSaveFreqMode();  //move here form out scope for reduce cpu use rate
        beforeIdle_ProcessTime = millis();
        }
      } //end of check TX Status
    //we check CAT after the encoder as it might put the radio into TX
    Check_Cat(inTx?1:0); // Serial2.println("Check_Cat");
    }
  if((millis() > (mact1 + 1000))) { task1(); }                      // tareas que se hacen cada segundo
}

void showSettings()
{
  Serial2.println("-----------------------------------------");
  Serial2.print("splitOn:"); Serial2.println(conf.splitOn);
  Serial2.print("isUSB:"); Serial2.println(conf.isUSB);
  Serial2.print("ritOn:"); Serial2.println(conf.ritOn);
  Serial2.print("cwMode:"); Serial2.println(conf.cwMode);
  Serial2.print("cwTimeout:"); Serial2.println(conf.cwTimeout);
  Serial2.print("modeCalibrate:"); Serial2.println(conf.modeCalibrate);
  Serial2.print("advancedFreqOption1:"); Serial2.println(conf.advancedFreqOption1);
  Serial2.print("attLevel:"); Serial2.println(conf.advancedFreqOption1);
  Serial2.print("if1TuneValue:"); Serial2.println(conf.if1TuneValue);
  Serial2.print("sdrModeOn:"); Serial2.println(conf.sdrModeOn);
  Serial2.print("SDR_Center_Freq:"); Serial2.println(conf.SDR_Center_Freq);
  Serial2.print("calibration:"); Serial2.println(conf.calibration);
  Serial2.print("vfoActive:"); Serial2.println(conf.vfoActive);
  Serial2.print("vfoA_mode:"); Serial2.println(conf.vfoA_mode);
  Serial2.print("vfoB_mode:"); Serial2.println(conf.vfoB_mode);
  Serial2.print("cwSpeed:"); Serial2.println(conf.cwSpeed);
  Serial2.print("vfoA:"); Serial2.println(conf.vfoA);
  Serial2.print("vfoB:"); Serial2.println(conf.vfoB);
  Serial2.print("sideTone:"); Serial2.println(conf.sideTone);
  Serial2.print("usbCarrier:"); Serial2.println(conf.usbCarrier);
  Serial2.print("cwDelayTime:"); Serial2.println(conf.cwDelayTime);
  Serial2.print("delayBeforeCWStartTime:"); Serial2.println(conf.delayBeforeCWStartTime);
  Serial2.print("SI5351BX_ADDR:"); Serial2.println(conf.SI5351BX_ADDR);
  Serial2.print("cwKeyType:"); Serial2.println(conf.cwKeyType);
  Serial2.print("tuneStepIndex:"); Serial2.println(conf.tuneStepIndex);
  Serial2.print("commonOption0:"); Serial2.println(conf.commonOption0);
  Serial2.print("displayOption1:"); Serial2.println(conf.displayOption1);
  Serial2.print("displayOption2:"); Serial2.println(conf.displayOption2);
  Serial2.print("useHamBandCount:"); Serial2.println(conf.useHamBandCount);
  Serial2.print("tuneTXType:"); Serial2.println(conf.tuneTXType);
  Serial2.print("isShiftDisplayCWFreq:"); Serial2.println(conf.isShiftDisplayCWFreq);
  for (byte i=0;i<10;i++)
    {
    Serial2.print("hamBandRange:"); Serial2.println(conf.hamBandRange[i][0]);
    Serial2.print("hamBandRange:"); Serial2.println(conf.hamBandRange[i][1]);
    }
  Serial2.print("ifShiftValue:"); Serial2.println(conf.ifShiftValue);
  for (byte i=0;i<9;i++)
    {
    Serial2.print("sMeterLevels:"); Serial2.println(conf.sMeterLevels[i]);
    }
  for (byte i=0;i<9;i++)
    {
    Serial2.print("KeyValues:"); Serial2.println(conf.KeyValues[i][0]);
    Serial2.print("KeyValues:"); Serial2.println(conf.KeyValues[i][1]);
    Serial2.print("KeyValues:"); Serial2.println(conf.KeyValues[i][2]);
    }
  Serial2.print("userCallsignLength:"); Serial2.println(conf.userCallsignLength);
  Serial2.print("WsprMSGCount:"); Serial2.println(conf.WsprMSGCount);
  Serial2.print("frequency:"); Serial2.println(conf.frequency);
  Serial2.print("ritRxFrequency:"); Serial2.println(conf.ritRxFrequency);
  Serial2.print("ritTxFrequency:"); Serial2.println(conf.ritTxFrequency);
  Serial2.print("scaledSMeter:"); Serial2.println(conf.scaledSMeter);
  Serial2.print("wifimode:"); Serial2.println(conf.wifimode);
  Serial2.print("scaledSMeter:"); Serial2.println(conf.scaledSMeter);
  Serial2.print("canalAP:"); Serial2.println(conf.canalAP);
  Serial2.print("ssidSTA:"); Serial2.println(conf.ssidSTA);
  Serial2.print("passSTA:"); Serial2.println(conf.passSTA);
  Serial2.print("ssidAP:"); Serial2.println(conf.ssidAP);
  Serial2.print("passAP:"); Serial2.println(conf.passAP);
  Serial2.print("EEip:"); Serial2.println(conf.EEip);
  Serial2.print("EEgw:"); Serial2.println(conf.EEgw);
  Serial2.print("EEmask:"); Serial2.println(conf.EEmask);
  Serial2.print("EEdns:"); Serial2.println(conf.EEdns);
  Serial2.print("EEdns2:"); Serial2.println(conf.EEdns2);
  Serial2.print("staticIP:"); Serial2.println(conf.staticIP);
  Serial2.print("ftpenable:"); Serial2.println(conf.ftpenable);
  Serial2.print("webPort:"); Serial2.println(conf.webPort);
  Serial2.print("firstIF:"); Serial2.println(conf.firstIF);
  for (byte i=0;i<9;i++)
    {
    Serial2.print("arTuneStep:"); Serial2.println(conf.arTuneStep[i]);
    }
  Serial2.println("=============================================");
}
