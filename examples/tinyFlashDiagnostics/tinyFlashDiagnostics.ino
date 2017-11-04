/*
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                           tinyFlashDiagnostics.ino                                                            |
  |                                                               SPIFlash library                                                                |
  |                                                                   v 2.6.0                                                                     |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                    Marzogh                                                                    |
  |                                                                  16.04.2017                                                                   |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
  |                                                                                                                                               |
  |                                  For a full diagnostics rundown - with error codes and details of the errors                                  |
  |                                uncomment #define RUNDIAGNOSTIC in SPIFlash.cpp in the library before compiling                                |
  |                                             and loading this application onto your Arduino.                                                   |
  |                                                                                                                                               |
  |~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~|
*/

#include "vars.h"

#include<SPIFlash.h>

//Define a flash memory size (if using non-Winbond memory) according to the list in defines.h
//#define CHIPSIZE MB64

  //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!//
  //                                                                                            //
  //                       ___ _____ _____ _             _____  _____                           //
  //                      / _ \_   _|_   _(_)           |  _  ||  ___|                          //
  //                     / /_\ \| |   | |  _ _ __  _   _ \ V / |___ \                           //
  //                     |  _  || |   | | | | '_ \| | | |/ _ \     \ \                          //
  //                     | | | || |   | | | | | | | |_| | |_| |/\__/ /                          //
  //                     \_| |_/\_/   \_/ |_|_| |_|\__, \_____/\____/                           //
  //                                                __/ |                                       //
  //                                               |___/                                        //
  //                                                                                            //
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~//
  //                       Uncomment any of the tests below as required.                        //
  //        Remember, the ATTiny has a limited amount of SRAM, so only a limited number         //
  //                             of tests can be run concurrently.                              //
  //                                                                                            //
  //|------------------------------------------//----------------------------------------------|//
  //|                     List of tests        //                 _status bits                 |//
  //|------------------------------------------//----------------------------------------------|//
  #define INTTEST                            //                     iW, iR                   |//
  #define FLOATTEST                          //                     fW, fR                   |//
  //#define STRINGTEST                         //                    sgW, sgR                  |//
  #define STRUCTTEST                         //                    scW, scR                  |//
  //#define ARRAYTEST                          //                     aW, aR                   |//
  #define ERASETEST                          //                     eB, eC                   |//
  #define POWERTEST                          //                    pON, pOFF                 |//
  //|------------------------------------------//----------------------------------------------|//

SPIFlash flash;

void setup() {
  startup();
  diagnose();
}

void loop() {

}

//************************************************************************************************//
//                                                                                                //
//                                    Non-board specific code                                     //
//                                                                                                //
//************************************************************************************************//

bool prevWritten() {
  addr = flash.getAddress(sizeof(dataPacket));
  flash.readAnything(addr, dataPacket);
  if (dataPacket.writeStatus != 0xFF) {
    return true;
  }
  else {
    return false;
  }
}
//************************************************************************************************//
