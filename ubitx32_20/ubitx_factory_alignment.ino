#include "ubitx.h"

/**
 * This procedure is only for those who have a signal generator/transceiver tuned to exactly 7.150 and a dummy load 
 */

void btnWaitForClick(){
  while(!btnDown())
    delay(50);
  while(btnDown())
    delay(50);
 delay(50);
}

void factory_alignment(){
  factoryCalibration(1);
  line2DisplayStatus = 1;
  if (conf.calibration == 0){
    printLine(0,"Setup Aborted");
    return;
  }

  //move it away to 7.160 for an LSB signal
  setFrequency(7160000l);
  updateDisplay(true,false);
  printLine(0,"#2 BFO");
  delay(1000);

  conf.usbCarrier = 11053000l;
  menuSetupCarrier(1);
  if (conf.usbCarrier == 11053000l){
    printLine(0,"Setup Aborted");
    return;
  }
  
  printLine(0,"#3:Test 3.5MHz");
  conf.cwMode = 0;
  conf.isUSB = false;
  setFrequency(3500000l);
  updateDisplay(true,false);

  while (!btnDown()){
    checkPTT();
    delay(100);
  }

  btnWaitForClick();
  printLine(0,"#4:Test 7MHz");

  setFrequency(7150000l);
  updateDisplay(true,false);
  while (!btnDown()){
    checkPTT();
    delay(100);
    }

  btnWaitForClick();
  printLine(0,"#5:Test 14MHz");

  conf.cwMode = 0;
  conf.isUSB = 1;
  setFrequency(14000000l);
  updateDisplay(true,false);
  while (!btnDown()){
    checkPTT();
    delay(100);
  }

  btnWaitForClick();
  printLine(0,"#6:Test 28MHz");

  setFrequency(28000000l);
  updateDisplay(true,false);
  while (!btnDown()){
    checkPTT();
    delay(100);
  }

  printLine(0,"Alignment done");
  delay(1000);

  conf.cwMode = 0;
  conf.isUSB = false;
  setFrequency(7150000l);
  updateDisplay(true,false);  
  
}
