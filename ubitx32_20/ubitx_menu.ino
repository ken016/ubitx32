/*
This source code started with Farhan's original source. The license rules are followed as well.
Calibration related functions kept the original source except for the minor ones. 
The part is collected in the last minute of this source.
Ian KD8CEC
 */
 
#include "ubitx.h"
#include "ubitx_eemap.h"

//Commonly called functions when exiting menus by KD8CEC

int32_t getValByKnob(int valueType, int targetValue, int32_t minValue, int32_t maxValue, int incStep, const char* Title, int Sensitivity)
{
    int knob;
    int moveDetectStep=0;
    int antvalue=targetValue;
    clearTFT();
    tft.setTextSize(3); tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(Title,0,20);
    tft.drawNumber(targetValue,220,20);
    displayYN(1,1,0);
    tft.setTextSize(3);  tft.setTextColor(TFT_WHITE, TFT_BLACK);
    while(!btnDown())
      {
      knob = enc_read();
      if (knob != 0)
        {
        moveDetectStep += (knob > 0 ? 1 : -1);
        if (moveDetectStep < (Sensitivity * -1)) {
          if (targetValue > minValue)
            targetValue -= incStep;
          moveDetectStep = 0;
          }
        else if (moveDetectStep > Sensitivity) {
          if (targetValue < maxValue)
            targetValue += incStep;
          moveDetectStep = 0;
          }
        tft.drawNumber(targetValue,220,20);
        }
      int auxres=checkYN();
      if (auxres==0) return targetValue;
      else if (auxres==1) return antvalue;
      Check_Cat(0);  //To prevent disconnections
      }
    return targetValue;
}

//IF Shift function, BFO Change like RIT, by KD8CEC
void menuIFSSetup(int btn){
  if (!btn)
    {
    if (isIFShift == 1)
      printLineF(0,F("IF Shift Change?"));
    else
      printLineF(0,F("IF Shift On?"));
    }
  else 
    {
    isIFShift = 1;
    conf.ifShiftValue = getValByKnob(2, conf.ifShiftValue, -20000, 20000, 50, "IFS", 2); 
    delay_background(500, 0); //for check Long Press function key
    if (btnDown() || conf.ifShiftValue == 0)
      {
      isIFShift = 0;
      conf.ifShiftValue = 0;
      setFrequency(conf.frequency);
      SetCarrierFreq();
      }
    saveconf();
    }
}

void setupFreq(){
  clearTFT();
  int knob = 0;
  int32_t prev_calibration;
  //round off the the nearest khz
  conf.frequency = (conf.frequency/1000l)* 1000l;
  setFrequency(conf.frequency);
  delay(100);
  tft.drawNumber(conf.calibration,180,40);
  prev_calibration = conf.calibration;
  tft.drawString("You should have a signal",0,130);
  tft.drawString("exactly at      Khz",0,150);
  tft.drawNumber(conf.frequency/1000l,135,150);
  tft.drawString("Rotate to zerobeat",0,170);
  displayYN(1,1,0);
  int result=-1;
  while (result==-1)
    {
    tft.fillRect(190, 40, 160, 30, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawNumber(conf.calibration,180,40);
    if (btnDown()) result=0; else result=checkYN();
    knob = enc_read();
    if (knob != 0)
      {
      conf.calibration += knob * 875;
      si5351bx_setfreq(0, conf.usbCarrier);  //set back the carrrier oscillator anyway, cw tx switches it off  
      si5351_set_calibration(conf.calibration);
      setFrequency(conf.frequency);
      }
    }
  if (result==0)
    {
    saveconf();
    initOscillators();
    si5351_set_calibration(conf.calibration);
    }
  setFrequency(conf.frequency);    
}

void setupBFO(){
  clearTFT();
  int knob = 0;
  unsigned long prevCarrier;
  prevCarrier = conf.usbCarrier;
  tft.drawNumber(conf.usbCarrier,180,70);
  tft.drawString("Set BFO",0,130);
  tft.drawString("Press TUNE to Save",0,170);
  displayYN(1,1,0);
  si5351bx_setfreq(0, conf.usbCarrier);
  //while (!btnDown()){
  int result=-1;
  while (result==-1)
    {
    tft.fillRect(190, 70, 160, 30, TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawNumber(conf.usbCarrier,180,70);
    if (btnDown()) result=0; else result=checkYN();
    knob = enc_read();
    if (knob != 0)
      {
      conf.usbCarrier += 50 * knob;
      si5351bx_setfreq(0, conf.usbCarrier);
      setFrequency(conf.frequency);
      delay(100);
      }
  }
  saveconf();
  si5351bx_setfreq(0, conf.usbCarrier);          
  setFrequency(conf.frequency);    
}

/* Take a deep breath, math(ematics) ahead
 * The 25 mhz oscillator is multiplied by 35 to run the vco at 875 mhz
 * This is divided by a number to generate different frequencies.
 * If we divide it by 875, we will get 1 mhz signal
 * So, if the vco is shifted up by 875 hz, the generated frequency of 1 mhz is shifted by 1 hz (875/875)
 * At 12 Mhz, the carrier will needed to be shifted down by 12 hz for every 875 hz of shift up of the vco
 */

 //this is used by the si5351 routines in the ubitx_5351 file
//extern int32_t calibration;
extern uint32_t si5351bx_vcoa;
extern int enc_read(void);
