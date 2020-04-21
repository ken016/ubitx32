/*************************************************************************
  KD8CEC's uBITX Display Routine for TFT320240 SPI
  Uses SPI interface  
-----------------------------------------------------------------------------
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************************/
#include "ubitx.h"

//======================================================================== 
//Begin of TFT320240 Library by EA4GZI
//========================================================================

void clearTFT()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

TFT_eSPI_Button btMain[10];        // buttons main display
TFT_eSPI_Button btNav[5];          // buttons main display
TFT_eSPI_Button btSet[5];          // buttons Setting display
byte btMainact[10]={0,0,0,0,0,0,0,0,0,0};
byte btSetact[15]={1,0,0,0,0};
byte btNavact[5]={1,0,0,0,1};
char btMaintext[15][8]={"RX","TX","vfo ","Band","Band","LSB","USB","CW","RIT","SPL"};
char btSettext[5][8]={"CAL","xxx","xxx","xxx","xxx"};
char btNavtext[5][8]={"<","Ent","xxx","xxx",">"};

char softBuffLines[2][20 + 1];
char softBuffSended[2][20 + 1];

char c[30], b[30];
char softBuff[20];
char softTemp[20];

void TFT_Init()
{
  Serial2.println("TFT_Init");
  tft.init(); tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("Init...",0,0);
}

//===================================================================
//Begin of Nextion LCD Protocol
// v0~v9, va~vz : Numeric (Transceiver -> Nextion LCD)
// s0~s9  : String (Text) (Transceiver -> Nextion LCD)
// vlSendxxx, vloxxx: Reserve for Nextion (Nextion LCD -> Transceiver)
//===================================================================
#define CMD_NOW_DISP      '0' //c0
char L_nowdisp = -1;          //Sended nowdisp

#define CMD_VFO_TYPE      'v' //cv
char L_vfoActive;             //vfoActive

#define CMD_CURR_FREQ     'c' //vc
unsigned long L_vfoCurr;      //vfoA
#define CMD_CURR_MODE     'c' //cc
byte L_vfoCurr_mode;          //vfoA_mode

#define CMD_VFOA_FREQ     'a' //va
unsigned long L_vfoA;         //vfoA
#define CMD_VFOA_MODE     'a' //ca
byte L_vfoA_mode;             //vfoA_mode

#define CMD_VFOB_FREQ     'b' //vb
unsigned long L_vfoB;         //vfoB
#define CMD_VFOB_MODE     'b' //cb
byte L_vfoB_mode;             //vfoB_mode

#define CMD_IS_RIT        'r' //cr
char L_ritOn;
#define CMD_RIT_FREQ      'r' //vr
unsigned long L_ritTxFrequency; //ritTxFrequency

#define CMD_IS_TX         't' //ct
char L_inTx;

#define CMD_IS_DIALLOCK   'l' //cl
byte L_isDialLock;            //byte isDialLock

#define CMD_IS_SPLIT      's' //cs
byte  L_Split;            //isTxType
#define CMD_IS_TXSTOP     'x' //cx
byte  L_TXStop;           //isTxType

#define CMD_TUNEINDEX     'n' //cn
byte L_tuneStepIndex;     //byte tuneStepIndex

#define CMD_SMETER        'p' //cs
byte L_scaledSMeter;      //scaledSMeter

#define CMD_SIDE_TONE     't' //vt
unsigned long L_sideTone; //sideTone
#define CMD_KEY_TYPE      'k' //ck
byte L_cwKeyType = -1;          //L_cwKeyType 0: straight, 1 : iambica, 2: iambicb

#define CMD_CW_SPEED      's' //vs
unsigned int L_cwSpeed;   //cwSpeed

#define CMD_CW_DELAY      'y' //vy
byte L_cwDelayTime=-1;       //cwDelayTime

#define CMD_CW_STARTDELAY 'e' //ve
byte L_delayBeforeCWStartTime=-1;  //byte delayBeforeCWStartTime

#define CMD_ATT_LEVEL     'f' //vf
byte L_attLevel;

byte L_isIFShift;             //1 = ifShift, 2 extend
#define CMD_IS_IFSHIFT    'i' //ci

int L_ifShiftValue;
#define CMD_IFSHIFT_VALUE 'i' //vi

byte L_sdrModeOn;
#define CMD_SDR_MODE      'j' //cj

#define CMD_UBITX_INFO     'm' //cm  Complete Send uBITX Information

byte L_isShiftDisplayCWFreq;  //byte isShiftDisplayCWFreq
int L_shiftDisplayAdjustVal;        //int shiftDisplayAdjustVal

//0:CW Display Shift Confirm, 1 : IFshift save
byte L_commonOption0;         //byte commonOption0

//0:Line Toggle, 1 : Always display Callsign, 2 : scroll display, 3 : s.meter
byte L_displayOption1;            //byte displayOption1
byte L_displayOption2;            //byte displayOption2 (Reserve)

#define TS_CMD_MODE           1
#define TS_CMD_FREQ           2
#define TS_CMD_BAND           3
#define TS_CMD_VFO            4
#define TS_CMD_SPLIT          5
#define TS_CMD_RIT            6
#define TS_CMD_TXSTOP         7
#define TS_CMD_SDR            8
#define TS_CMD_LOCK           9 //Dial Lock
#define TS_CMD_ATT           10 //ATT
#define TS_CMD_IFS           11 //IFS Enabled
#define TS_CMD_IFSVALUE      12 //IFS VALUE
#define TS_CMD_STARTADC      13
#define TS_CMD_STOPADC       14
#define TS_CMD_SPECTRUMOPT   15 //Option for Spectrum
#define TS_CMD_SPECTRUM      16 //Get Spectrum Value
#define TS_CMD_TUNESTEP      17 //Get Spectrum Value
#define TS_CMD_WPM           18 //Set WPM
#define TS_CMD_KEYTYPE       19 //Set KeyType

#define TS_CMD_SWTRIG        21 //SW Action Trigger for WSPR and more
#define TS_CMD_READMEM       31 //Read EEProm
#define TS_CMD_WRITEMEM      32 //Write EEProm
#define TS_CMD_LOOPBACK0     74 //Loopback1 (Response to Loopback Channgel)
#define TS_CMD_LOOPBACK1     75 //Loopback2 (Response to Loopback Channgel)
#define TS_CMD_LOOPBACK2     76 //Loopback3 (Response to Loopback Channgel)
#define TS_CMD_LOOPBACK3     77 //Loopback4 (Response to Loopback Channgel)
#define TS_CMD_LOOPBACK4     78 //Loopback5 (Response to Loopback Channgel)
#define TS_CMD_LOOPBACK5     79 //Loopback6 (Response to Loopback Channgel)
#define TS_CMD_FACTORYRESET  85 //Factory Reset
#define TS_CMD_UBITX_REBOOT  95 //Reboot

char nowdisp = 0;

#define SWS_HEADER_INT_TYPE  'v'  //Numeric Protocol Prefex

//Control must have prefix 'v' or 's'
const byte ADCIndex[6] = {A0, A1, A2, A3, A6, A7};

void SendCommandUL(char varIndex, unsigned long sendValue)
{
  memset(softTemp, 0, 20);
  ultoa(sendValue, softTemp, DEC);
}

void SendCommandL(char varIndex, long sendValue)
{
  memset(softTemp, 0, 20);
  ltoa(sendValue, softTemp, DEC);
}

uint8_t softBuff1Num[14] = {'p', 'm', '.', 'c', '0', '.', 'v', 'a', 'l', '=', 0, 0xFF, 0xFF, 0xFF};
void SendCommand1Num(char varType, char sendValue) //0~9 : Mode, nowDisp, ActiveVFO, IsDialLock, IsTxtType, IsSplitType
{
  softBuff1Num[4] = varType;
  softBuff1Num[10] = sendValue + 0x30;

   for (int i = 0; i < 14; i++)
     {
     //EA4GZI     SWSerial_Write(softBuff1Num[i]);
     }
}

void SetSWActivePage(char newPageIndex)
{
    if (L_nowdisp != newPageIndex)
    {
      L_nowdisp = newPageIndex;
      SendCommand1Num(CMD_NOW_DISP, L_nowdisp);
    }
}
//===================================================================
//End of Nextion LCD Protocol
//===================================================================

// The generic routine to display one line on the LCD 
void printLine(unsigned char linenmbr, const char *c) {
  tft.drawString(c,0,20*linenmbr);
}

void printLineF(char linenmbr, const __FlashStringHelper *c)
{
  int i;
  char tmpBuff[21];
  PGM_P p = reinterpret_cast<PGM_P>(c);  
  for (i = 0; i < 21; i++){
    unsigned char fChar = pgm_read_byte(p++);
    tmpBuff[i] = fChar;
    if (fChar == 0)
      break;
  }
  printLine(linenmbr, tmpBuff);
}

void printLineFromEEPRom(char linenmbr, char lcdColumn, byte eepromStartIndex, byte eepromEndIndex, char offsetTtype) 
{
  int colIndex = lcdColumn;
  for (byte i = eepromStartIndex; i <= eepromEndIndex; i++)
    {
    if (++lcdColumn <= 20)
      softBuffLines[linenmbr][colIndex++] = EEPROM.read((offsetTtype == 0 ? USER_CALLSIGN_DAT : WSPR_MESSAGE1) + i);
    else
      break;
    }
}

void clearLine2()
{
  printLine(0,"");
  line2DisplayStatus = 0;
}

void printLine2ClearAndUpdate(){
  printLine(0, "");
  line2DisplayStatus = 0;  
  updateDisplay(true,false);
}
//End of Display Base Routines
//==================================================================================

//==================================================================================
//Begin of User Interface Routines
//Main Display for Nextion LCD
byte nowPageIndex = 0;

//sendType == 1 not check different 
void sendUIData(int sendType)
{
  char nowActiveVFO = conf.vfoActive == VFO_A ? 0 : 1;
  if (L_vfoActive != nowActiveVFO)
    {
    L_vfoActive = nowActiveVFO;
    SendCommand1Num(CMD_VFO_TYPE, L_vfoActive);
    }

  if (L_vfoCurr != conf.frequency)
  {
    L_vfoCurr = conf.frequency;
    SendCommandUL(CMD_CURR_FREQ, conf.frequency);
  }

  byte vfoCurr_mode = modeToByte();
  if (L_vfoCurr_mode != vfoCurr_mode)
  {
    L_vfoCurr_mode = vfoCurr_mode;
    SendCommand1Num(CMD_CURR_MODE, L_vfoCurr_mode);
  }

  //if auto cw key mode, exit
  //if (isCWAutoMode != 0 || menuOn != 0)
  if (isCWAutoMode != 0)
    return;

  //nowPageIndex = 0;
  if (menuOn==0)
    {
    if (sendType == 0)
      {
      SetSWActivePage(0);
      }
    else
      {
      SetSWActivePage(0);
      }
    }
  else
    {
    //Text Line Mode
      SetSWActivePage(1);
    }

  //VFOA
  if (L_vfoA != conf.vfoA)
    {
    L_vfoA = conf.vfoA;
    SendCommandUL(CMD_VFOA_FREQ, L_vfoA);
    }

  //#define CMD_VFOA_MODE     'a' //ca
  if (L_vfoA_mode != conf.vfoA_mode)
    {
    L_vfoA_mode = conf.vfoA_mode;
    SendCommand1Num(CMD_VFOA_MODE, L_vfoA_mode);
    }

  //VFOB
  if (L_vfoB != conf.vfoB)
    {
    L_vfoB = conf.vfoB;
    SendCommandUL(CMD_VFOB_FREQ, L_vfoB);
    }

  if (L_vfoB_mode != conf.vfoB_mode)
    {
    L_vfoB_mode = conf.vfoB_mode;
    SendCommand1Num(CMD_VFOB_MODE, L_vfoB_mode);  
    }

  if (L_isDialLock != isDialLock)
    {
    L_isDialLock = isDialLock;
    SendCommand1Num(CMD_IS_DIALLOCK, L_isDialLock);  
    }

  if (L_ritOn != conf.ritOn)
    {
    L_ritOn = conf.ritOn;
    SendCommand1Num(CMD_IS_RIT, L_ritOn);  
    }
  
  if (L_ritTxFrequency != conf.ritTxFrequency)
    {
    L_ritTxFrequency = conf.ritTxFrequency;
    SendCommandUL(CMD_RIT_FREQ, L_ritTxFrequency);  
    }

  if (L_inTx != inTx)
  {
    L_inTx = inTx;
    SendCommand1Num(CMD_IS_TX, L_inTx);  
  }

  if (L_isDialLock != isDialLock)
  {
    L_isDialLock = isDialLock;
    SendCommand1Num(CMD_IS_DIALLOCK, L_isDialLock);  
  }

  if (L_Split != conf.splitOn)
  {
    L_Split = conf.splitOn;
    SendCommand1Num(CMD_IS_SPLIT, L_Split);  
  }
  

  byte isTXStop = ((isTxType & 0x01) == 0x01);
  if (L_TXStop != isTXStop)
  {
    L_TXStop = isTXStop;
    SendCommand1Num(CMD_IS_TXSTOP, L_TXStop);
  }

  if (L_tuneStepIndex != conf.tuneStepIndex)
  {
    L_tuneStepIndex = conf.tuneStepIndex;
    SendCommand1Num(CMD_TUNEINDEX, L_tuneStepIndex);
  }

  if (L_scaledSMeter != conf.scaledSMeter)
  {
    L_scaledSMeter = conf.scaledSMeter;
    SendCommand1Num(CMD_SMETER, L_scaledSMeter);  
  }

  if (L_sideTone != conf.sideTone)
  {
    L_sideTone = conf.sideTone;
    SendCommandL(CMD_SIDE_TONE, L_sideTone);
  }

  if (L_cwKeyType != conf.cwKeyType) {
    L_cwKeyType = conf.cwKeyType;
    SendCommand1Num(CMD_KEY_TYPE, L_cwKeyType);  
  }

  if (L_cwSpeed != conf.cwSpeed)
  {
    L_cwSpeed = conf.cwSpeed;
    SendCommandL(CMD_CW_SPEED, L_cwSpeed);  
  }

  if (L_cwDelayTime != conf.cwDelayTime)
  {
    L_cwDelayTime = conf.cwDelayTime;
    SendCommandL(CMD_CW_DELAY, L_cwDelayTime);  
  }

  if (L_delayBeforeCWStartTime != conf.delayBeforeCWStartTime)
  {
    L_delayBeforeCWStartTime = conf.delayBeforeCWStartTime;
    SendCommandL(CMD_CW_STARTDELAY, L_delayBeforeCWStartTime);
  }

  if (L_attLevel != conf.attLevel)
  {
    L_attLevel = conf.attLevel;
    SendCommandL(CMD_ATT_LEVEL, L_attLevel);
  }

  if (L_isIFShift != isIFShift)
  {
    L_isIFShift = isIFShift;
    SendCommand1Num(CMD_IS_IFSHIFT, L_isIFShift);
  }

  if (L_ifShiftValue != conf.ifShiftValue)
  {
    L_ifShiftValue = conf.ifShiftValue;
    SendCommandL(CMD_IFSHIFT_VALUE, L_ifShiftValue);
  }

  if (L_sdrModeOn != conf.sdrModeOn)
  {
    L_sdrModeOn = conf.sdrModeOn;
    SendCommand1Num(CMD_SDR_MODE, L_sdrModeOn);
  }
}

void updateTime()
{
  tft.setTextSize(2);  
  tft.setTextColor(WiFi.isConnected()? TFT_GREEN:TFT_RED);
  tft.drawString(WiFi.isConnected()?"CON":"DIS",70,220);
  tft.setTextColor(WiFi.isConnected()? TFT_WHITE:TFT_BLACK);
  //              55:55:55
  tft.drawString("            ",155,220);
  tft.drawNumber(second(),220,220); tft.drawString(":",210,220); 
  tft.drawNumber(minute(),190,220); tft.drawString(":",178,220); 
  tft.drawNumber(hour(),155,220);  
}

void displayFreq()
{
  Serial2.print("FREC:"); Serial2.println(conf.frequency);
  char auxC[20]="", buff[20]="";
  tft.setTextSize(4);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("          ",80,80);
  if (conf.frequency/1000000<10)
    {
    tft.drawString(" ",56,80);
    tft.drawNumber(conf.frequency/1000000,80,80);
    }
  else
    tft.drawNumber(conf.frequency/1000000,56,80);
  tft.drawString(".",99,80);
  if ((conf.frequency % 1000000)<10)
    { tft.drawString("00000",118,80); tft.drawNumber(conf.frequency % 1000000,118+124,80);  }
  else if ((conf.frequency % 1000000)<100)
    { tft.drawString("0000",118,80); tft.drawNumber(conf.frequency % 1000000,118+100,80);  }
  else if ((conf.frequency % 1000000)<1000)
    { tft.drawString("000",118,80); tft.drawNumber(conf.frequency % 1000000,118+76,80);  }
  else if ((conf.frequency % 1000000)<10000)
    { tft.drawString("00",118,80); tft.drawNumber(conf.frequency % 1000000,118+48,80);  }
  else if ((conf.frequency % 1000000)<100000)
    { tft.drawString("0",118,80); tft.drawNumber(conf.frequency % 1000000,118+24,80);  }
  else
    tft.drawNumber(conf.frequency % 1000000,118,80);
  tft.setTextSize(2);
  tft.drawString("Hz",290,100);
  tft.drawNumber(conf.calibration,90,110);
}

void displayNav()
{
  // botones navegación
  for (byte i=0;i<5;i++) 
    if (btNavact[i]==1)
      {
      btNav[i].initButtonUL(&tft,62*i,210,60,30,2,TFT_WHITE,TFT_BLACK,btNavtext[i],2);
      btNav[i].drawButton();
      }
}

void updateDisplay(boolean alldata, boolean freqdata) {
  Serial2.print("tftpage:"); Serial2.println(tftpage);
    freqdata=true; 
    clearTFT();
    if (tftpage==0)   // Main page
      {
      if (alldata) 
        {
        // botones superiores
        for (byte i=0;i<5;i++)
          {
          btMain[i].initButtonUL(&tft,62*i,0,60,30,2,btMainact[i]==0?TFT_WHITE:TFT_YELLOW,TFT_BLACK,btMaintext[i],2);
          btMain[i].drawButton();
          }
        for (byte i=5;i<10;i++)
          {
          btMain[i].initButtonUL(&tft,62*(i-5),35,60,30,2,btMainact[i]==0?TFT_WHITE:TFT_YELLOW,TFT_BLACK,btMaintext[i],2);
          btMain[i].drawButton();
          }
        displayNav();
        displayFreq();   // frecuencia
        updateTime();    // time, status
        }
      else
        {
        displayFreq();
        }
      }
    else if (tftpage==1)    // Setup
      {
      // data
      tft.drawString("Calibration:",0,40); tft.drawNumber(conf.calibration,160,40);
      // botones inferiores
      for (byte i=0;i<5;i++) 
        if (btSetact[i]==1)
          {
          btSet[i].initButtonUL(&tft,62*i,0,60,30,2,TFT_WHITE,TFT_BLACK,btSettext[i],2);
          btSet[i].drawButton();
          }
      displayNav();
      updateTime();
      }
}

//****************************************************************
// Spectrum for Range scan and Band Scan
//****************************************************************
#define RESPONSE_SPECTRUM     0
#define RESPONSE_EEPROM       1
#define RESPONSE_EEPROM_HEX_R 72  //Nextion order (Reverse)
#define RESPONSE_EEPROM_STR   87  //String

const uint8_t ResponseHeader[11]={'p', 'm', '.', 's', 'h', '.', 't', 'x', 't', '=', '"'};
const char HexCodes[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f', };

//void sendSpectrumData(unsigned long startFreq, unsigned long incStep, int scanCount, int delayTime, int sendCount)
//sendResponseData(RESPONSE_EEPROM, 0, eepromIndex, eepromReadLength, eepromDataType, 1);
//protocol Type : 0 - Spectrum, 1 : EEProm
//startFreq   : Spectrum - Frequency, EEProm - 0
//sendOption1 : Spectrum - 1 Step Frequency, EEProm - EEProm Start Address
//scanCount   : Spectrum - 1 Set Length, EEProm - Read Length
//sendOption2 : Spectrum - Value offset (because support various S-Meter), EEProm - EEProm Response DataType (0:HEX, 1:String)
//sendCount : Spectrum - All scan set count, EEProm - always 1

void sendResponseData(int protocolType, unsigned long startFreq, unsigned int sendOption1, int readCount, int sendOption2, int sendCount)  //Spectrum and EEProm Data
{
  unsigned long beforFreq = conf.frequency;
  unsigned long k;
  uint8_t adcBytes[200];    //Maximum 200 Step
  
  //Voltage drop
  //scanResult[0] = analogRead(ANALOG_SMETER);
  //adcBytes[0] = analogRead(ANALOG_SMETER);
  //delay(10);
  int readedValue = 0;

  for (int si = 0; si < sendCount; si++)
    {
    for (int i = 0; i < 11; i++)
      {
      //EA4GZI   SWSerial_Write(ResponseHeader[i]);
      }
      
    for (k = 0; k < readCount; k ++)
      {
      if (protocolType == RESPONSE_SPECTRUM)
        {
        //Spectrum Data
        //Sampling Range
        setFrequency(startFreq + (k * sendOption1));
        //Wait time for charging
        //delay(10);

#ifdef USE_I2CSMETER 
        readedValue = GetI2CSmeterValue(I2CMETER_UNCALCS);
#else
        //ADC
        readedValue = analogRead(ANALOG_SMETER);
        readedValue -= (sendOption2 * 3); //0 ~ 765
        //Down Scale
        readedValue /= 2;
        if (readedValue < 0) { readedValue = 0; }
        else if (readedValue>255) { readedValue=255; }
#endif        
        }
      else
        {
        readedValue = EEPROM.read(((sendOption2 == RESPONSE_EEPROM_HEX_R) ? (readCount - k - 1) : k) + sendOption1);
        }

      if (protocolType == RESPONSE_EEPROM && sendOption2 == RESPONSE_EEPROM_STR) //None HEX
        {
        //EA4GZI SWSerial_Write(readedValue);
        }
      else
        {
        //EA4GZI         SWSerial_Write(HexCodes[readedValue >> 4]);
        //EA4GZI         SWSerial_Write(HexCodes[readedValue & 0xf]);
        }
      }
   
    } //end of for
}

//****************************************************************
//Receive command and processing from External device (LCD or MCU)
//****************************************************************
int spectrumSendCount = 10;   //count of full scan and Send
int spectrumOffset = 0;    //offset position
int spectrumScanCount = 100;  //Maximum 200
unsigned int spectrumIncStep = 1000;   //Increaase Step
extern uint8_t receivedCommandLength;
extern void SWSerial_Read(uint8_t * receive_cmdBuffer);
uint8_t swr_buffer[20];

char checkCount = 0;
char checkCountSMeter = 0;

//execute interval : 0.25sec
void idle_process()
{
  //S-Meter Display
  if (((conf.displayOption1 & 0x08) == 0x08 && (conf.sdrModeOn == 0)) && (++checkCountSMeter > SMeterLatency))
  {
#ifdef USE_I2CSMETER 
    readconf();
#else
    int newSMeter;
    newSMeter = analogRead(ANALOG_SMETER) / 4;
    currentSMeter = newSMeter;
    conf.scaledSMeter = 0;
    for (byte s = 8; s >= 1; s--) {
      if (currentSMeter > conf.sMeterLevels[s]) {
        conf.scaledSMeter = s;
        break;
      }
    }
#endif  
    checkCountSMeter = 0; //Reset Latency time
    } //end of S-Meter
  sendUIData(1);
}

//AutoKey LCD Display Routine
void Display_AutoKeyTextIndex(byte textIndex)
{
  byte diplayAutoCWLine = 0;
  if ((conf.displayOption1 & 0x01) == 0x01)
    diplayAutoCWLine = 1;
  softBuffLines[diplayAutoCWLine][0] = byteToChar(textIndex);
  softBuffLines[diplayAutoCWLine][1] = ':';
}

void DisplayVersionInfo(const __FlashStringHelper * fwVersionInfo)
{
  tft.drawString("Version:",0,20);
  tft.drawString(fwVersionInfo,140,20);
}

void  initButtons()
{
  btMainact[0]=1;   // RX
  btMainact[1]=0;   // TX
  btMainact[2]=(conf.vfoActive==VFO_B);   // Vfo A/B
  btMainact[3]=0;   // Band Down
  btMainact[4]=0;   // Band Up
  btMainact[5]=(conf.isUSB==0);   // LSB
  btMainact[6]=(conf.isUSB==1);   // USB
  btMainact[7]=(conf.cwMode>0);   // CW
  btMainact[8]=(conf.ritOn==1);     // RIT
  btMainact[9]=(conf.splitOn==1);   // Split
  btMainact[10]=0;  // Page Dorwn
  btMainact[11]=0;  // 
  btMainact[12]=0;  // Cal
  btMainact[13]=0;  // Set
  btMainact[14]=0;  // Page Up
}

void handletfttouch()
{
  uint16_t x, y;
  if (tft.getTouch(&x, &y))
    {
    if (tft.getRotation()==1) { y=tft.height()-y;}
    else if (tft.getRotation()==3) { x=tft.width()-x; y=tft.height()-y;}
    if (tftpage==0)
      {
      for (byte i=0; i<10;i++)
        {
         if (btMain[i].contains(x,y)) 
           {
           if (i==0)    // RX
             {
             }
           else if (i==1)    // TX
             {
             }
           else if (i==2)    // vfo
             {
             conf.vfoActive=conf.vfoActive==VFO_A?VFO_B:VFO_A;
             btMainact[2]=(conf.vfoActive==VFO_B);
             strcpy(btMaintext[2],conf.vfoActive==VFO_A?"vfoA":"vfoB");
             }
           else if (i==3)    // band down
             {
              setNextHamBandFreq(conf.frequency, -1);  //Prior Band
             }
           else if (i==4)    // band up
             {
              setNextHamBandFreq(conf.frequency, 1);  //Next Band
             }
           else if (i==5)    // LSB
             {
             conf.isUSB=0; btMainact[5]=1; btMainact[6]=0;
             }
           else if (i==6)    // USB
             {
             conf.isUSB=1; btMainact[5]=0; btMainact[6]=1;
             }
           else if (i==7)    // CW
             {
             if (conf.cwMode==0) conf.cwMode=1; else conf.cwMode=0;  btMainact[7]=(conf.cwMode>0);
             }
           else if (i==8)    // RIT
             {
             if (conf.ritOn==1) ritDisable(); else ritEnable(conf.frequency);
             btMainact[8]=(conf.ritOn==1);
             Serial2.print("ritOn:"); Serial2.println(conf.ritOn);
             }
           else if (i==9)    // SPL
             {
             }
           updateDisplay(true,false);
           }
        }
      }
    else if (tftpage==1)
      {
      for (byte i=0; i<5;i++)
        {
         if (btSet[i].contains(x,y)) 
           {
           if (i==0)    // CAL
             {
             setupFreq();
             }
           else if (i==1)    // 
             {
             }
           else if (i==2)    // 
             {
             }
           else if (i==3)    // band down
             {
             }
           else if (i==4)    // band up
             {
             }
           updateDisplay(true,false);
           }
        }
      }
    for (byte i=0; i<5;i++)
      {
       if (btNav[i].contains(x,y)) 
         {
         if (i==0)  { tftpage=tftpage>0?tftpage-1:1;    }
         else if (i==1) {     }
         else if (i==2) {     }
         else if (i==3) {     }
         else if (i==4) { tftpage=tftpage<1?tftpage+1:0; }
         updateDisplay(true,false);
         }
      }
    } 
}
