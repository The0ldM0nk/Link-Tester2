//Ping_Transmitter_130118.ino
#define programname "Ping_Transmitter_130118"
#define programversion "V1.0"
#define dateproduced "13/1/18"
#define aurthorname "Stuart Robinson"
#include <Arduino.h>
#define LoRa_Device_in_MB1                           //required if board is using Mikrobus sockets

/*
*****************************************************************************************************************************
  LoRaTracker Test Programs

  Copyright of the author Stuart Robinson - 13/1/2018

  http://www.LoRaTracker.uk

  These programs may be used free of charge for personal, recreational and educational purposes only.

  This program, or parts of it, may not be used for or in connection with any commercial purpose without the explicit permission
  of the author Stuart Robinson.

  The programs are supplied as is, it is up to individual to decide if the programs are suitable for the intended purpose and
  free from errors.
*****************************************************************************************************************************
*/

/*
********************************************************************************************************************************
  Program operation

  This test program has been written to enable simpel checks to be made on the operation of antennas or receivers. The transmitter
  can send a simple FM tone, this can be used to measure the strength of signal transmitted. The measurement device could be a 
  low cost SDR, or for field or portable use an RF Explorer or even a tracker board with display setup as an RSSI meter. 

  The transmitter can also send a range of data packets and the distances at which these are received can be used to measure 
  antenna or receiver efficiency. 
  
  Do not forget to fit an antenna to the LoRa device, you can destroy it if you use it without and antenna
********************************************************************************************************************************
*/

/*
********************************************************************************************************************************
  Connections

  The program uses the hardware SPI interface on the Arduino to connect to the LoRa device, so the SPI SCK, MOSI and MISO pins are
  assumed to be connected. The test transmitter program needs pins connected to act as chip select, and to generate the FM tones.
  The reset pin on the LoRa device can be ommited.

  You can explicitly define the required pins below by removing the two // characters in front of the #defines
********************************************************************************************************************************
*/

//#define lora_NSS 10                   //pin number where the chip select for the LoRa device is connected
//#define lora_NReset 9                 //pin where LoRa device reset line is connected
//#define lora_TonePin 6                //pin number for radio tone generation, connects to LoRa device pin DIO2
//#define lora_DIO0 2                   //pin connected to DIO0 on LoRa device, used on receiver
//#define LED1 8                        //pin number for LED
//#define BUZZ 7                        //pin number for buzzer, used on receiver

/*
***********************************************************************************************************************************************
  As an alternative to explicitly defining the Arduino pins required, there are pre-defined board definition files for the LoRaTracker boards
  included in the LoRaTracker Library;

  https://github.com/LoRaTracker/LoRaTracker-Library

  Select (include) the board definition file you require by removing the // characters before the appropriate include line in the list below
***********************************************************************************************************************************************
*/

//#include "HAB2_Board_Definitions.h"
//#include "Locator2_Board_Definitions.h"
//#include "LCD_Receiver_Board_Definitions.h"
//#include "Relay1_Board_Definitions.h"
//#include "Receiver2_Board_Definitions.h"
//#include "Sensor1_Board_Definitions.h"
//#include "MicroRX_Board_Definitions.h"
//#include "PIHTracker2_Board_Definitions.h"
//#include "PIHTracker3_Board_Definitions.h"
//#include "RFM98_Shield_January2016_Board_Definitions.h"
//#include "MarkTwo_Board_Definitions.h"
//#include "MicroBit_Board_Definitions.h"


/*
**************************************************************************************************
  Changes:


  To do:


******************************************************************************************************
*/


const char Broadcast = '*';             //Broadcast address
const char Testpacket = 'T';            //Test packet
const char LocationBinaryPacket = 's';  //Short location packet in binary format
const char HABPacket = '$';             //HAB Style location payload in CSV ASCII format
const char LocationCSVPacket = 'S';     //Short loacation packet in CSV ASCII format
const char ACK = 'A';                   //Acknowledge
const char ClearToSendCommand = 'c';    


unsigned int seq = 0;
unsigned int testloop = 0;
byte keypress;

#include <SPI.h>
#include "Test_Settings.h"
#include "LoRa_Test.h"
#include "Binary_Test.h"





#ifdef USE_NON_AVR                                                //may be needed for non AVR cores
#include "avr/dtostrf.h"                                   
#endif



#define Serial_Monitor_Baud 115200                                //this is baud rate used for the Arduino IDE Serial Monitor


void loop()
{

  testloop++;

  Serial.print(F("Test Loop "));
  Serial.println(testloop);
  Serial.println();
  Serial.flush();


#ifdef TransmittTestTone
  digitalWrite(LED1, HIGH);
  Serial.println(F("Transmitt Test Tone "));
  lora_Tone(tone_frequencyHz, tone_lengthmS, transmit_power);    //Transmit a pseudo FM tone
  digitalWrite(LED1, LOW);
  delay(transmission_delaymS);
#endif


  init_TXLoRaTest();                                             //setup for test mode 1


#ifdef SendTestpacket
  digitalWrite(LED1, HIGH);
  Serial.print("Send Test Packet ");
  Serial.flush();
  Send_Test_Packet('1');
  Serial.println();
  digitalWrite(LED1, LOW);
  delay(transmission_delaymS);
#endif


#ifdef SendLocationCSVPacket
  digitalWrite(LED1, HIGH);
  Serial.print("Send Location CSV Packet ");
  Serial.flush();
  Send_LocationCSV_Packet();
  Serial.println();
  digitalWrite(LED1, LOW);
  delay(transmission_delaymS);
#endif


#ifdef SendBinaryLocation
  digitalWrite(LED1, HIGH);
  Serial.print("Send Binary Location Packet ");
  Serial.flush();
  Send_Binary_Location_Packet();
  Serial.println();
  digitalWrite(LED1, LOW);
  delay(transmission_delaymS);
#endif


#ifdef SendHABPacket
  digitalWrite(LED1, HIGH);
  Serial.print("Send HAB Packet ");
  Serial.flush();
  Send_HAB_Packet();
  Serial.println();
  digitalWrite(LED1, LOW);
  delay(transmission_delaymS);
#endif

}


void Send_HAB_Packet()
{
  byte Count;
  Count = buildHABPayload();
  lora_TXEnd = Count;
  PrintPacket();
  Serial.print(F(" "));
  lora_Send(0, Count, HABPacket, Broadcast, ThisNode, 10, transmit_power, NoStrip);   //send the packet, data is in TXbuff from lora_TXStart to lora_TXEnd
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");

}


byte buildHABPayload()
{
  //build the long tracker payload
  unsigned int index, index2, hours, mins, seconds, alt, sats, volts, internaltemperature, resets;
  unsigned int ramc_Current_config, TRStatus, runmAhr, GPSfixtime;
  unsigned int CRC;
  byte Count, len;
  byte max_length = 128;
  int calibration;
  char LatArray[10], LonArray[10];
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
  ramc_Current_config = 0x02;
  TRStatus = 0x81;
  runmAhr = 45;
  calibration = 2000;
  GPSfixtime = 4.5;

  len = sizeof(lora_TXBUFF);
  memset(lora_TXBUFF, 0, len);                                 //clear array to 0s

  node[0] = ThisNode;
  node[1] = 0;

  Count = snprintf((char*) lora_TXBUFF,
                   max_length,
                   "$$$$%s%s,%d,%02d:%02d:%02d,%s,%s,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
                   Flight_ID,
                   node,                                       //defined by 'ThisNode' in settings file
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
                   ramc_Current_config,
                   TRStatus,
                   runmAhr,
                   calibration,
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
                   transmit_power
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



void Send_LocationCSV_Packet()
{
  //now build and send Short payload
  byte Count;
  Count = Build_LocationCSV_Packet();
  lora_TXEnd = Count;
  PrintPacket();
  lora_Send(0, lora_TXEnd, LocationCSVPacket, Broadcast, ThisNode, 10, transmit_power, NoStrip);
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
  lora_Send(0, lora_TXEnd, LocationBinaryPacket, Broadcast, ThisNode, 10, transmit_power, NoStrip);
  Serial.print(F("TX Time "));
  Serial.print(lora_TXTime);
  Serial.print("mS ");
}



void Send_Test_Packet(char lmode)
{
  //build and send test packet

  if (transmit_power > 9)
  {
    lora_TXBUFF[0] = '1';
    lora_TXBUFF[1] = ((transmit_power - 10) + 0x30);
    lora_TXBUFF[2] = lmode;
  }
  else
  {
    lora_TXBUFF[0] = '0';
    lora_TXBUFF[1] = (transmit_power + 0x30);
    lora_TXBUFF[2] = lmode;
  }

  lora_TXEnd = 2;
  PrintPacket();
  Serial.print(F(" "));
  lora_Send(0, lora_TXEnd, Testpacket, Broadcast, ThisNode, 10, transmit_power, NoStrip);
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


void init_TXLoRaTest()
{
  float freq_temp;
  lora_Setup();
  lora_SetFreq(Frequency1, CalibrationOffset);
  freq_temp = lora_GetFreq();
  Serial.print(F("Set to Frequency "));
  Serial.print(freq_temp, 3);
  Serial.println(F("Mhz"));
  lora_SetModem2(test1_Bandwidth, test1_SpreadingFactor, test1_CodeRate, Explicit);	//setup the LoRa modem parameters for test
}


void setup()
{
  pinMode(LED1, OUTPUT);		                  //for PCB LED
  Led_FlashStart();

  Serial.begin(Serial_Monitor_Baud);                        //setup Serial console ouput
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


  if (lora_CheckDevice() == true)
  {
    Serial.println(F("LoRa Device Found"));
    #ifdef EnableStartupTone
    init_TXLoRaTest();
    digitalWrite(LED1, HIGH);
    Serial.print(F("Start Test Tone "));
    lora_Tone(1000, 2500, 2);                   //Transmit an FM tone, 1000hz, 1000ms, 2dBm
    lora_TXOFF();                               //so off time is recorded correctly
    digitalWrite(LED1, LOW);
    Serial.println(F(" - Finished"));
    Serial.println();
    #endif
  }
  else
  {
    Serial.println(F("LoRa Device Error"));
    System_Error();
  }

  lora_Print();
  Serial.println();

  init_TXLoRaTest();                               //setup for test mode 1
}

