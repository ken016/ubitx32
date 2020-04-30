/*
This source code started with Farhan's original source. The license rules are followed as well.
Calibration related functions kept the original source except for the minor ones. 
The part is collected in the last minute of this source.
Ian KD8CEC
 */
 
#include "ubitx.h"
#include "ubitx_eemap.h"

//Current Frequency and mode to active VFO by KD8CEC
void FrequencyToVFO(byte isSaveFreq)
{
  //Save Frequency & Mode Information
  if (conf.vfoActive == VFO_A)
    {
    conf.vfoA = conf.frequency;
    conf.vfoA_mode = modeToByte();
    if (isSaveFreq)
      storeFrequencyAndMode(1);
    }
  else
    {
    conf.vfoB = conf.frequency;
    conf.vfoB_mode = modeToByte();
    if (isSaveFreq)
      storeFrequencyAndMode(2);
    }
}

//Commonly called functions when exiting menus by KD8CEC
void menuClearExit(int delayTime)
{
  if (delayTime > 0)
    delay_background(delayTime, 0);
  printLine2ClearAndUpdate();
  menuOn = 0;
}

//Ham band or general band movement by KD8CEC
void menuBand(int btn){
  int knob = 0;
  int stepChangeCount = 0;
  byte btnPressCount = 0;

  if (!btn){
   printLineF(0,F("Band Select?"));
   return;
    }

  //printLineF(0,F("Press to confirm"));
  //wait for the button menu select button to be lifted)
  while (btnDown()) {
    delay_background(50, 0);
    if (btnPressCount++ > 20) {
      btnPressCount = 0;
      if (conf.tuneTXType > 0) { //Just toggle 0 <-> 2, if tuneTXType is 100, 100 -> 0 -> 2
        conf.tuneTXType = 0;
        printLineF(0,F("General"));
        }
      else {
        conf.tuneTXType = 2;
        printLineF(0,F("Ham band"));
        }
      delay_background(1000, 0);
      printLine2ClearAndUpdate();
      }
    }
  printLineF(0,F("Press to confirm"));
  
  byte currentBandIndex = -1;
  //Save Band Information
  if (conf.tuneTXType == 2 || conf.tuneTXType == 3 || conf.tuneTXType == 102 || conf.tuneTXType == 103) { //only ham band move
    //Get Now Band Index
    currentBandIndex = getIndexHambanBbyFreq(conf.frequency);
    
    if (currentBandIndex >= 0) {
      //Save Frequency to Band Frequncy Record
      saveBandFreqByIndex(conf.frequency, modeToByte(), currentBandIndex);
      }
    }
  
  //delay(50);    
  ritDisable();

  while(!btnDown()){
    knob = enc_read();
    if (knob != 0){
      if (conf.tuneTXType == 2 || conf.tuneTXType == 3 || conf.tuneTXType == 102 || conf.tuneTXType == 103) { //only ham band move
        if (knob < 0) {
          if (stepChangeCount-- < -3) {
            setNextHamBandFreq(conf.frequency, -1);  //Prior Band
            stepChangeCount = 0;
            }
          }
        else if (knob > 0) {
          if (stepChangeCount++ > 3) {
            setNextHamBandFreq(conf.frequency, 1); //Next Band
            stepChangeCount = 0;
            }
          }
        }       //end of only ham band move
      else {  //original source
        if (knob < 0 && conf.frequency > 3000000l)  setFrequency(conf.frequency - 200000l);
        if (knob > 0 && conf.frequency < 30000000l) setFrequency(conf.frequency + 200000l);
        conf.isUSB=(conf.frequency > 10000000l);
        }
      updateDisplay(true,false);
    }
    delay_background(20, 0);
  }
  FrequencyToVFO(1);
  menuClearExit(500);
}

//Convert Mode, Number by KD8CEC
//0: default, 1:not use, 2:LSB, 3:USB, 4:CWL, 5:CWU, 6:FM
byte modeToByte(){
  if (conf.cwMode == 0)
    {
    return conf.isUSB?3:2;
    }
  else if (conf.cwMode == 1)
    {
    return 4;
    }
  else
    {
    return 5;
    }
}

//Convert Number to Mode by KD8CEC
//autoSetModebyFreq : 0
//autoSetModebyFreq : 1, if (modValue is not set, set mode by frequency)
void byteToMode(byte modeValue, byte autoSetModebyFreq){
  if (modeValue == 4)  conf.cwMode = 1;
  else if (modeValue == 5) conf.cwMode = 2;
  else
    {
    conf.cwMode = 0;
    if (modeValue == 3) conf.isUSB = 1;
    else if (autoSetModebyFreq == 1 && (modeValue == 0))
      conf.isUSB = (conf.frequency > 10000000l) ? true : false;
    else
      conf.isUSB = 0;
  }
}

//Memory to VFO, VFO to Memory by KD8CEC
void menuCHMemory(int btn, byte isMemoryToVfo){
  int knob = 0;
  int selectChannel = 0;
  byte isDisplayInfo = 1;
  int moveStep = 0;
  unsigned long resultFreq, tmpFreq = 0;
  byte loadMode = 0;
  
  if (!btn){
    printLineF(0,isMemoryToVfo == 1?F("Channel To VFO?"):F("VFO To Channel?"));
    }
  else {
    delay_background(500, 0);
    while(!btnDown()){
      if (isDisplayInfo == 1) {
        //Display Channel info *********************************
        memset(auxc, 0, sizeof(auxc));
        if (selectChannel >= 20 || selectChannel <=-1)  {
          strcpy(auxc, "Exit?");    }
        else
          {
          //Read Frequency from eeprom
          readconf();
          loadMode = (byte)(resultFreq >> 29);
          resultFreq = resultFreq & 0x1FFFFFFF;

          //display frequency
          tmpFreq = resultFreq;
          for (int i = 15; i >= 6; i--) {
            if (tmpFreq > 0) {
              if (i == 12 || i == 8) auxc[i] = '.';
              else {
                auxc[i] = tmpFreq % 10 + 0x30;
                tmpFreq /= 10;
                }
              }
            else
              auxc[i] = ' ';
            }
          }
        printLine(0,auxc);
        isDisplayInfo = 0;
      }

      knob = enc_read();

      if (knob != 0)
      {
        moveStep += (knob > 0 ? 1 : -1);
        if (moveStep < -3) {
          if (selectChannel > -1)
            selectChannel--;

          isDisplayInfo = 1;
          moveStep = 0;
        }
        else if (moveStep > 3) {
          if (selectChannel < 20)
            selectChannel++;
            
          isDisplayInfo = 1;
          moveStep = 0;
        }
      }

      Check_Cat(0);  //To prevent disconnections
    } //end of while (knob)

    if (selectChannel < 20 && selectChannel >= 0)
      {
      if (isMemoryToVfo == 1)
        {
        if (resultFreq > 3000 && resultFreq < 60000000)
          {
          byteToMode(loadMode, 1);
          setFrequency(resultFreq);
          }
        }
      else
        {
        //Save current Frequency to Channel (selectChannel)
        saveconf();
        printLine(0,"Saved Frequency");
        }
      }
    menuClearExit(500);
    }
}

//VFO Toggle and save VFO Information, modified by KD8CEC
void menuVfoToggle(int btn)
{
  if (!btn){
    if (conf.vfoActive == VFO_A)
      printLineF(0,F("Select VFO B?"));
    else
      printLineF(0,F("Select VFO A?"));    
    }
  else {
      FrequencyToVFO(1);
      if (conf.vfoActive == VFO_B){
        conf.vfoActive = VFO_A;
        conf.frequency = conf.vfoA;
        saveCheckFreq = conf.frequency;
        byteToMode(conf.vfoA_mode, 0);
        }
      else {
        conf.vfoActive = VFO_B;
        conf.frequency = conf.vfoB;
        saveCheckFreq = conf.frequency;
        byteToMode(conf.vfoB_mode, 0);
        }
      ritDisable();
      setFrequency(conf.frequency);
  //Only Clear And Delay for Character LCD
  menuClearExit(0);
  }
}


//Split communication using VFOA and VFOB by KD8CEC
void menuSplitOnOff(int btn){
  if (!btn){
    printLineF(0,conf.splitOn == 0?F("Split On?"):F("Split Off?"));
    }
  else {
      if (conf.splitOn == 1){
        conf.splitOn = 0;
        printLineF(0,F("SPT Off"));
        }
      else {
        conf.splitOn = 1;
        if (conf.ritOn == 1) conf.ritOn = 0;
        printLineF(0,F("SPT On"));
        }

  menuClearExit(500);
  }
}

//Function to disbled transmission
//by KD8CEC
void menuTxOnOff(int btn, byte optionType){
  if (!btn){
    printLineF(0,(isTxType & optionType) == 0?F("TX OFF?"):F("TX ON?"));
    }
  else {
      if ((isTxType & optionType) == 0){
        isTxType |= optionType;
        printLineF(0,F("TX OFF"));
        }
      else {
        isTxType &= ~(optionType);
        printLineF(0,F("TX ON"));
        }
  //Only Clear And Delay for Character LCD
  menuClearExit(500);
  }
}

//Toggle SDR MODE
void menuSDROnOff(int btn)
{
  if (!btn){
    if (conf.sdrModeOn == 0)
      printLineF(0,F("SDR Mode On?"));
    else
      printLineF(0,F("SDR Mode Off?"));
  }
  else {
      if (conf.sdrModeOn == 1){
        conf.sdrModeOn = 0;
        printLineF(0,F("SPK MODE"));
      }
      else {
        conf.sdrModeOn = 1;
        
        if (conf.ritOn == 1)
          conf.ritOn = 0;

        if (conf.splitOn == 1)
          conf.splitOn = 0;
          
        printLineF(0,F("SDR MODE"));
      }

    saveconf();
    setFrequency(conf.frequency);
    SetCarrierFreq();

  //Only Clear And Delay for Character LCD
  menuClearExit(500);
  }
}

void displayEmptyData(void){
  printLineF(0,F("Empty data"));
  delay_background(2000, 0);
}

//Builtin CW Keyer Logic by KD8CEC
void menuCWAutoKey(int btn){
    if (!btn){
      printLineF(0,F("Memory Keyer"));
      return;
      }
    
    printLineF(1,F("PTT to Send"));
    delay_background(500, 0);
    beforeCWTextIndex = 255;  //255 value is for start check
    isCWAutoMode = 1;
    updateDisplay(true,false);
    menuOn = 0;
}

//Standalone WSPR Beacone
void menuWSPRSend(int btn){
  if (!btn){
     printLineF(0,F("WSPR Beacon"));
     return;
    }
  if (conf.WsprMSGCount < 1)
    {
    displayEmptyData();
    return;
    }

  SendWSPRManage();
  menuClearExit(1000);
}

//Append by KD8CEC
void menuSetupCWCarrier(int btn){
  int knob = 0;
  unsigned long prevCarrier;
   
  if (!btn){
    printLineF(0,F("Set CW RX BFO"));
    return;
    }

  prevCarrier = conf.cwmCarrier;
  printLineF(1,F("PTT to confirm. "));
  delay_background(1000, 0);

  si5351bx_setfreq(0, conf.cwmCarrier);
  printCarrierFreq(conf.cwmCarrier);

  //disable all clock 1 and clock 2 
  while (digitalRead(PTT) == HIGH && !btnDown())
    {
    knob = enc_read();

    if (knob > 0)
      conf.cwmCarrier -= 5;
    else if (knob < 0)
      conf.cwmCarrier += 5;
    else
      continue; //don't update the frequency or the display
      
    si5351bx_setfreq(0, conf.cwmCarrier);
    printCarrierFreq(conf.cwmCarrier);
    delay_background(100, 0);
  }

  //save the setting
  if (digitalRead(PTT) == LOW){
    printLineF(0,F("Carrier set!"));
    saveconf();
    delay_background(1000, 0);
    }
  else 
    conf.cwmCarrier = prevCarrier;

  if (conf.cwMode == 0)
    si5351bx_setfreq(0, conf.usbCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  else
    si5351bx_setfreq(0, conf.cwmCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  setFrequency(conf.frequency);
  menuClearExit(0);
}

//=======================================================
//BEGIN OF STANDARD TUNE SETUP for reduce Program Memory
// by KD8CEC
//=======================================================
//valueType 0 : Normal
//          1 : CW Change -> Generate Tone
//          2 : IF Shift Setup -> SetFrequency, Set SideTone
//          5 : ATT
//          11 : Select Mode (different display type)
//knobSensitivity : 1 ~ 
int getValueByKnob(int valueType, int targetValue, int minKnobValue, int maxKnobValue, int incStep, const char* displayTitle, int knobSensitivity)
{
    int knob;
    int moveDetectStep = 0;
    char isInitDisplay = 1;
    delay_background(300, 0); //Default Delay

    if (valueType < 10)
      {
      strcpy(auxb, "Press, set ");
      strcat(auxb, displayTitle);
      printLine(1,auxb);
      }
    
    while(!btnDown())
      {
      knob = enc_read();
      if (knob != 0 || isInitDisplay == 1)
        {
        isInitDisplay = 0;
        moveDetectStep += (knob > 0 ? 1 : -1);
        if (moveDetectStep < (knobSensitivity * -1)) {
          if (targetValue > minKnobValue)
            targetValue -= incStep;
          moveDetectStep = 0;
          }
        else if (moveDetectStep > knobSensitivity) {
          if (targetValue < maxKnobValue)
            targetValue += incStep;
            
          moveDetectStep = 0;
          }

        strcpy(auxb, displayTitle);

        if (valueType == 11)   //Mode Select
          {
          auxb[targetValue * 4] = '>';
          }
        else
          {
          strcat(auxb, ":");
          itoa(targetValue,auxc, 10);
          strcat(auxb, auxc);
          }
        
        printLine(0,auxb);

        if (valueType == 1) //Generate Side Tone
          {
          tone(CW_TONE, targetValue);          
          }
        else if (valueType == 2 || valueType == 5 ) // 2:IFS, 5:ATT
          {
          if (valueType == 2)
            conf.ifShiftValue = targetValue;
          else 
            conf.attLevel = targetValue;

          setFrequency(conf.frequency);
          SetCarrierFreq();
        }
      }

      Check_Cat(0);  //To prevent disconnections
    }

    return targetValue;
}

void menuCWSpeed(int btn){
    int wpm;

    wpm = 1200/conf.cwSpeed;
     
    if (!btn){
     strcpy(auxb, "CW:");
     itoa(wpm,auxc, 10);
     strcat(auxb, auxc);
     strcat(auxb, "WPM Change?");
     printLine(0,auxb);
     return;
    }
    wpm = getValueByKnob(0, wpm, 3, 50, 1, "WPM", 3);
    /*
    while(!btnDown()){

      knob = enc_read();
      if (knob != 0){
        if (wpm > 3 && knob < 0) wpm--;
        if (wpm < 50 && knob > 0) wpm++;
        strcpy(b, "WPM:");
        itoa(wpm,c, 10);
        strcat(b, c);
        printLine(0,b);
      }

      Check_Cat(0);  //To prevent disconnections
    }
    */
  //save the setting
  conf.cwSpeed = 1200 / wpm;
  saveconf();
  //Only Clear And Delay for Character LCD
  menuClearExit(1000);
  
}

//Modified by KD8CEC
void menuSetupCwTone(int btn){
    //int prev_sideTone;
     
    if (!btn){
      printLineF(0,F("Change CW Tone"));
      return;
    }

    conf.sideTone = getValueByKnob(1, conf.sideTone, 100, 2000, 10, "Tone", 2); //1 : Generate Tone, targetValue, minKnobValue, maxKnobValue, stepSize
    noTone(CW_TONE);
    printLineF(0,F("Sidetone set!"));
    saveconf();

  delay_background(2000, 0);
  menuClearExit(0);
 }

//Modified by KD8CEC
void menuSetupCwDelay(int btn){
    //int knob = 0;
    int tmpCWDelay = conf.cwDelayTime * 10;
     
    if (!btn){
      printLineF(0,F("CW TX->RX Delay"));
     return;
    }

    tmpCWDelay = getValueByKnob(0, tmpCWDelay, 3, 2500, 10, "Delay", 2); //0 : Generate Tone, targetValue, minKnobValue, maxKnobValue, stepSize

    //save the setting
    conf.cwDelayTime = tmpCWDelay / 10;
    saveconf();
   //menuClearExit(1000);
  //Only Clear And Delay for Character LCD
  menuClearExit(1000);
}

//CW Time delay by KD8CEC
void menuSetupTXCWInterval(int btn){
    //char needDisplayInformation = 1;
    //int knob = 0;
    int tmpTXCWInterval = conf.delayBeforeCWStartTime * 2;
     
    if (!btn){
      printLineF(0,F("CW Start Delay"));
     return;
    }

    //printLineF(1,F("Press, set Delay"));
    //delay_background(300, 0);

    tmpTXCWInterval = getValueByKnob(0, tmpTXCWInterval, 0, 500, 2, "Delay", 2); //0 : Generate Tone, targetValue, minKnobValue, maxKnobValue, stepSize

   conf.delayBeforeCWStartTime = tmpTXCWInterval / 2;
   saveconf();
   //menuClearExit(1000);

  //Only Clear And Delay for Character LCD
  menuClearExit(1000);
   
}

//IF Shift function, BFO Change like RIT, by KD8CEC
void menuIFSSetup(int btn){
  //int knob = 0;
  //char needApplyChangeValue = 1;
  
  if (!btn){
    if (isIFShift == 1)
      printLineF(0,F("IF Shift Change?"));
    else
      printLineF(0,F("IF Shift On?"));
  }
  else 
  {
      isIFShift = 1;

      conf.ifShiftValue = getValueByKnob(2, conf.ifShiftValue, -20000, 20000, 50, "IFS", 2); 
      delay_background(500, 0); //for check Long Press function key
      
      if (btnDown() || conf.ifShiftValue == 0)
      {
        isIFShift = 0;
        conf.ifShiftValue = 0;
        setFrequency(conf.frequency);
        SetCarrierFreq();
      }

      //Store IF Shiift
      saveconf();
      //menuClearExit(0);
  //Only Clear And Delay for Character LCD
  menuClearExit(0);
  }
}

//ATT SETUP (IF1(45MHZ) SHIFT), by KD8CEC
void menuATTSetup(int btn){
  //int knob = 0;
  //char needApplyChangeValue = 1;
  
  if (!btn){
    if (conf.attLevel != 0)
      printLineF(0,F("ATT Change?"));
    else
      printLineF(0,F("ATT On?"));
  }
  else 
  {
      conf.attLevel = getValueByKnob(5, conf.attLevel, 0, 250, 5, "ATT", 2); //2 : (SetFrequency), targetValue, minKnobValue, maxKnobValue, stepSize
      delay_background(500, 0); //for check Long Press function key
      
      if (btnDown() || conf.attLevel == 0)
      {
        conf.attLevel = 0;
        setFrequency(conf.frequency);
        //SetCarrierFreq();
      }
      //menuClearExit(0);

    //Only Clear And Delay for Character LCD
    menuClearExit(0);
     
  }
}

//Functions for CWL and CWU by KD8CEC
void menuSelectMode(int btn){
  //int knob = 0;
  int selectModeType = 0;
  int beforeMode = 0;
  //int moveStep = 0;
  
  if (!btn){
      printLineF(0,F("Select Mode?"));
  }
  else 
  {
    //LSB, USB, CWL, CWU
    if (conf.cwMode == 0 && conf.isUSB == 0)
      selectModeType = 0;
    else if (conf.cwMode == 0 && conf.isUSB == 1)
      selectModeType = 1;
    else if (conf.cwMode == 1)
      selectModeType = 2;
    else
      selectModeType = 3;

    beforeMode = selectModeType;
    selectModeType = getValueByKnob(11, selectModeType, 0, 3, 1, " LSB USB CWL CWU", 4); //3 : Select Mode, targetValue, minKnobValue, maxKnobValue, stepSize

    if (beforeMode != selectModeType) 
    {
      //printLineF(1,F("Changed Mode"));
      if (selectModeType == 0) {
        conf.cwMode = 0; conf.isUSB = 0;
      }
      else if (selectModeType == 1) {
        conf.cwMode = 0; conf.isUSB = 1;
      }
      else if (selectModeType == 2) {
        conf.cwMode = 1;
      }
      else if (selectModeType == 3) {
        conf.cwMode = 2;
      }

      FrequencyToVFO(1);
    }

    SetCarrierFreq();
    setFrequency(conf.frequency);
    //menuClearExit(500);
  //Only Clear And Delay for Character LCD
  menuClearExit(500);
  }
}

//Select CW Key Type by KD8CEC
void menuSetupKeyType(int btn){
  //int knob = 0;
  int selectedKeyType = 0;
  //int moveStep = 0;
  if (!btn){
    printLineF(0,F("Change Key Type?"));
    }
  else {
    selectedKeyType = conf.cwKeyType;

    //selectedKeyType = getValueByKnob(12, selectedKeyType, 0, 2, 1, " KEY:", 5); //4 : Select Key Type, targetValue, minKnobValue, maxKnobValue, stepSize
    selectedKeyType = getValueByKnob(11, selectedKeyType, 0, 2, 1, " ST  IA  IB", 5); //4 : Select Key Type, targetValue, minKnobValue, maxKnobValue, stepSize

    printLineF(0,F("CW Key Type set!"));
    conf.cwKeyType = selectedKeyType;
    saveconf();

    if (conf.cwKeyType == 0)
      Iambic_Key = false;
    else
    {
      Iambic_Key = true;
      if (conf.cwKeyType == 1)
        keyerControl &= ~IAMBICB;
      else
        keyerControl |= IAMBICB;
    }
    
    //menuClearExit(1000);
  //Only Clear And Delay for Character LCD
  menuClearExit(1000);
    
  }
}

//=====================================================
//END OF STANDARD Set by Knob for reduce Program Memory
//=====================================================
 
//Lock Dial move by KD8CEC
void setDialLock(byte tmpLock, byte fromMode) {
  if (tmpLock == 1)
    isDialLock |= (conf.vfoActive == VFO_A ? 0x01 : 0x02);
  else
    isDialLock &= ~(conf.vfoActive == VFO_A ? 0x01 : 0x02);
    
  if (fromMode == 2 || fromMode == 3) return;

  //delay_background(1000, 0);
  printLine2ClearAndUpdate();
}

byte btnDownTimeCount;

#define PRESS_ADJUST_TUNE 20 //1000msec 20 * 50 = 1000milisec
#define PRESS_LOCK_CONTROL 40 //2000msec 40 * 50 = 2000milisec

//Modified by KD8CEC
void doMenu(){
  int select=0, i,btnState;
  char isNeedDisplay = 0;
  
  //for DialLock On/Off function
  btnDownTimeCount = 0;
  
  //wait for the button to be raised up

  //Appened Lines by KD8CEC for Adjust Tune step and Set Dial lock
  Serial2.println("while(btnDown()");
  while(btnDown()){
    Serial2.println("btnDown");
    delay_background(50, 0);
    
    if (btnDownTimeCount++ == (PRESS_ADJUST_TUNE)) { //Set Tune Step 
      printLineF(0,F("Set Tune Step?"));
    }
    else if (btnDownTimeCount > (PRESS_LOCK_CONTROL)) {  //check long time Down Button -> 2.5 Second => Lock
      if (conf.vfoActive == VFO_A)
        setDialLock((isDialLock & 0x01) == 0x01 ? 0 : 1, 0); //Reverse Dial lock
      else
        setDialLock((isDialLock & 0x02) == 0x02 ? 0 : 1, 0); //Reverse Dial lock
      return;
    }
  }
  delay(50);  //debounce

  //ADJUST TUNE STEP 
  if (btnDownTimeCount > PRESS_ADJUST_TUNE)
  {
    printLineF(1,F("Press to set"));
    isNeedDisplay = 1; //check to need display for display current value
    
    while (!btnDown())
    {
      delay_background(50, 0);
      if (isNeedDisplay) {
        strcpy(auxb, "Tune Step:");
        itoa(conf.arTuneStep[conf.tuneStepIndex -1], auxc, 10);
        strcat(auxb, auxc);
        printLine(0,auxb);
        isNeedDisplay = 0;
      }
        
      i = enc_read();

      if (i != 0) {
        select += (i > 0 ? 1 : -1);

        if (select * select >= 25) {  //Threshold 5 * 5 = 25
          if (select < 0) {
            if (conf.tuneStepIndex > 1)
              conf.tuneStepIndex--;
          }
          else {
            if (conf.tuneStepIndex < 9)
              conf.tuneStepIndex++;
          }
          select = 0;
          isNeedDisplay = 1;
        }
      }
    } //end of while

    saveconf();
    delay_background(500, 0);
    printLine2ClearAndUpdate();
    return;
  }   //set tune step

  //Below codes are origial code with modified by KD8CEC
  menuOn = 2;
  
  //*********************************************************************************
  // New type menu for developer by KD8CEC
  // Selectable menu
  // Version : 1.097 ~
  //*********************************************************************************

  #define FN_DEFAULT_MENU     2 //Setup Onff / Exit
  #define FN_DEFAULT_SETUP    1 //Exit

  #define FN_BAND_IDX         (1 -1)        //0 or -1
  #define FN_VFO_TOGGLE_IDX   (FN_BAND_IDX        + 1)
  #define FN_MODE_IDX         (FN_VFO_TOGGLE_IDX  + 1)
  #define FN_RIT_IDX          (FN_MODE_IDX        + 1)
  #define FN_IFSHIFT_IDX      (FN_RIT_IDX         + 1)
  #define FN_ATT_IDX          (FN_IFSHIFT_IDX     + 1)
  #define FN_CW_SPEED_IDX     (FN_ATT_IDX         + 1)
  #define FN_SPLIT_IDX        (FN_CW_SPEED_IDX    + 1)
  #define FN_VFOTOMEM_IDX     (FN_SPLIT_IDX       + 1)
  #define FN_MEMTOVFO_IDX     (FN_VFOTOMEM_IDX    + 1)
  #define FN_MEMORYKEYER_IDX  (FN_MEMTOVFO_IDX    + 1)
  #define FN_WSPR_IDX         (FN_MEMORYKEYER_IDX + 1)
  #define FN_SDRMODE_IDX      (FN_WSPR_IDX        + 1)
  #define FN_SETUP_IDX        (FN_SDRMODE_IDX     + 1)
  #define FN_EXIT_IDX         (FN_SETUP_IDX       + 1)
  #define FN_CALIBRATION_IDX  (FN_EXIT_IDX        + 1)
  #define FN_CARRIER_IDX      (FN_CALIBRATION_IDX + 1)
  #define FN_CWCARRIER_IDX    (FN_CARRIER_IDX     + 1)
  #define FN_CWTONE_IDX       (FN_CWCARRIER_IDX   + 1)
  #define FN_CWDELAY_IDX      (FN_CWTONE_IDX      + 1)
  #define FN_TXCWDELAY_IDX    (FN_CWDELAY_IDX     + 1)
  #define FN_KEYTYPE_IDX      (FN_TXCWDELAY_IDX   + 1)
  #define FN_ADCMONITOR_IDX   (FN_KEYTYPE_IDX)
  #define FN_TXONOFF_IDX      (FN_ADCMONITOR_IDX  + 1)

  #define FN_MENU_COUNT       (FN_DEFAULT_MENU + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1)
  #define FN_SETUP_COUNT      (FN_DEFAULT_SETUP + 1 + 1 + 1 + 1 + 1 + 1 + 1 + 1)
  #define FN_STEP_COUNT       (FN_MENU_COUNT + 1)
    
  while (menuOn){
    i = enc_read();
    btnState = btnDown();

    if (i > 0){
      if (conf.modeCalibrate && select + i < FN_STEP_COUNT * 10)
        select += i;
      else if (!conf.modeCalibrate && select + i < FN_MENU_COUNT * 10)
        select += i;
    }
    else if (i < 0 && select - i >= -10)
      select += i;
      
    switch (select / 10)
    {
      case FN_BAND_IDX : 
        menuBand(btnState); 
        break;
      case FN_VFO_TOGGLE_IDX : 
        menuVfoToggle(btnState); 
        break;
      case FN_MODE_IDX : 
        menuSelectMode(btnState); 
        break;
      case FN_RIT_IDX : 
        menuRitToggle(btnState); 
        break;
      case FN_IFSHIFT_IDX : 
        menuIFSSetup(btnState); 
        break;
      case FN_ATT_IDX : 
        menuATTSetup(btnState); 
        break;
      case FN_CW_SPEED_IDX : 
        menuCWSpeed(btnState); 
        break;
      case FN_SPLIT_IDX : 
        menuSplitOnOff(btnState);        //SplitOn / off
        break;
      case FN_VFOTOMEM_IDX : 
        menuCHMemory(btnState, 0);       //VFO to Memroy
        break;
      case FN_MEMTOVFO_IDX : 
        menuCHMemory(btnState, 1);       //Memory to VFO
        break;
      case FN_MEMORYKEYER_IDX : 
        menuCWAutoKey(btnState);  
        break;
      case FN_WSPR_IDX : 
        menuWSPRSend(btnState);
        break;
      case FN_SDRMODE_IDX : 
        menuSDROnOff(btnState);
        break;
      case FN_SETUP_IDX : 
        menuSetup(btnState);
        break;
      case FN_EXIT_IDX : 
        menuExit(btnState);
        break;
      case FN_CALIBRATION_IDX : 
        menuSetupCalibration(btnState);  //crystal
        break;
      case FN_CARRIER_IDX : 
        menuSetupCarrier(btnState);      //ssb
        break;
      case FN_CWCARRIER_IDX : 
        menuSetupCWCarrier(btnState);    //cw
        break;
      case FN_CWTONE_IDX : 
        menuSetupCwTone(btnState);  
        break;
      case FN_CWDELAY_IDX : 
        menuSetupCwDelay(btnState);  
        break;
      case FN_TXCWDELAY_IDX : 
        menuSetupTXCWInterval(btnState);  
        break;
      case FN_KEYTYPE_IDX :
        menuSetupKeyType(btnState);  
        break;
      case FN_TXONOFF_IDX :
        menuTxOnOff(btnState, 0x01);       //TX OFF / ON
        break;
      default :
        menuExit(btnState);  break;
    } //end of switch
    Check_Cat(0);  //To prevent disconnections
  }   //end of while

}

//*************************************************************************************
//Original Source Part
//The code below is the original source part that I kept unchanged for compatibility.
//By KD8CEC
//*************************************************************************************

/** 
 Original source comment 
 *  Menus
 *  The Radio menus are accessed by tapping on the function button. 
 *  - The main loop() constantly looks for a button press and calls doMenu() when it detects
 *  a function button press. 
 *  - As the encoder is rotated, at every 10th pulse, the next or the previous menu
 *  item is displayed. Each menu item is controlled by it's own function.
 *  - Eache menu function may be called to display itself
 *  - Each of these menu routines is called with a button parameter. 
 *  - The btn flag denotes if the menu itme was clicked on or not.
 *  - If the menu item is clicked on, then it is selected,
 *  - If the menu item is NOT clicked on, then the menu's prompt is to be displayed
 */

/**
 * The calibration routines are not normally shown in the menu as they are rarely used
 * They can be enabled by choosing this menu option
 */
void menuSetup(int btn){
  if (!btn)
  {
    if (!conf.modeCalibrate)
      printLineF(0,F("Setup On?"));
    else
      printLineF(0,F("Setup Off?"));
  }
  else 
  {
    conf.modeCalibrate = !conf.modeCalibrate;
    //menuClearExit(1000);

  //Only Clear And Delay for Character LCD
  menuClearExit(1000);
  }
}

void menuExit(int btn){
  if (!btn){
      printLineF(0,F("Exit Menu?"));
  }
  else
   menuClearExit(0);
}

//modified for reduce used flash memory by KD8CEC
void menuRitToggle(int btn){
  if (!btn){
    printLineF(0,conf.ritOn == 1?F("RIT Off?"):F("RIT On?"));
    }
  else {
    if (conf.ritOn == 0) ritEnable(conf.frequency); else  ritDisable();
      //menuClearExit(500);
    //Only Clear And Delay for Character LCD
    menuClearExit(500);
    }
}

void setupBFO(){
  int knob = 0;
  unsigned long prevCarrier;
  prevCarrier = conf.usbCarrier;
  tft.drawString("Set BFO",0,130);
  tft.drawString("Press TUNE to Save",0,170);
  si5351bx_setfreq(0, conf.usbCarrier);
  printCarrierFreq(conf.usbCarrier);
  displayYN();
  while (!btnDown()){
    tft.fillRect(210, 80, 160, 20, TFT_BLACK);
    tft.drawNumber(conf.usbCarrier,210,80);
    knob = enc_read();
    if (knob != 0)
      conf.usbCarrier += 50 * knob;
    else
      continue; //don't update the frequency or the display
    si5351bx_setfreq(0, conf.usbCarrier);
    setFrequency(conf.frequency);
    delay(100);
  }
  saveconf();
  si5351bx_setfreq(0, conf.usbCarrier);          
  setFrequency(conf.frequency);    
  menuOn = 0; 
}

void setupFreq(){
  Serial2.println("setupFreq");
  int knob = 0;
  int32_t prev_calibration;
  //round off the the nearest khz
  conf.frequency = (conf.frequency/1000l)* 1000l;
  setFrequency(conf.frequency);
  while (btnDown()) delay(100);
  delay(100);
  tft.drawNumber(conf.calibration,210,50);
  prev_calibration = conf.calibration;
  tft.drawString("You should have a signal",0,130);
  tft.drawString("exactly at      Khz",0,150);
  tft.drawNumber(conf.frequency/1000l,135,150);
  tft.drawString("Rotate to zerobeat",0,170);
  displayYN();
  while (!btnDown())
    {
    tft.fillRect(210, 50, 160, 20, TFT_BLACK);
    tft.drawNumber(conf.calibration,210,50);
    knob = enc_read();
    if (knob != 0)
      conf.calibration += knob * 875;
    else  
      continue; //don't update the frequency or the display
    si5351bx_setfreq(0, conf.usbCarrier);  //set back the cardrier oscillator anyway, cw tx switches it off  
    si5351_set_calibration(conf.calibration);
    setFrequency(conf.frequency);
    }
  saveconf();
  initOscillators();
  si5351_set_calibration(conf.calibration);
  setFrequency(conf.frequency);    
  //debounce and delay
  while(btnDown()) delay(50); delay(100);
}

/**
 * Take a deep breath, math(ematics) ahead
 * The 25 mhz oscillator is multiplied by 35 to run the vco at 875 mhz
 * This is divided by a number to generate different frequencies.
 * If we divide it by 875, we will get 1 mhz signal
 * So, if the vco is shifted up by 875 hz, the generated frequency of 1 mhz is shifted by 1 hz (875/875)
 * At 12 Mhz, the carrier will needed to be shifted down by 12 hz for every 875 hz of shift up of the vco
 * 
 */

 //this is used by the si5351 routines in the ubitx_5351 file
//extern int32_t calibration;
extern uint32_t si5351bx_vcoa;

void factoryCalibration(int btn){
  Serial2.println("factoryCalibration");
  int knob = 0;
  //keep clear of any previous button press
  while (btnDown()) delay(100);
  delay(100);
  if (!btn){
    printLineF(6,F("Set Calibration?"));
    return;
  }
  conf.calibration = 0;
  conf.cwMode = 0;
  conf.isUSB = 1;

  //turn off the second local oscillator and the bfo
  si5351_set_calibration(conf.calibration);
  startTx(TX_CW, 1);
  si5351bx_setfreq(2, 10000000l); 
  
  strcpy(auxb, "#1 10 MHz cal:");
  ltoa(conf.calibration/8750, auxc, 10);
  strcat(auxb, auxc);
  Serial2.print("auxb:");Serial2.println(auxb);
  tft.drawNumber(conf.calibration,90,110);
  printLine(7,auxb);     

  while (!btnDown())
    {
    if (digitalRead(PTT) == LOW && !keyDown) cwKeydown();
    if (digitalRead(PTT)  == HIGH && keyDown) cwKeyUp();
    knob = enc_read();
    Serial2.print("calibration:");Serial2.println(conf.calibration);
    if (knob > 0) conf.calibration += 875;
    else if (knob < 0) conf.calibration -= 875;
    else 
      continue; //don't update the frequency or the display
      
    si5351_set_calibration(conf.calibration);
    si5351bx_setfreq(2, 10000000l);
    strcpy(auxb, "#1 10 MHz cal:");
    ltoa(conf.calibration/8750, auxc, 10);
    strcat(auxb, auxc);
    printLine(7,auxb);     
    }

  conf.cwTimeout = 0;
  keyDown = 0;
  stopTx();

  printLineF(0,F("Calibration set!"));
  saveconf();
  initOscillators();
  setFrequency(conf.frequency);
  updateDisplay(true,false);

  while(btnDown())
    delay(50);

  menuClearExit(100);
}

void menuSetupCalibration(int btn){
  int knob = 0;
  int32_t prev_calibration;
   
  if (!btn){
    printLineF(0,F("Set Calibration?"));
    return;
  }

  printLineF(1,F("Set to Zero-beat,"));
  printLineF(0,F("press PTT to save"));
  delay_background(1000, 0);
  
  prev_calibration = conf.calibration;
  conf.calibration = 0;
  si5351_set_calibration(conf.calibration);
  setFrequency(conf.frequency);    
  
  strcpy(auxb, "cal:");
  ltoa(conf.calibration/8750, auxc, 10);
  strcat(auxb, auxc);
  printLine(0,auxb);     

  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0){
      conf.calibration += 8750;
      conf.usbCarrier += 120;
    }
    else if (knob < 0){
      conf.calibration -= 8750;
      conf.usbCarrier -= 120;
    }
    else
      continue; //don't update the frequency or the display

    si5351_set_calibration(conf.calibration);
    si5351bx_setfreq(0, conf.usbCarrier);
    setFrequency(conf.frequency);    

    strcpy(auxb, "cal:");
    ltoa(conf.calibration/8750, auxc, 10);
    strcat(auxb, auxc);
    printLine(0,auxb);     
  }
  
  //save the setting
  if (digitalRead(PTT) == LOW){
    printLineF(1,F("Calibration set!"));
    printLineF(0,F("Set Carrier now"));
    saveconf();
    delay_background(2000, 0);
  }
  else
    conf.calibration = prev_calibration;

  initOscillators();
  //si5351_set_calibration(calibration);
  setFrequency(conf.frequency);    
  //printLine2ClearAndUpdate();
  //menuOn = 0;
  menuClearExit(0);
}

void printCarrierFreq(unsigned long freq){

  memset(auxc, 0, sizeof(auxc));
  memset(auxb, 0, sizeof(auxb));

  ultoa(freq, auxb, DEC);
  
  strncat(auxc, auxb, 2);
  strcat(auxc, ".");
  strncat(auxc, &auxb[2], 3);
  strcat(auxc, ".");
  strncat(auxc, &auxb[5], 3);
  printLine(0,auxc);    
}

//modified by KD8CEC (just 1 line remarked //usbCarrier = ...
void menuSetupCarrier(int btn){
  int knob = 0;
  unsigned long prevCarrier;
   
  if (!btn){
      printLineF(0,F("Set the BFO"));
    return;
  }

  prevCarrier = conf.usbCarrier;
  printLineF(1,F("Tune to best Signal"));  
  printLineF(1,F("PTT to confirm. "));
  delay_background(1000, 0);

  //usbCarrier = 11995000l; //Remarked by KD8CEC, Suggest from many user, if entry routine factoryrest
  /*
  //for uBITX V5.0, but not used by KD8CEC, if you want default value of carrier on Calibration, delete remark symbols
  usbCarrier = 11053000l;
   */

  
  si5351bx_setfreq(0, conf.usbCarrier);
  printCarrierFreq(conf.usbCarrier);

  //disable all clock 1 and clock 2 
  while (digitalRead(PTT) == HIGH && !btnDown())
  {
    knob = enc_read();

    if (knob > 0)
      conf.usbCarrier -= 5;
    else if (knob < 0)
      conf.usbCarrier += 5;
    else
      continue; //don't update the frequency or the display
      
    si5351bx_setfreq(0, conf.usbCarrier);
    printCarrierFreq(conf.usbCarrier);

    Check_Cat(0);  //To prevent disconnections
    delay(100);
  }

  //save the setting
  if (digitalRead(PTT) == LOW){
    printLineF(0,F("Carrier set!"));
    saveconf();
    delay_background(1000, 0);
  }
  else 
    conf.usbCarrier = prevCarrier;

  //si5351bx_setfreq(0, usbCarrier);          
  if (conf.cwMode == 0)
    si5351bx_setfreq(0, conf.usbCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  else
    si5351bx_setfreq(0, conf.cwmCarrier);  //set back the carrier oscillator anyway, cw tx switches it off
  
  setFrequency(conf.frequency);    
  //printLine2ClearAndUpdate();
  //menuOn = 0; 
  menuClearExit(0);
}
