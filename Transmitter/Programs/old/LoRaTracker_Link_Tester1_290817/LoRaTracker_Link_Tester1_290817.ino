//LoRaTracker_Link_Tester1_150617.ino


#define programname "LoRaTracker_Link_Tester1_150617"
#define programversion "V1.0"
#define dateproduced "15/06/17"
#define aurthorname "Stuart Robinson"

#include <Arduino.h>
      
/*
**************************************************************************************************
  
  LoRaTracker Programs for Arduino

  Copyright of the author Stuart Robinson - 15/06/17

  http://www.LoRaTracker.uk

  These programs may be used free of charge for personal, recreational and educational purposes only.

  This program, or parts of it, may not be used for or in connection with any commercial purpose without
  the explicit permission of the author Stuart Robinson.

  The programs are supplied as is, it is up to individual to decide if the programs are suitable for the
  intended purpose and free from errors.

    To do;

******************************************************************************************************
*/

//Global Program Variables

unsigned int seq = 0;
unsigned int testloop = 0;
byte keypress;

#define LoRa_Device_in_MB1
#include "PIHTracker2_Board_Definitions.h"           //define the board type here
#include "Program_Definitions.h"  
#include "Test_Settings.h" 
#include <SPI.h>
#include "LoRa3.h"
#include "Binary2.h"



void loop()
{

  testloop++;
  
  Serial.print(F("Test Loop "));
  Serial.println(testloop);
  Serial.flush();

  Serial.print(F("Loop Start Tone "));
  Serial.flush();
  lora_Tone(500, LoopStartTone_lengthmS, start_power);       //Transmit an pseudo FM tone
  delay(250);

  /*
  *********************************************************
  Start Mode 1 routines
  *********************************************************
  */

#ifdef EnableMode1

  init_TXLoRaTest2();                                        //make sure we send the change mode at the correct test settings
  Serial.println();
  Serial.print("Send Initialise Test Mode1 ");
  Send_Test1Mode_Packet();                                   //causes RX to switch to mode 1 
  delay(Mode1StartDelaymS);                                  //delay a bit after sending mode switch to allow RX time to print totals
  init_TXLoRaTest1();                                        //now we can go into test mode 1
  Serial.println();
  Serial.println();

  for (lora_TestPower = start_power; lora_TestPower >= end_power; lora_TestPower--)
  {
    Serial.print(lora_TestPower);
    Serial.println("dBm  Packet");

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

  init_TXLoRaTest1();                             //make sure we send the change mode at the correct test settings
  Serial.println();
  Serial.print("Send Initialise Test Mode2 ");
  Send_Test2Mode_Packet();                      //causes RX to switch to mode 2
  delay(Mode1StartDelaymS);                     //delay a bit after sending mode switch to allow RX time to print totals
  init_TXLoRaTest2();                              //now we can go into test mode 2        
  Serial.println();
  Serial.println();
  delay(2000);

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
  buildHABPayload();
  PrintPacket();
  Serial.print(F(" "));
  lora_Send(0, lora_TXEnd, HABPacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);   //send the packet, data is in TXbuff from lora_TXStart to lora_TXEnd
  //lora_TXOFF();                                     //so off time is recorded correctly
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");

}


void buildHABPayload()
{
  //build the long tracker payload
  unsigned int index, index2, hours, mins, seconds, alt, sats, volts, internaltemperature, resets;
  unsigned int ramv_Current_config, TRStatus, runmAhr, GPSfixtime;
  unsigned int CRC;
  byte Count;
  byte max_length = 128;
  float calibration;
  char LatArray[10], LonArray[10], CalibrationArray[10];
  char node[2];
  char output_BUFF[128];
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

  memset(output_BUFF, 0, sizeof(output_BUFF));                        //clear array

  node[0] = ramv_ThisNode;
  node[1] = 0;

  snprintf(output_BUFF,
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

  Count = strlen(output_BUFF);                      //how long is the array so far ?

  CRC = 0xffff;                                   //start value for CRC16

  for (index = 4; index < Count; index++)                     //element 4 is first character after $$$$ at start
  {
    CRC ^= (((unsigned int)output_BUFF[index]) << 8);
    for (index2 = 0; index2 < 8; index2++)
    {
      if (CRC & 0x8000)
        CRC = (CRC << 1) ^ 0x1021;
      else
        CRC <<= 1;
    }
  }

  output_BUFF[Count++] = '*';
  output_BUFF[Count++] = Hex((CRC >> 12) & 15);      //add the checksum bytes to the end
  output_BUFF[Count++] = Hex((CRC >> 8) & 15);
  output_BUFF[Count++] = Hex((CRC >> 4) & 15);
  output_BUFF[Count] = Hex(CRC & 15);
  lora_TXEnd = Count;
}


int Build_LocationCSV_Packet()
{
  //build short payload in buffer
  byte Count;
  byte max_length = 64;
  char LatArray[10], LonArray[10];
  unsigned int alt;
  char output_BUFF[128];
  
  dtostrf(TestLatitude, 7, 5, LatArray);
  dtostrf(TestLongitude, 7, 5, LonArray);
  alt = TestAltitude;
  
  memset(output_BUFF, 0, sizeof(output_BUFF));  
  
  snprintf(output_BUFF,
           max_length,
           "%s,%s,%d,%d,",
           LatArray,
           LonArray,
           alt,
           lora_TestPower
          );

  Count = strlen(output_BUFF);
  lora_TXEnd = Count;
}




char Hex(char lchar)
{
  //used in CRC calculation
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
  lora_TXBUFF[0] = '1';
  lora_Send(0, 0, TestMode1, Broadcast, ThisNode, 10, start_power, NoStrip);
  //lora_TXOFF();                                     //so off time is recorded correctly
  delay(2000);              //leave time for receiver to print running totals
}

void Send_Test2Mode_Packet()
{
  //causes RX to switch mode
  lora_TXBUFF[0] = '2';
  lora_Send(0, 0, TestMode2, Broadcast, ThisNode, 10, start_power, NoStrip);
  //lora_TXOFF();                                     //so off time is recorded correctly
  delay(2000);              //leave time for receiver to print running totals
}


void Send_LocationCSV_Packet()
{
  //now build and send Short payload
  Build_LocationCSV_Packet();
  PrintPacket();
  lora_Send(0, lora_TXEnd, LocationCSVPacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);
  //lora_TXOFF();                                     //so off time is recorded correctly
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");
}


void Send_Binary_Location_Packet()
{
  Serial.print(F(" "));
  Serial.print(TestLatitude, 5);
  Serial.print(F(" "));
  Serial.print(TestLongitude, 5);
  Serial.print(F(" "));
  Serial.print(TestAltitude, 1);
  Serial.print(F(" "));
  Serial.print(config_byte);
  Serial.print(F(" "));
  Write_Float(0, TestLatitude, lora_TXBUFF);
  Write_Float(4, TestLongitude, lora_TXBUFF);
  Write_Float(8, TestAltitude, lora_TXBUFF);
  Write_Byte(12, config_byte, lora_TXBUFF);
  #ifdef DEBUG
  Print_Binary_Buffer();
  #endif
  lora_Send(0, 13, LocationBinaryPacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);
  //lora_TXOFF();                                     //so off time is recorded correctly
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");
}


void Print_Binary_Buffer()
{
  byte index, bufferData;
  Serial.print(" Binary Buffer ");
  for (index = 0; index <= 13; index++)
  {
    bufferData = lora_TXBUFF[index];
    Serial.print(bufferData);
    Serial.print(" ");
  }
}


void Send_Test_Packet(char lmode)
{
  //build and send test packet
  int count;

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
  lora_Send(0, 2, Testpacket, Broadcast, ThisNode, 10, lora_TestPower, NoStrip);
  //lora_TXOFF();                                     //so off time is recorded correctly
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
  //setup for testmode
  uint32_t freq_temp;
  lora_Setup1();
  lora_SetFreq(Frequency1, FrequencyOffset);
  freq_temp = lora_GetFreq();
  Serial.print(F("Set to Frequency "));
  Serial.println(freq_temp);
  lora_SetModem2(test1_Bandwidth, test1_SpreadingFactor, test1_CodeRate, Explicit);	//setup the LoRa modem parameters for test
#ifdef DEBUG
  Serial.println();
  lora_Print();
#endif
}


void init_TXLoRaTest2()
{
  //setup for testmode
  uint32_t freq_temp;
  lora_Setup2();
  lora_SetFreq(Frequency2, FrequencyOffset);
  freq_temp = lora_GetFreq();
  Serial.print(F("Set to Frequency "));
  Serial.println(freq_temp);
  lora_SetModem2(test2_Bandwidth, test2_SpreadingFactor, test2_CodeRate, Explicit);  //setup the LoRa modem parameters for test
#ifdef DEBUG
  Serial.println();
  lora_Print();
#endif
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
  byte lora_Lvar1, lora_LRegData, lora_LLoop;
  long lora_Lvar2;
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
  byte lora_Lvar1, lora_LRegData, lora_LLoop;
  long lora_Lvar2;
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
  lora_SetFreq(Frequency1, FreqOffset);
  lora_SetModem2(test1_Bandwidth, test1_SpreadingFactor, test1_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}


void init_RXLoRaTest2()
{
  //setup for testmode
  modenumber = 2;
  lora_Setup1();
  lora_SetFreq(Frequency2, FreqOffset);
  lora_SetModem2(test2_Bandwidth, test2_SpreadingFactor, test2_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}


void setup()
{
  pinMode(LED1, OUTPUT);		        //for PCB LED
  Led_FlashStart();

  Serial.begin(38400);                        //setup Serial console ouput
  Serial.println(F(programname));
  Serial.println(F(programversion));
  Serial.println(F(dateproduced));
  Serial.println(F(aurthorname));
  Serial.print(F("Node "));
  Serial.print(ThisNode);
  Serial.println();
  Serial.println();

  SPI.begin();
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

  pinMode(lora_NReset, OUTPUT);			        //LoRa Device reset line
  pinMode (lora_NSS, OUTPUT);			          //LoRa Device select line
  digitalWrite(lora_NSS, HIGH);
  digitalWrite(lora_NReset, HIGH);

  if (lora_CheckDevice() == true)
  {
    init_TXLoRaTest1();
    digitalWrite(LED1, HIGH);
    Serial.println();
    Serial.print(F("Start Test Tone "));
    lora_Tone(1000, 1000, 2);                   //Transmit an FM tone, 1000hz, 1000ms, 2dBm
    lora_TXOFF();                                     //so off time is recorded correctly
    digitalWrite(LED1, LOW);
    Serial.println(F(" - Finished"));
    Serial.println();
  }
  else
  {
    Serial.println(F("LoRa Device Error"));
    System_Error();
  }

  delay(2000);

}

