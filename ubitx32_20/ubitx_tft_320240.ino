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

TFT_eSPI_Button btVFO[2];          // buttons vfo
TFT_eSPI_Button btMEM[3];          // buttons mem
TFT_eSPI_Button btFreq[9];         // buttons frequency display
TFT_eSPI_Button btFreqTx[9];       // buttons frequency display TX at ritOn & SplitOn
TFT_eSPI_Button btMain[10];        // buttons main display
TFT_eSPI_Button btNav[5];          // buttons Nav display
TFT_eSPI_Button btCal[5];          // buttons Calibration display
TFT_eSPI_Button btCW[5];           // buttons CW setting
TFT_eSPI_Button btSet[5];          // buttons Setting 
TFT_eSPI_Button btSetRad[5];       // buttons Setting radio
TFT_eSPI_Button btMemMan[5];       // buttons mem manager
TFT_eSPI_Button btNet[5];          // buttons Setting Net display
TFT_eSPI_Button btFlot[5];         // buttons flotantes
TFT_eSPI_Button btSta[5];          // buttons Status
TFT_eSPI_Button btYN[3];           // buttons OK / Cancel / Backspace
TFT_eSPI_Button btKey[50];         // buttons Key 0..9, A..Z & symbols
TFT_eSPI_Button btSel[4];          // botones barra estado: "< Cancel OK >"
TFT_eSPI_Button btMemN[7];         // botones lista memorias visibles, mempos
TFT_eSPI_Button btMemF[7];         // botones lista memorias visibles, frequency

byte btMainact[10]={0,0,0,0,0,0,0,0,0,0};
uint16_t btMaincol[10]={TFT_RED,TFT_YELLOW,TFT_YELLOW,TFT_YELLOW,TFT_YELLOW,TFT_YELLOW,TFT_CYAN,TFT_ORANGE,TFT_YELLOW,TFT_YELLOW};
int16_t btMainposx[10]={ 0,64,260,260, 64,   0,128,192,128,192}; // position mains buttons
int16_t btMainposy[10]={70,70, 35,  0,105, 105, 70, 70,105,105}; // position mains buttons

byte btCWact[15]={1,1,1,1,1};
byte btCalact[15]={1,1,1,1,1};
byte btSetact[15]={1,1,1,1,1};
byte btSetRadact[15]={1,1,1,0,0};
byte btNetact[15]={1,1,1,1,1};
byte btNavact[5]={1,1,1,0,0};
byte btFlotact[5]={1,1,1,1,1};
byte btYNact[3]={1,1,1};
char btMaintext[15][8]={"RX","V/M","Band-","Band+","LSB","CW","RIT","SPL","IFS","ATT"};
char btCaltext[5][15]={"Calibration","BFO","SI5351 ADDR","TFT calib.","Del. Memories"};
char btCWtext[5][15]={"Key type","WPM","Sidetone","Delay Time","Start Time"};
char btSettext[5][15]={"Language","CallSign","Latitude","Longitude","xxx"};
char btSetRadtext[5][15]={"Scan mode","TX range","SSB auto","xxx","xxx"};
char btMemMantext[5][15]={"xxx","xxx","xxx","xxx","xxx"};
char btNettext[5][15]={"Auto Connect","Scan SSID","Password","WiFi Mode","Static IP"};
char btNavtext[5][8]={"<","H",">","xx","xx"};
char btFlottext[5][8]={"Ent","Scan-","Scan+","Lock",">VFO"};
char btYNtext[3][8]={"OK","Cancel","<--"};
char btKeytext[50][2]={"0","1","2","3","4","5","6","7","8","9",
                       "A","B","C","D","E","F","G","H","I","J",
                       "K","L","M","N","Ñ","O","P","Q","R","S",
                       "T","U","V","W","X","Y","Z","x","x","x",
                       "-","=","_",".",",",";","/","(",")","x"};
char keytypetext[3][10]={"Straigth","Iambic A","Iambic B"};

char auxc[30], auxb[30];

void clearTFT()
{
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
}

void touch_calibrate()
{
  return;   // NO FUNCIONA CORRECTAMENTE
  uint16_t calData[5];
  // Calibrate
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 40);
  tft.setTextFont(1);
  tft.setTextSize(2);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.println("Touch corners as indicated");

  tft.calibrateTouch(calData, TFT_YELLOW, TFT_BLACK, 15);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  for (byte i=0;i<5;i++) conf.calData[i]=calData[i];
}

void TFT_Init()
{
  tft.init();
  tft.setRotation(1);
/**  if (conf.calData[0]==0)
    {
    touch_calibrate();
    tft.setTouch(conf.calData);
    for (byte i=0; i<5; i++) { Serial2.print(conf.calData[i]); Serial2.print("-"); } Serial2.println();
    saveconf();
    ESP.restart();
    }
  else
    tft.setTouch(conf.calData);  **/
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.setTextSize(2);
  tft.drawString("Init...",0,0);
}

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

void clearLine2()
{
  printLine(0,"");
}

void printLine2ClearAndUpdate(){
  printLine(0, "");
  updateDisplay(true,false);
}
//End of Display Base Routines
//==================================================================================

void displayTime()
{
  if (!WiFi.isConnected()) return;
  tft.setTextDatum(MR_DATUM);     // derecha
  btSta[1].initButtonUL(&tft,160,220,30,20,2,TFT_BLACK,TFT_WHITE,itoa(hour(),buff,10),2);
  btSta[1].drawButton();
  btSta[2].initButtonUL(&tft,196,220,30,20,2,TFT_BLACK,TFT_WHITE,itoa(minute(),buff,10),2);
  btSta[2].drawButton();
  btSta[3].initButtonUL(&tft,229,220,30,20,2,TFT_BLACK,TFT_WHITE,itoa(second(),buff,10),2);
  btSta[3].drawButton();
  tft.setTextSize(2);  tft.setTextColor(TFT_WHITE);
  tft.setTextDatum(ML_DATUM);     // izquierda
  tft.drawString(":",188,230);  tft.drawString(":",220,230); 
}

void drawST(boolean flechas, boolean escok)     // barra navegación
{
  if (flechas)
    {
    btSel[0].initButtonUL(&tft,0,200,75,40,2,TFT_WHITE,TFT_BLACK,flecha[0],2);  btSel[0].drawButton();
    btSel[3].initButtonUL(&tft,245,200,75,40,2,TFT_WHITE,TFT_BLACK,flecha[3],2);  btSel[3].drawButton();
    }
  if (escok)
    {
    btSel[1].initButtonUL(&tft,80,200,75,40,2,TFT_WHITE,TFT_BLACK,"ESC",2);  btSel[1].drawButton();
    btSel[2].initButtonUL(&tft,165,200,75,40,2,TFT_WHITE,TFT_BLACK,"OK",2);  btSel[2].drawButton();
    }
  else
    {
    tft.setTextSize(2);
    tft.setTextColor(WiFi.isConnected()?TFT_GREEN:TFT_RED, TFT_BLACK);
    tft.drawString(WiFi.isConnected()?"CON":"DIS",105,220);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawNumber(tftpage,85,220);
    if (hour()<10) tft.drawNumber(0,167,220); 
    tft.drawNumber(hour(),hour()<10?167:155,220); 
    tft.drawString(":",178,220);
    if (minute()<10) tft.drawNumber(0,188,220); 
    tft.drawNumber(minute(),minute()<10?200:188,220); 
    tft.drawString(":",210,220);
    if (second()<10) { tft.drawNumber(0,220,220); }
    tft.drawNumber(second(),second()<10?232:220,220);   
    }
}

void drawAP(byte nact)
{
  clearTFT();
  for (int i=0; i<nAP; i++)
    if (WiFi.RSSI(i)>-90)
      {
      WiFi.SSID(i).toCharArray(auxchar, 20);
      if (i==tftapactual) { tft.setTextColor(TFT_BLACK, TFT_WHITE);    }
      tft.drawNumber(i,0,i*20);
      tft.drawString(auxchar,20,i*20); 
      tft.drawNumber(WiFi.RSSI(i),240,i*20);
      tft.drawString("dbm",280,i*20); 
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      }
  drawST(true,true);
}

void selectapTFT()
{
  tftpage=21; 
  nAPact=0;
  tftapactual=0;
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  clearTFT();
  tft.drawString("Scanning...",0,0); 
  nAP=WiFi.scanNetworks(false, false);  if (nAP>maxAP) nAP=maxAP;
  drawAP(tftapactual);
}

void displayStatus(boolean flechas, boolean escok)
{
  if (escok)
    {
    displayYN(1,1,0);
    }
  else
    {
    btSta[0].initButtonUL(&tft,98,210,30,30,2,WiFi.isConnected()?TFT_GREEN:TFT_RED,TFT_BLACK,"WiFi",1);
    btSta[0].drawButton();
    btSta[4].initButtonUL(&tft,130,210,30,30,2,TFT_WHITE,TFT_BLACK,"Rst",1);
    btSta[4].drawButton();
    displayTime();
    }
}

void displayFreq()
{
  char freqpart[9][4]={"","","","","","","","",""};
  char freqpartSec[9][4]={"","","","","","","","",""};
  unsigned long f=conf.frequency;
  unsigned long fsec;
  int16_t colf;
  if (conf.ritOn==1) colf=TFT_CYAN; else if (conf.splitOn==1) colf=TFT_ORANGE; else colf=TFT_GREEN;

  if (conf.ritOn==1) fsec=conf.ritTxFrequency;
  else if (conf.splitOn==1) fsec=conf.frequencyB;
  else fsec=conf.vfoActive==VFO_A?conf.frequencyB:conf.frequencyA;
  
  for (byte i=0;i<9;i++)    // separa cifras
    {
    long auxL=long(pow(10,8-i));
    if ((i==0)&&(f<100000000)) strcat(freqpart[i]," "); 
    else if ((i==1)&&(f<10000000)) strcat(freqpart[i]," "); 
    else strcat(freqpart[i],itoa((f/auxL)%10,buff,10));
    if ((i==0)&&(fsec<100000000)) strcat(freqpartSec[i]," "); 
    else if ((i==1)&&(fsec<10000000)) strcat(freqpartSec[i]," "); 
    else strcat(freqpartSec[i],itoa((fsec/auxL)%10,buff,10));
    }
  char auxchar[2][5]={"A","B"};
  if (conf.vfoActive==VFO_B) { strcpy(auxchar[0],"B"); strcpy(auxchar[1],"A"); }
  
  btVFO[1].initButtonUL(&tft,0,0,20,20,2,TFT_BLACK,colf,auxchar[1],2);
  btVFO[1].drawButton();
  tft.setTextSize(2); tft.setTextColor(colf);
  tft.setTextDatum(ML_DATUM);
  if ((conf.ritOn==1) || (conf.splitOn==1))
    {
    tft.drawString("TX",21,0);
    if (conf.splitOn==1) tft.drawString(conf.isUSBB==0?"LSB":"USB",70,0);
    }
  else
    {
    if (conf.vfoActive==VFO_A)
      {
      tft.drawString(conf.cwModeB==0?"   ":"CW-",35,0);
      tft.drawString(conf.isUSBB==0?"LSB":"USB",70,0);
      }
    else
      {
      tft.drawString(conf.cwModeA==0?"   ":"CW-",35,0);
      tft.drawString(conf.isUSBA==0?"LSB":"USB",70,0);
      }
    }
  for (byte i=0;i<9;i++)    // frecuencia secundaria 
    {
    btFreqTx[i].initButtonUL(&tft,140+12*i,0,12,20,2,TFT_BLACK,colf,freqpartSec[i],2);
    btFreqTx[i].drawButton();
    }
  tft.fillCircle(173,14,1,colf);    // punto de miles

  btVFO[0].initButtonUL(&tft,0,30,30,30,2,TFT_BLACK,TFT_WHITE,auxchar[0],3);
  btVFO[0].drawButton();
  for (byte i=0;i<9;i++)      // frecuencia principal  
    {
    if (i<6) btFreq[i].initButtonUL(&tft,35+26*i,30,26,40,2,TFT_BLACK,conf.tuneStepIndex==i?TFT_YELLOW:TFT_WHITE,freqpart[i],4);
    else btFreq[i].initButtonUL(&tft,71+20*i,38,20,30,2,TFT_BLACK,conf.tuneStepIndex==i?TFT_YELLOW:TFT_WHITE,freqpart[i],3);
    btFreq[i].drawButton();
    }
  tft.fillCircle(110,62,2,TFT_WHITE);    // punto de miles
}

void displayNav()   // botones navegación
{
  strcpy(btNavtext[0],tftpage==22?"Ed":"<");
  strcpy(btNavtext[2],tftpage==22?"Ok":">");
  for (byte i=0;i<5;i++) if (btNavact[i]==1)
    {
    btNav[i].initButtonUL(&tft,32*i,210,30,30,2,TFT_WHITE,TFT_BLACK,btNavtext[i],2);
    btNav[i].drawButton();
    }
}

void displayYN(byte en0, byte en1, byte en2)
{
  // botones Yes/Cancel
  btYNact[0]=en0; btYNact[1]=en1; btYNact[2]=en2;
  for (byte i=0;i<3;i++)
    if (btYNact[i]==1)
      {
      btYN[i].initButtonUL(&tft,100*i,210,90,30,2,TFT_WHITE,TFT_BLACK,btYNtext[i],2);
      btYN[i].drawButton();
      }
}

void displayCal()
{
  // botones setting
  for (byte i=0;i<5;i++) if (btCalact[i]==1)
    {
    btCal[i].initButtonUL(&tft,0,35*i+30,160,30,2,TFT_WHITE,TFT_BLACK,btCaltext[i],2);
    btCal[i].drawButton();
    }
}

void displayCWSet()
{
  // botones setting
  for (byte i=0;i<5;i++) if (btCWact[i]==1)
    {
    btCW[i].initButtonUL(&tft,0,35*i+30,160,30,2,TFT_WHITE,TFT_BLACK,btCWtext[i],2);
    btCW[i].drawButton();
    }
}

void displayKey(byte mode) // 0: all keys, 1:numeric, 2:alfa only  
{ 
  if (mode<=1)
    for (byte i=0;i<10;i++)
      {
      btKey[i].initButtonUL(&tft,31*(i%10),35+(31*int(i/10)),30,30,2,TFT_WHITE,TFT_BLACK,btKeytext[i],2);
      btKey[i].drawButton();
      }
  if(mode !=1)
    for (byte i=10;i<50;i++)
      {
      btKey[i].initButtonUL(&tft,31*(i%10),35+(31*int(i/10)),30,30,2,TFT_WHITE,TFT_BLACK,btKeytext[i],2);
      btKey[i].drawButton();
      }
  displayYN(1,1,1);
}

void displaySet()
{
  // botones setting
  for (byte i=0;i<5;i++) if (btSetact[i]==1)
    {
    btSet[i].initButtonUL(&tft,0,35*i+30,160,30,2,TFT_WHITE,TFT_BLACK,btSettext[i],2);
    btSet[i].drawButton();
    }
}

void displaySetRad()
{
  // botones setting radio
  for (byte i=0;i<5;i++) if (btSetRadact[i]==1)
    {
    btSetRad[i].initButtonUL(&tft,0,35*i+30,160,30,2,TFT_WHITE,TFT_BLACK,btSetRadtext[i],2);
    btSetRad[i].drawButton();
    }
}

void displayMemMan()
{
  // botones setting radio
  for (byte i=0;i<5;i++) 
    {
    btMemMan[i].initButtonUL(&tft,0,35*i+30,160,30,2,TFT_WHITE,TFT_BLACK,btMemMantext[i],2);
    btMemMan[i].drawButton();
    }
}

void displayMemList()
{
  tft.setTextSize(2); 
  for (int i=0; i<7; i++)
    {
    if (memlin==i)
      {
      btMemN[i].initButtonUL(&tft,0,70+20*i,40,20,2,TFT_WHITE,TFT_BLACK,itoa(mempos+i-memlin,buff,10),2);
      btMemN[i].drawButton();
      tft.setTextColor(TFT_BLACK,TFT_WHITE);
      }
    else
      {
      btMemN[i].initButtonUL(&tft,0,70+20*i,40,20,2,TFT_BLACK,TFT_WHITE,itoa(mempos+i-memlin,buff,10),2);
      btMemN[i].drawButton();
      tft.setTextColor(TFT_WHITE,TFT_BLACK);
      }
    tft.drawString("                       ",45,80+20*i);    
    if (memo.act[mempos+i-memlin]==1)
      {
      tft.drawString(memo.descr[mempos+i-memlin],45,80+20*i);    
      tft.drawString(memo.vfoActive[mempos+i-memlin]==VFO_A?"A":"B",160,80+20*i);    
      tft.drawString(memo.isUSB[mempos+i-memlin]==0?"LSB":"USB",180,80+20*i);    
      tft.drawString(memo.cwMode[mempos+i-memlin]==0?"  ":"CW",220,80+20*i);    
      tft.drawString(memo.ritOn[mempos+i-memlin]==0?"   ":"Rit",250,80+20*i);    
      tft.drawString(memo.splitOn[mempos+i-memlin]==0?"   ":"SPL",290,80+20*i);    
      }
    }
}

void displayNet()
{
  // botones setting Net
  btNet[0].initButtonUL(&tft,0,35*0+30,160,30,2,conf.autoWiFi==1?TFT_YELLOW:TFT_WHITE,TFT_BLACK,btNettext[0],2);
  btNet[0].drawButton();
  for (byte i=1;i<4;i++) if (btNetact[i]==1)
    {
    btNet[i].initButtonUL(&tft,0,35*i+30,160,30,2,TFT_WHITE,TFT_BLACK,btNettext[i],2);
    btNet[i].drawButton();
    }
  btNet[4].initButtonUL(&tft,0,35*4+30,160,30,2,conf.staticIP==1?TFT_YELLOW:TFT_WHITE,TFT_BLACK,btNettext[4],2);
  btNet[4].drawButton();
}

void displayFlot()
{
  // botones flotantes
  btFlot[0].initButtonUL(&tft,260,140,60,30,2,TFT_WHITE,TFT_BLACK,btFlottext[0],2);
  btFlot[0].drawButton();
  btFlot[1].initButtonUL(&tft,260,105,60,30,2,scanF==1?TFT_CYAN:TFT_WHITE,TFT_BLACK,btFlottext[1],2);
  btFlot[1].drawButton();
  btFlot[2].initButtonUL(&tft,260,70,60,30,2,scanF==2?TFT_CYAN:TFT_WHITE,TFT_BLACK,btFlottext[2],2);
  btFlot[2].drawButton();
  btFlot[3].initButtonUL(&tft,260,210,60,30,2,keylock==1?TFT_RED:TFT_WHITE,TFT_BLACK,btFlottext[3],2);
  btFlot[3].drawButton();
  strcpy(btFlottext[4],conf.memMode==0?">MEM":">VFO");
  btFlot[4].initButtonUL(&tft,260,175,60,30,2,TFT_WHITE,TFT_BLACK,btFlottext[4],2);
  btFlot[4].drawButton();
}

void displayMain()
{
  initButtons();
  // botones superiores
  for (byte i=0;i<10;i++)
    {
    btMain[i].initButtonUL(&tft,btMainposx[i],btMainposy[i],60,30,2,btMainact[i]==1?btMaincol[i]:TFT_WHITE,TFT_BLACK,btMaintext[i],2);
    btMain[i].drawButton();
    }
}

void updateDisplay(boolean alldata, boolean freqdata) {
  freqdata=true; 
  clearTFT();
  if (tftpage==0)   // Main page
    {
    if (alldata) 
      {
      displayMain();
      displayNav();
      displayFlot(); 
      displayStatus(true,false);    // time, status
      }
    displayFreq();   // frecuencia
    }
  else if (tftpage==1)    // Setup
    {
    displaySet();
    displayNav();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("1 - Setup",0,0);
    tft.drawString(conf.lang==0?"Español":"English",180,40);
    tft.drawString(conf.CallSign,180,75);
    tft.drawNumber(conf.latitud,180,110);
    tft.drawNumber(conf.longitud,180,145);
    }
  else if (tftpage==2)    // WiFi
    {
    displayNet();
    displayNav();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("2 - Setup WiFi",0,0);
    tft.drawString(conf.autoWiFi==1?"YES":"NO",180,40);
    tft.drawString(conf.ssidSTA,180,75);
    tft.drawString(conf.passSTA,180,110);
    tft.drawString(conf.wifimode==0?"AP":conf.wifimode==1?"STA":"AP+STA",180,145);
    tft.drawString(conf.staticIP==1?"YES":"NO",180,180);
    }
  else if (tftpage==3)    // Calibration
    {
    displayCal();
    displayNav();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("3 - Calibration",0,0);
    tft.drawNumber(conf.calibration,180,40);
    tft.drawNumber(conf.usbCarrier,180,75);
    tft.drawNumber(conf.SI5351BX_ADDR,180,110);
    }
  else if (tftpage==4)    // CW setting
    {
    displayCWSet();
    displayNav();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("4 - CW setting",0,0);
    tft.drawString(keytypetext[conf.cwKeyType],180,40);
    tft.drawNumber(1200/conf.cwSpeed,180,75);
    tft.drawNumber(conf.sideTone,180,110);
    tft.drawNumber(conf.cwDelayTime,180,145);
    tft.drawNumber(conf.delayBeforeCWStartTime,180,180);
    }
  else if (tftpage==5)    // Setup radio
    {
    displaySetRad();
    displayNav();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("5 - Radio Setup",0,0);
    tft.drawString(conf.scanallf==0?"Ham Bands":"All Range",180,40);
    tft.drawString(conf.TXall==0?"Ham Bands":"All Range",180,75);
    tft.drawString(conf.autoMode==0?"NO":"YES",180,110);
    }
  else if (tftpage==6)    // Mem manage
    {
    displayMemMan();
    displayNav();
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString("6 - Manage Memories",0,0);
    tft.drawString("Ham Bands",180,40);
    tft.drawString("All Range",180,75);
    tft.drawString("YES",180,110);
    }
  else if (tftpage==21)    // select AP
    {
    }
  else if (tftpage==22)    // Mem display and select
    {
    displayNav();
    displayStatus(true,false);    // time, status
    displayFreq();   // frecuencia
    displayMemList();
    }
}

void DisplayVersionInfo(const __FlashStringHelper * fwVersionInfo)
{
  tft.drawString("Version:",0,20);
  tft.drawString(fwVersionInfo,140,20);
}

void  initButtons()
{
  btMainact[0]=inTx==1?1:0;         // RX / TX
  btMainact[1]=conf.memMode==0?0:1; // V/M
  btMainact[2]=0;                   // Band Down
  btMainact[3]=0;                   // Band Up
  btMainact[4]=conf.isUSB==1?1:0;   // LSB / USB
  if (conf.vfoActive==VFO_A)
    {
    btMainact[4]=conf.isUSBA==1?1:0;   // LSB / USB
    strcpy(btMaintext[4],conf.isUSBA==1?"USB":"LSB");   // LSB / USB
    }
  else
    {
    btMainact[4]=conf.isUSBB==1?1:0;   // LSB / USB
    strcpy(btMaintext[4],conf.isUSBB==1?"USB":"LSB");   // LSB / USB
    }
  btMainact[5]=conf.cwMode>0?1:0;   // CW
  btMainact[6]=conf.ritOn==1?1:0;   // RIT
  btMainact[7]=conf.splitOn==1?1:0; // Split
  btMainact[8]=isIFShift==1?1:0;    // IFS
  btMainact[9]=conf.attLevel>0?1:0; // ATT
}

int checkYN()
{
  uint16_t x, y;
  if (tft.getTouch(&x, &y))
    {
    if (tft.getRotation()==1) { y=tft.height()-y;}
    else if (tft.getRotation()==3) { x=tft.width()-x; y=tft.height()-y;}
    for (byte i=0;i<3;i++)
      if (btYN[i].contains(x,y)) { delay(100); return i;  }
    }
  return -1;
}

long getNumberTFT(long valini, byte len, char *tunits)
{
  clearTFT();
  tft.setTextSize(4); tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(tunits,200,0);
  displayKey(1);
  strcpy(auxtft,itoa(valini,buff,10));
  tft.setTextSize(4); tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(auxtft,0,0);
  uint16_t x, y;
  while (true) 
    {
    if (tft.getTouch(&x, &y))
      {
      if (tft.getRotation()==1) { y=tft.height()-y;}
      else if (tft.getRotation()==3) { x=tft.width()-x; y=tft.height()-y;}
      for (byte i=0; i<10; i++)    // check KeyN buttons
        {
        if (btKey[i].contains(x,y)) 
          {
          delay(100);
          if (strlen(auxtft)<len)
            {
            strcat(auxtft, itoa(i,buff,10));
            tft.setTextSize(4); tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString(auxtft,0,0);
            }
          }
        }
      int result=checkYN();
      if (result==0) { return atol(auxtft);   }   // OK
      else if (result==1) { return -1; }     // Cancel
      else if (result==2)                    // Backspace
        { 
        if (strlen(auxtft)>0) auxtft[strlen(auxtft)-1]=0; 
        tft.drawString("                    ",0,0);  
        tft.drawString(auxtft,0,0); }     
      }
    }
}

int getCharTFT(char *valini, byte tam)
{
  clearTFT();
  displayKey(0);
  uint16_t x, y;
  strcpy(auxtft,valini);
  tft.setTextSize(4); tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(auxtft,0,0);
  while (true) 
    {
    if (tft.getTouch(&x, &y))
      {
      if (tft.getRotation()==1) { y=tft.height()-y;}
      else if (tft.getRotation()==3) { x=tft.width()-x; y=tft.height()-y;}
      for (byte i=0; i<50; i++)    // check KeyA buttons
        {
        if (btKey[i].contains(x,y)) 
          {
          if (strlen(auxtft)<tam)
            {
            strcat(auxtft, btKeytext[i]);
            tft.setTextSize(4); tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.drawString(auxtft,0,0);
            }
          delay(100);
          }
        }
      int result=checkYN();
      if (result==0) { return strlen(auxtft);   }        // OK
      else if (result==1) { return -1; }  // Cancel
      else if (result==2)                 // Backspace
        { 
        if (strlen(auxtft)>0) auxtft[strlen(auxtft)-1]=0; 
        tft.drawString("                    ",0,0);  
        tft.drawString(auxtft,0,0); }     
      }
    }
}

void editMEM()
{
  
}

void checkNavButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)     // navigation buttons
  {
   if (btNav[i].contains(x,y)) 
     {
     delay(100);
     if (i==0)      
       { 
       if (tftpage==22)     // lista memorias
         editMEM();         // edit memoria
       else
         tftpage=tftpage>0?tftpage-1:6; // next page
       }     
     else if (i==1) { if (tftpage==22) conf.memMode=0; tftpage=0; }               // main page
     else if (i==2) 
       { 
       if (tftpage==22) { conf.memMode=0; tftpage=0; }
       else  { tftpage=tftpage<6?tftpage+1:0; }    // prior page
       }
     else if (i==3) {  }
     else if (i==4) {  }
     updateDisplay(true,false);
     }
  }
}

void checkStaButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)   // buttons status line
    {
     if (btSta[i].contains(x,y)) 
       {
       if (i==0)              // WiFi
         { if (WiFi.isConnected()) 
             WiFi.disconnect();  
           else
             {
             initWiFi();
             initFTP();
             initHTML();
             initTime();
             initWebserver();
             checkMyIP();
             }
         }
       else if (i==1) {     }
       else if (i==2) {     }
       else if (i==3) {     }
       else if (i==4) { ESP.restart(); }    // reset
       updateDisplay(true,false);
       }
    }
}

void checkSelButtons(uint16_t x, uint16_t y)
{
  for (byte i=0;i<4;i++)  // botones Cancel,OK,N,Backspace
    { 
    if (btSel[i].contains(x,y))  
      { 
      if (i==0) { tftapactual--; if (tftapactual<0) tftapactual=nAP-1; drawAP(tftapactual);}
      else if (i==1) { tftpage=2; updateDisplay(true,false); }
      else if (i==2) { WiFi.SSID(tftapactual).toCharArray(conf.ssidSTA, 20); saveconf(); tftpage=2; updateDisplay(true,false); }
      else if (i==3) { tftapactual++; if (tftapactual>nAP-1) tftapactual=0; drawAP(tftapactual);}
      }  
    }
}

void startScan(byte dir)
{
  if ((conf.actualBand==99) && (conf.scanallf==0))
    {
    tftErrormsg("OUT OF BAND","Modify parameter","Scan mode");
    tftpage=5;
    updateDisplay(true,true);
    return;
    }
  scanF=scanF==dir?0:dir; 
  updateDisplay(true,false);   
}

void stopScan() { scanF=0; }

void checkFlotButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5; i++)    // check float buttons
    { 
    if (btFlot[i].contains(x,y)) 
      {
      if (i==3) { keylock=keylock==0?1:0; updateDisplay(true,false); }    // lock key
      if (keylock==1) return; 
      if (i==0)     // ent frequency
        {
        stopScan();
        long auxL=getNumberTFT(conf.frequency/1000,5,"Khz");  // retorna con OK o Cancel
        if (auxL!=-1) { setFrequency(auxL*1000); }
        updateDisplay(true,false);
        }
      else if (i==1) { startScan(1); }     // scan down
      else if (i==2) { startScan(2); }     // scan up
      else if (i==4)        // VFO to Mem / Mem to VFO
        { 
        if (conf.memMode==0)   // VFO mode
          { 
          conf.lastmempos=mempos; 
          saveVFOtoMem();
          updateDisplay(true,true); 
          }
        else               // Mem mode
          { 
          mempos=conf.lastmempos; 
          setMEMtoVFO(mempos); 
          }          
        saveconf();
        }     
      }
    }
}

void checkFreqButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<9; i++)    // check step buttons
    { 
    if (btFreq[i].contains(x,y)) 
      { 
      conf.tuneStepIndex=i; 
      if (i<=5) conf.frequency=conf.frequency/1000 *1000;  
      else if (i==6) conf.frequency=conf.frequency/100 *100;  
      else if (i==7) conf.frequency=conf.frequency/10 *10;  
      displayFreq(); 
      } 
    }
}

void checkVFOButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<2;i++)    // check buttons VFO
    {
    if (btVFO[i].contains(x,y)) 
      {
      setVFO(conf.vfoActive==VFO_A?VFO_B:VFO_A);
      updateDisplay(true,false);
      return;
      }
    }
}

void checkMainButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<10;i++)    // check buttons function
    {
    if (btMain[i].contains(x,y)) 
      {
      if (i==0)   // RX / TX
        { 
        if (inTx==1) { txTFT=false; stopTx(); }  // RX
        else { txTFT=true; startTx(TX_SSB,1); }  // TX
        strcpy(btMaintext[0],inTx==0?"RX":"TX");
        }   
      else if (i==1)    // V/M
        { 
        conf.memMode=conf.memMode==0?1:0; 
        firstmem=conf.memMode==1;
        tftpage=conf.memMode==0?0:22;
        }
      else if (i==2)    //Prior Band
        { 
        conf.ritOn=0; btMainact[6]=0;
        strcpy(btMaintext[4],conf.isUSB==1?"USB":"LSB"); 
        setNextHamBandFreq(conf.frequency,-1);
        } 
      else if (i==3)    //Next Band
        { 
        conf.ritOn=0; btMainact[6]=0;
        strcpy(btMaintext[4],conf.isUSB==1?"USB":"LSB"); 
        setNextHamBandFreq(conf.frequency, 1); 
        }  
      else if (i==4)   // LSB-USB 
        { 
        setSSBMode(); 
        btMainact[4],conf.isUSB==1?1:0; 
        strcpy(btMaintext[4],conf.isUSB==1?"USB":"LSB"); 
        setFrequency(conf.frequency);
        }
      else if (i==5)   // CW 
        { 
        setCWMode(conf.cwMode==0?1:0); 
        btMainact[5]=(conf.cwMode>0);
        setFrequency(conf.frequency);
        }
      else if (i==6)    // RIT
        {
        conf.ritOn=conf.ritOn==0?1:0;
        btMainact[i]=(conf.ritOn==1); 
        if (conf.ritOn==1) 
          {
          conf.splitOn=0;
          conf.ritTxFrequency = conf.frequency;
          }
        else
          {
          setFrequency(conf.ritTxFrequency);
          }
        }
      else if (i==7)    // SPL
        {
        conf.splitOn=conf.splitOn==0?1:0;
        btMainact[i]=conf.splitOn==1;
        if (conf.splitOn==1) { conf.ritOn=0; setFrequency(conf.ritTxFrequency);; }
        }
      else if (i==8)    // IFS
        {
        isIFShift=isIFShift==0?1:0; btMainact[i]=isIFShift==1?1:0;
        }
      else if (i==9)    // ATT
        {
        if (conf.attLevel==0) conf.attLevel=1; else conf.attLevel=0;
        btMainact[i]=conf.attLevel==0?0:1;
        }
      if (i>0) saveconf();
      updateDisplay(true,true);
      }
    }
}

void checkSetButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)
  {
   if (btSet[i].contains(x,y)) 
     {
     if (i==0) { conf.lang=conf.lang==0?1:0; }   // language
     else if (i==1)                              // CallSign
       { 
       int auxI=getCharTFT(conf.CallSign,10); 
       if (auxI !=-1) { strcpy(conf.CallSign,auxtft);  }
       }   
     else if (i==2)           // Latitude
       {  
       long auxL=getNumberTFT(conf.latitud,10,"");  
       if (auxL!=-1) { conf.latitud=auxL; }
       }                         
     else if (i==3)               // Longitude
       {  
       long auxL=getNumberTFT(conf.longitud,10,"");  
       if (auxL!=-1) { conf.longitud=auxL; }
       }                         // Latitude
     else if (i==4) {  }                         // 
     saveconf(); 
     updateDisplay(true,false);
     }
  }
}

void checkSetRadButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)
  {
   if (btSetRad[i].contains(x,y)) 
     {
     if (i==0) { conf.scanallf=conf.scanallf==0?1:0; }   // scan mode
     else if (i==1) { conf.TXall=conf.TXall==0?1:0; }    // TX range
     else if (i==2) { conf.autoMode=conf.autoMode==0?1:0; }    // SSB auto                         
     else if (i==3) {  }                      
     else if (i==4) {  }                
     saveconf(); 
     updateDisplay(true,false);
     }
  }
}

void checkMemManButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)
  {
   if (btSetRad[i].contains(x,y)) 
     {
     if (i==0) {  }         // 
     else if (i==1) {  }    // 
     else if (i==2) {  }    //                          
     else if (i==3) {  }    //                     
     else if (i==4) {  }    //          
     saveconf(); 
     updateDisplay(true,false);
     }
  }
}

void checkNetButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)
    {
    if (btNet[i].contains(x,y)) 
      {
      if (i==0)      { conf.autoWiFi=conf.autoWiFi==0?1:0;  }   // AutoConnect
      else if (i==1) { selectapTFT(); }   // SSID
      else if (i==2) { if (getCharTFT(conf.passSTA,10) !=-1) strcpy(conf.passSTA ,auxtft); }  // Password
      else if (i==3) { if (conf.wifimode<2) conf.wifimode++; else conf.wifimode=0; }        // WiFi Mode
      else if (i==4) { conf.staticIP=conf.staticIP==0?1:0; }                                //  Static IP
      saveconf();  
      if (i!=1) updateDisplay(true,false);
      }
    }
}

void checkCalButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)
    {
     if (btCal[i].contains(x,y)) 
       {
       if (i==0)      { setupFreq(); }   // CAL
       else if (i==1) { setupBFO();  }   // BFO
       else if (i==2) 
         {  
          long auxL=getNumberTFT(conf.SI5351BX_ADDR,3,"");  // retorna con OK o Cancel
          if (auxL!=-1) { conf.SI5351BX_ADDR=auxL; }
         }      // 
       else if (i==3) { touch_calibrate(); saveconf(); }      // 
       else if (i==4) { deleteMemo(); } // delete memories
       updateDisplay(true,false);
       }
    }
}

void checkCWButtons(uint16_t x, uint16_t y)
{
  for (byte i=0; i<5;i++)
    {
     if (btCW[i].contains(x,y)) 
       {
       if (i==0) {conf.cwKeyType=conf.cwKeyType==2?0:conf.cwKeyType+1; }   // Key type
       else if (i==1) 
         { 
         int wpm=1200/conf.cwSpeed;
         wpm=getValByKnob(1, wpm, 3, 50, 1, "WPM", 3);
         conf.cwSpeed = 1200/wpm;
         }   
       else if (i==2) {  conf.sideTone=getValByKnob(2, conf.sideTone, 100, 2000, 10, "Tone", 2); }    
       else if (i==3) 
         {  
         int tmpCWDelay=conf.cwDelayTime*10;
         tmpCWDelay=getValByKnob(3, tmpCWDelay, 3, 2500, 10, "Delay", 2);
         conf.cwDelayTime = tmpCWDelay/10;
         }      
       else if (i==4) 
         {
         int tmpTXCWInterval = conf.delayBeforeCWStartTime*2;
         tmpTXCWInterval=getValByKnob(4, tmpTXCWInterval, 0, 500, 10, "Start time", 2);
         conf.delayBeforeCWStartTime=tmpTXCWInterval/2;
         }
       saveconf(); 
       updateDisplay(true,false);
       }
    }
}
void handletfttouch()
{
  uint16_t x, y;
  if (tft.getTouch(&x, &y))
    {
    if (tft.getRotation()==1) { y=tft.height()-y;}
    else if (tft.getRotation()==3) { x=tft.width()-x; y=tft.height()-y;}
    if (tftpage==0)   // main page
      {
      if (scanF>0) { scanF=0; updateDisplay(true,false); return; }
      checkFlotButtons(x,y);
      if (keylock==1) return;
      checkFreqButtons(x,y);
      checkMainButtons(x,y);
      checkVFOButtons(x,y);
      }
    else if (tftpage==1) { checkSetButtons(x,y); }   // Setup page
    else if (tftpage==2) { checkNetButtons(x,y); }   // WiFi page
    else if (tftpage==3) { checkCalButtons(x,y); }   // Calibration page
    else if (tftpage==4) { checkCWButtons(x,y); }    // CW setting page
    else if (tftpage==5) { checkSetRadButtons(x,y); } // radio setting page
    else if (tftpage==6) { checkMemManButtons(x,y); } // radio setting page
    else if (tftpage==21) { checkSelButtons(x,y);}  // Select AP
    else if (tftpage==22) { checkNavButtons(x,y);}  // Select AP
    if (tftpage!=21) { checkNavButtons(x,y); checkStaButtons(x,y); }
    } 
}
