/*************************************************************************
  header file for C++ by KD8CEC
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
#ifndef _UBITX_HEADER__
#define _UBITX_HEADER__

//==============================================================================
// Compile Option
//==============================================================================

//Select betwen Analog S-Meter and DSP (I2C) Meter
#define USE_I2CSMETER

//#define ENABLE_FACTORYALIGN
#define SMeterLatency   3               //1 is 0.25 sec

//==============================================================================
// Hardware, Define PIN Usage
//==============================================================================
/**
 * We need to carefully pick assignment of pin for various purposes.
 * There are two sets of completely programmable pins on the Raduino.
 * First, on the top of the board, in line with the LCD connector is an 8-pin connector
 * that is largely meant for analog inputs and front-panel control. It has a regulated 5v output,
 * ground and six pins. Each of these six pins can be individually programmed 
 * either as an analog input, a digital input or a digital output. 
 * The pins are assigned as follows (left to right, display facing you): 
 *      Pin 1 (Violet), A7, SPARE => Analog S-Meter
 *      Pin 2 (Blue),   A6, KEYER (DATA)
 *      Pin 3 (Green), +5v 
 *      Pin 4 (Yellow), Gnd
 *      Pin 5 (Orange), A3, PTT
 *      Pin 6 (Red),    A2, F BUTTON
 *      Pin 7 (Brown),  A1, ENC B
 *      Pin 8 (Black),  A0, ENC A
 *Note: A5, A4 are wired to the Si5351 as I2C interface 
 *       *     
 * Though, this can be assigned anyway, for this application of the Arduino, we will make the following
 * assignment
 * A2 will connect to the PTT line, which is the usually a part of the mic connector
 * A3 is connected to a push button that can momentarily ground this line. This will be used for RIT/Bandswitching, etc.
 * A6 is to implement a keyer, it is reserved and not yet implemented
 * A7 is connected to a center pin of good quality 100K or 10K linear potentiometer with the two other ends connected to
 * ground and +5v lines available on the connector. This implments the tuning mechanism
 */
// for ESP32
#define A0 34
#define A1 35
#define A2 36
#define A3 39

//#define A6 39
//#define A7 39

#define ENC_A         (A0)
#define ENC_B         (A1)
#define FBUTTON       (A2)
#define PTT           (A3)
#define ANALOG_KEYER  (4)
//#define ANALOG_KEYER  (A6)
#define ANALOG_SMETER (A7)  //by KD8CEC 
#define RXD2 22             // UART2   //EA4GZI
#define TXD2 32             // UART2   //EA4GZI

/** 
 *  The second set of 16 pins on the Raduino's bottom connector are have the three clock outputs and the digital lines to control the rig.
 *  This assignment is as follows :
 *    Pin   1   2    3    4    5    6    7    8    9    10   11   12   13   14   15   16
 *         GND +5V CLK2  GND  GND  CLK1 GND  GND  CLK0  GND  D2   D3   D4   D5   D6   D7  
 *  These too are flexible with what you may do with them, for the Raduino, we use them to :
 *  - TX_RX line : Switches between Transmit and Receive after sensing the PTT or the morse keyer
 *  - CW_KEY line : turns on the carrier for CW
 */
#define TX_RX         (23)   // Relay
#define CW_TONE       (16) 
#define TX_LPF_A      (25)   // Relay, azul
#define TX_LPF_B      (26)   // Relay, verde
#define TX_LPF_C      (27)   // Relay, marrón
#define CW_KEY        (2)

//******************************************************
//DSP (I2C) Meter 
//******************************************************
//S-Meter Address
#define I2CMETER_ADDR     0x58
//VALUE TYPE============================================
//Signal
#define I2CMETER_UNCALCS  0x58 //Uncalculated Signal Meter

//==============================================================================
// for public, Variable, functions
//==============================================================================
//Ham Band
//0x00 : None, 0x01 : MODE, 0x02:BAND+, 0x03:BAND-, 0x04:TUNE_STEP, 0x05:VFO Toggle, 0x06:SplitOn/Off, 0x07:TX/ON-OFF,  0x08:SDR Mode On / Off, 0x09:Rit Toggle
#define FKEY_PRESS      0x78

extern char auxc[30];
extern char auxb[30];
extern void clearTFT(void);

#endif    //end of if header define

