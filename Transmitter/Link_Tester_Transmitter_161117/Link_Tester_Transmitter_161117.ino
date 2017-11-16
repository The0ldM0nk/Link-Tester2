//Link_Tester_Transmitter_171117.ino


#define programname "Link_Tester_Transmitter_171117"
#define programversion "V2.2"
#define dateproduced "17/11/17"
#define aurthorname "Stuart Robinson"

#include <Arduino.h>
      
/*
**************************************************************************************************
  
  LoRaTracker Programs for Arduino

  Copyright of the author Stuart Robinson - 17/11/17

  http://www.LoRaTracker.uk

  These programs may be used free of charge for personal, recreational and educational purposes only.

  This program, or parts of it, may not be used for or in connection with any commercial purpose without
  the explicit permission of the author Stuart Robinson.

  The programs are supplied as is, it is up to individual to decide if the programs are suitable for the
  intended purpose and free from errors.

  To do;

 
******************************************************************************************************
*/


#include "Pin_Definitions.h"                   // this file has the pin definitions used to connec tot the LoRa device


/*
******************************************************************************************************
  Definitions for packet types used in this program
******************************************************************************************************
*/

const char ACK = 'A';                   //Acknowledge 
const char ClearToSendCommand = 'c';    //clear to send a packet
const char LocationCSVPacket = 'S';     //Short loacation packet in CSV ASCII format
const char LocationBinaryPacket = 's';  //Short location packet in binary format
const char Testpacket = 'T';            //Test packet
const char LMLCSVPacket = '8';          //short LML payload; lat,lon,alt in CSV ASCII format
const char TestMode1 = '1';             //used to switch to Testmode1 settings
const char TestMode2 = '2';             //used to switch to Testmode2 settings
const char Broadcast = '*';             //Broadcast address
const char HABPacket = '$';             //HAB Style location payload in CSV ASCII format


//Global Program Variables

unsigned int seq = 0;
unsigned int testloop = 0;
byte keypress;
byte modenumber;
byte ramv_ThisNode;
byte lora_TestPower = 10;                //is also start up tone power

#include <SPI.h>
#include "Test_Settings.h" 
#include "LoRa_Test.h"
#include "Binary_Test.h"


void loop()
{

  testloop++;
  
  Serial.print(F("Test Loop "));
  Serial.println(testloop);
  Serial.println();
  Serial.flush();

  Serial.println(F("Loop Start Tone "));
  Serial.flush();
 
  /*
  *********************************************************
  Start Mode 1 routines
  *********************************************************
  */

#ifdef EnableMode1

  lora_Tone2(500, LoopStartTone_lengthmS, start_power);       //Transmit a pseudo FM tone
  delay(250);
  
  init_TXLoRaTest1();                                         //setup for test mode 1
  
  #ifndef EnableMode2
  Send_Test1Mode_Packet();                              
  delay(mode_delaymS);
  #endif
  
  Serial.println("Start Mode1 Packets");
  Serial.println();
  for (lora_TestPower = start_power; lora_TestPower >= end_power; lora_TestPower--)
  {
    Serial.print(lora_TestPower);
    Serial.println("dBm  Packets");

#ifdef SendTestpacket
    lora_Custom1();
    Serial.print("Send Test Packet ");
    Serial.flush();
    Send_Test_Packet('1');
    Serial.println();
    delay(packet_delay);
#endif

#ifdef SendLocationCSVPacket
    lora_Custom1();
    Serial.print("Send Location CSV Packet ");
    Serial.flush();
    Send_LocationCSV_Packet();
    Serial.println();
    delay(packet_delay);
#endif

#ifdef SendBinaryLocation
    lora_Custom1();
    Serial.print("Send Binary Location Packet ");
    Serial.flush();
    Send_Binary_Location_Packet();
    Serial.println();
    delay(packet_delay);
#endif


#ifdef SendHABPacket
    lora_Custom1();
    Serial.print("Send HAB Packet ");
    Serial.flush();
    Send_HAB_Packet();
    Serial.println();
    delay(packet_delay);
#endif
    Serial.println();
  }

  Serial.println("Finished Mode1 Packets");
  
  #ifdef EnableMode2
  Send_Test2Mode_Packet();                              //switch to mode2
  #endif
  Serial.println();
  delay(mode_delaymS);

#endif

  /********************************************************
    End Mode 1 routines
  *********************************************************
  */


  /********************************************************
    Start Mode 2 routines
  *********************************************************
  */

#ifdef EnableMode2

  lora_Tone2(500, LoopStartTone_lengthmS, start_power);       //Transmit a pseudo FM tone
  delay(250);
  lora_Tone2(500, LoopStartTone_lengthmS, start_power);       //Transmit a pseudo FM tone
  delay(250);
  
  init_TXLoRaTest2();                                         //setup for test mode 2        

  Serial.println("Start Mode2 Packets");
  Serial.println();
  for (lora_TestPower = start_power; lora_TestPower >= end_power; lora_TestPower--)
  {
    Serial.print(lora_TestPower);
    Serial.println("dBm  Packets");

#ifdef SendTestpacket
    lora_Custom2();
    Serial.print("Send Test Packet ");
    Serial.flush();
    Send_Test_Packet('2');
    Serial.println();
    delay(packet_delay);
#endif


#ifdef SendLocationCSVPacket
    lora_Custom2();
    Serial.print("Send Location CSV Packet ");
    Serial.flush();
    Send_LocationCSV_Packet();
    Serial.println();
    delay(packet_delay);
#endif

#ifdef SendBinaryLocation
    lora_Custom2();
    Serial.print("Send Binary Location Packet ");
    Serial.flush();
    Send_Binary_Location_Packet();
    Serial.println();
    delay(packet_delay);
#endif

#ifdef SendHABPacket
    lora_Custom2();
    Serial.print("Send HAB Packet ");
    Serial.flush();
    Send_HAB_Packet();
    Serial.println();
    delay(packet_delay);
#endif

    
    Serial.println();

  }
  
  Serial.println("Finished Mode2 Packets");
  
  #ifdef EnableMode1
  Send_Test1Mode_Packet();                              //switch to mode1
  #endif
  Serial.println();
  delay(mode_delaymS);

#endif

  /********************************************************
    End Mode 2 routines
  *********************************************************
  */
}


void Send_HAB_Packet()
{
  byte Count;
  Count = buildHABPayload();
  lora_TXEnd = Count;
  PrintPacket();
  Serial.print(F(" "));
  lora_Send(0, Count, HABPacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);   //send the packet, data is in TXbuff from lora_TXStart to lora_TXEnd
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");

}


byte buildHABPayload()
{
  //build the long tracker payload
  unsigned int index, index2, hours, mins, seconds, alt, sats, volts, internaltemperature, resets;
  unsigned int ramv_Current_config, TRStatus, runmAhr, GPSfixtime;
  unsigned int CRC;
  byte Count, len;
  byte max_length = 128;
  float calibration;
  char LatArray[10], LonArray[10], CalibrationArray[10];
  char node[2];
  seq++;
  hours = 10;
  mins = 10;
  seconds = 10;
  dtostrf(TestLatitude, 7, 5, LatArray);
  dtostrf(TestLongitude, 7, 5, LonArray);
  alt = TestAltitude;
  sats = 5;
  volts = 3999;
  internaltemperature = -10;
  resets = 11;
  ramv_Current_config = 0x02;
  TRStatus = 0x81;
  runmAhr = 45;
  calibration = 2000;
  dtostrf(calibration, 3, 1, CalibrationArray);
  GPSfixtime = 4.5;

  len = sizeof(lora_TXBUFF);
  memset(lora_TXBUFF, 0, len);                                 //clear array to 0s

  node[0] = ramv_ThisNode;
  node[1] = 0;

  Count = snprintf((char*) lora_TXBUFF,
           max_length,
           "$$$$%s%s,%d,%02d:%02d:%02d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%s,%d",
           Flight_ID,
           node,
           seq,
           hours,
           mins,
           seconds,
           LatArray,
           LonArray,
           alt,
           sats,
           volts,
           internaltemperature,
           resets,
           ramv_Current_config,
           TRStatus,
           runmAhr,
           CalibrationArray,
           GPSfixtime
          );

  CRC = 0xffff;                                               //start value for CRC16

  for (index = 4; index < Count; index++)                     //element 4 is first character after $$$$ at start
  {
    CRC ^= (((unsigned int)lora_TXBUFF[index]) << 8);
    for (index2 = 0; index2 < 8; index2++)
    {
      if (CRC & 0x8000)
        CRC = (CRC << 1) ^ 0x1021;
      else
        CRC <<= 1;
    }
  }

  lora_TXBUFF[Count++] = '*';
  lora_TXBUFF[Count++] = Hex((CRC >> 12) & 15);                //add the checksum bytes to the end
  lora_TXBUFF[Count++] = Hex((CRC >> 8) & 15);
  lora_TXBUFF[Count++] = Hex((CRC >> 4) & 15);
  lora_TXBUFF[Count] = Hex(CRC & 15);
  return Count;
}


byte Build_LocationCSV_Packet()
{
  //build short payload in buffer
  byte Count, len;
  byte max_length = 64;
  char LatArray[10], LonArray[10];
  unsigned int alt;
  
  dtostrf(TestLatitude, 7, 5, LatArray);
  dtostrf(TestLongitude, 7, 5, LonArray);
  alt = TestAltitude;
  
  len = sizeof(lora_TXBUFF);
  memset(lora_TXBUFF, 0, len);                                 //clear array to 0s
  
  Count = snprintf((char*) lora_TXBUFF,
           max_length,
           "%s,%s,%d,%d,",
           LatArray,
           LonArray,
           alt,
           lora_TestPower
          );

  return Count;
}


char Hex(byte lchar)
{
  //used in CRC calculation in buildHABPacket
  char Table[] = "0123456789ABCDEF";
  return Table[lchar];
}


void PrintPacket()
{
  byte index;
  for (index = 0; index <= lora_TXEnd; index++)
  {
    Serial.write(lora_TXBUFF[index]);
  }
}


void Send_Test1Mode_Packet()
{
  //causes RX to switch mode
  Serial.println(F("Send_Test1Mode_Packet()"));
  lora_TXBUFF[0] = '1';
  lora_Send(0, 0, TestMode1, Broadcast, ThisNode, 10, 17, NoStrip);
  delay(1000);                                                       //leave time for receiver to print running totals
}


void Send_Test2Mode_Packet()
{
  //causes RX to switch mode
  Serial.println(F("Send_Test2Mode_Packet()"));
  lora_TXBUFF[0] = '2';
  lora_Send(0, 0, TestMode2, Broadcast, ThisNode, 10, 17, NoStrip);
  delay(1000);                                                      //leave time for receiver to print running totals
}


void Send_LocationCSV_Packet()
{
  //now build and send Short payload
  byte Count;
  Count = Build_LocationCSV_Packet();
  lora_TXEnd = Count;
  PrintPacket();
  lora_Send(0, lora_TXEnd, LocationCSVPacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");
}


void Send_Binary_Location_Packet()
{
  Serial.print(F(" "));
  Serial.print(TestLatitude, 6);
  Serial.print(F(" "));
  Serial.print(TestLongitude, 6);
  Serial.print(F(" "));
  Serial.print(TestAltitude);
  Serial.print(F(" "));
  Serial.print(config_byte);
  Serial.print(F(" "));
  Write_Float(0, TestLatitude, lora_TXBUFF);
  Write_Float(4, TestLongitude, lora_TXBUFF);
  Write_UInt(8, TestAltitude, lora_TXBUFF);
  Write_Byte(10, config_byte, lora_TXBUFF);
  lora_TXEnd = 10;
  lora_Send(0, lora_TXEnd, LocationBinaryPacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");
}



void Send_Test_Packet(char lmode)
{
  //build and send test packet
 
  if (lora_TestPower > 9)
  {
    lora_TXBUFF[0] = '1';
    lora_TXBUFF[1] = ((lora_TestPower - 10) + 0x30);
    lora_TXBUFF[2] = lmode;
  }
  else
  {
    lora_TXBUFF[0] = '0';
    lora_TXBUFF[1] = (lora_TestPower + 0x30);
    lora_TXBUFF[2] = lmode;
  }

  lora_TXEnd = 2;
  PrintPacket();
  Serial.print(F(" "));
  lora_Send(0, lora_TXEnd, Testpacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");
}



void Led_FlashStart()
{
  byte index;
  for (index = 0; index <= 4; index++)
  {
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
  }
}


void System_Error()
{
  while (1)
  {
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
  }
}


void init_TXLoRaTest1()
{
  //setup for testmode1
  Serial.println(F("init_TXLoRaTest1()"));
  float freq_temp;
  lora_Setup1();
  lora_SetFreq(Frequency1, CalibrationOffset);
  freq_temp = lora_GetFreq();
  Serial.print(F("Set to Frequency "));
  Serial.print(freq_temp,3);
  Serial.println(F("Mhz"));
  lora_SetModem2(test1_Bandwidth, test1_SpreadingFactor, test1_CodeRate, Explicit);	//setup the LoRa modem parameters for test
}


void init_TXLoRaTest2()
{
  //setup for testmode2
  Serial.println(F("init_TXLoRaTest2()"));
  float freq_temp;
  lora_Setup2();
  lora_SetFreq(Frequency2, CalibrationOffset);
  freq_temp = lora_GetFreq();
  Serial.print(F("Set to Frequency "));
  Serial.print(freq_temp,3);
  Serial.println(F("Mhz"));
  lora_SetModem2(test2_Bandwidth, test2_SpreadingFactor, test2_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}


void lora_Setup1()
{
  //initialise LoRa device registers and check its responding
  lora_ResetDev();        //clear all registers to default
  lora_Write(lora_RegOpMode, 0x08);   //RegOpMode, need to set to sleep mode before configure for LoRa mode
  lora_Write(lora_RegOcp, 0x2B);    //RegOcp
  lora_Write(lora_RegLna, 0x23);    //RegLna
  lora_Write(lora_RegSymbTimeoutLsb, 0xFF); //RegSymbTimeoutLsb
  lora_Write(lora_RegPreambleLsb, 0x0C);  //RegPreambleLsb, default

  if (!lora_CheckDevice())
  {
    Serial.println();
    Serial.print(F("LoRa Device Error"));
    Serial.println();
  }
}


void lora_Setup2()
{
  //initialise LoRa device registers and check its responding
  lora_ResetDev();        //clear all registers to default
  lora_Write(lora_RegOpMode, 0x08);   //RegOpMode, need to set to sleep mode before configure for LoRa mode
  lora_Write(lora_RegOcp, 0x0B);    //RegOcp
  lora_Write(lora_RegLna, 0x23);    //RegLna
  lora_Write(lora_RegSymbTimeoutLsb, 0xFF); //RegSymbTimeoutLsb
  lora_Write(lora_RegPreambleLsb, 0x0C);  //RegPreambleLsb
  lora_Write(lora_RegFdevLsb, Deviation); //LSB of deviation, 5kHz
  if (!lora_CheckDevice())
  {
    Serial.println();
    Serial.print(F("LoRa Device Error"));
    Serial.println();
  }
}

void lora_Custom1()
{
  //used to have a custom set of LoRa register settings for each test mode
}

void lora_Custom2()
{
  //used to have a custom set of LoRa register settings for each test mode
}

void lora_RXONLoRa1()
{
  //puts LoRa device into listen mode and receives packet exits with packet in array lora_RXBUFF[]
  lora_RXPacketL = 0;
  lora_RXPacketType = 0;
  lora_RXDestination = 0;
  lora_RXSource = 0;
  lora_RXStart = 0;
  lora_RXEnd = 0;
  lora_Write(lora_RegOpMode, 0x09);
  lora_Write(lora_RegFifoRxBaseAddr, 0x00);
  lora_Write(lora_RegFifoAddrPtr, 0x00);
  lora_Write(lora_RegIrqFlags, 0xFF);
  lora_Write(lora_RegIrqFlagsMask, 0x9F);                //only allow rxdone and crc error
  lora_Write(lora_RegOpMode, 0x8D);
  lora_BackGroundRSSI = lora_Read(lora_RegRssiValue);    //get the background noise level now receiver is on
}


void lora_RXONLoRa2()
{
  //puts LoRa device into listen mode and receives packet exits with packet in array lora_RXBUFF[]
  lora_RXPacketL = 0;
  lora_RXPacketType = 0;
  lora_RXDestination = 0;
  lora_RXSource = 0;
  lora_RXStart = 0;
  lora_RXEnd = 0;
  lora_Write(lora_RegOpMode, 0x09);
  lora_Write(lora_RegFifoRxBaseAddr, 0x00);
  lora_Write(lora_RegFifoAddrPtr, 0x00);
  lora_Write(lora_RegIrqFlags, 0xFF);
  lora_Write(lora_RegIrqFlagsMask, 0x9F);                //only allow rxdone and crc error
  lora_Write(lora_RegOpMode, 0x8D);
  lora_BackGroundRSSI = lora_Read(lora_RegRssiValue);    //get the background noise level now receiver is on
}

void init_RXLoRaTest1()
{
  //setup for testmode
  modenumber = 1;
  lora_Setup1();
  lora_SetFreq(Frequency1, CalibrationOffset);
  lora_SetModem2(test1_Bandwidth, test1_SpreadingFactor, test1_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}


void init_RXLoRaTest2()
{
  //setup for testmode
  modenumber = 2;
  lora_Setup1();
  lora_SetFreq(Frequency2, CalibrationOffset);
  lora_SetModem2(test2_Bandwidth, test2_SpreadingFactor, test2_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}


void setup()
{
  pinMode(LED1, OUTPUT);		                  //for PCB LED
  Led_FlashStart();

  Serial.begin(115200);                        //setup Serial console ouput
  Serial.println(F(programname));
  Serial.println(F(programversion));
  Serial.println(F(dateproduced));
  Serial.println(F(aurthorname));
  Serial.print(F("Node "));
  Serial.print(ThisNode);
  Serial.println();

  SPI.begin();
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

  pinMode(lora_NReset, OUTPUT);			           //LoRa Device reset line
  pinMode (lora_NSS, OUTPUT);			             //LoRa Device select line
  digitalWrite(lora_NSS, HIGH);
  digitalWrite(lora_NReset, HIGH);

  ramv_ThisNode = ThisNode;

  if (lora_CheckDevice() == true)
  {
    init_TXLoRaTest1();
    digitalWrite(LED1, HIGH);
    Serial.print(F("Start Test Tone "));
    lora_Tone2(1000, 1000, 2);                   //Transmit an FM tone, 1000hz, 1000ms, 2dBm
    lora_TXOFF();                               //so off time is recorded correctly
    digitalWrite(LED1, LOW);
    Serial.println(F(" - Finished"));
    Serial.println();
  }
  else
  {
    Serial.println(F("LoRa Device Error"));
    System_Error();
  }

  lora_Print();
  Serial.println();

  #ifdef EnableMode2 
  init_TXLoRaTest1();                               //setup for test mode 1
  Send_Test1Mode_Packet();                          //force switch to mode1
  #endif
}

