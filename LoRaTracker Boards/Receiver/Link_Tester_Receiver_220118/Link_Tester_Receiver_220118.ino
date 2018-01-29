#define programname "Link_Tester_Receiver_220118"
#define programversion "V2.3"
#define dateproduced "22/01/18"
#define aurthorname "Stuart Robinson"
#include <Arduino.h>

/*
**************************************************************************************************

  LoRaTracker Programs for Arduino

  Copyright of the author Stuart Robinson - 22/1/18

  http://www.LoRaTracker.uk

  These programs may be used free of charge for personal, recreational and educational purposes only.

  This program, or parts of it, may not be used for or in connection with any commercial purpose without
  the explicit permission of the author Stuart Robinson.

  The programs are supplied as is, it is up to individual to decide if the programs are suitable for the
  intended purpose and free from errors.

  To do;


******************************************************************************************************
*/

/*
********************************************************************************************************************************
  Connections

  You can explicitly define the required pin below by removing the two // characters in front of the #defines
********************************************************************************************************************************
*/

//#define lora_NSS 10                   //pin number where the chip select for the LoRa device is connected
//#define lora_NReset 9                 //pin where LoRa device reset line is connected
//#define lora_TonePin 6                //pin number for radio tone generation, connects to LoRa device pin DIO2
//#define lora_DIO0 2                   //pin connected to DIO0 on LoRa device
//#define LED1 8                        //pin number for LED

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
//#include "Receiver2_Board_Defintions.h"
//#include "Sensor1_Board_Definitions.h"
//#include "MicroRX_Board_Definitions.h"
//#include "PIHTracker2_Board_Definitions.h"
#include "PIHTracker3_Board_Definitions.h"
//#include "RFM98_Shield_January2016_Board_Definitions.h"
//#include "MarkTwo_Board_Definitions.h"



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

String results[5];
byte modenumber;
byte keypress;
byte lora_TestPower = 2;                //is also start up tone power

unsigned long Mode1_Packet_Count = 0;
unsigned long Mode2_Packet_Count = 0;

byte Mode1_Max_Power = 0;
byte Mode2_Max_Power = 0;

unsigned long Mode1_Cycles = 0;
unsigned long Mode2_Cycles = 0;


#include "Test_Settings.h"
#include <SPI.h>
#include "LoRa_Test.h"
#include "Binary_Test.h"

unsigned int lora_Test1Count[20];            //buffer where counts of received packets are stored
unsigned int lora_Test2Count[20];           //buffer where counts of received packets are stored
signed long ramc_CalibrationOffset = CalibrationOffset;

void loop()
{
  checkforpacket();
}


void checkforpacket()
{
  //check if a packet has been received
  byte lora_Ltemp;
  lora_Ltemp = lora_readRXready2();                  //this reads the DIO0 pin, creates less EMI than polling the LoRa device registers.

  if (lora_Ltemp == 64)
  {
    lora_RXOFF();                                    //stop more packets comming in

    digitalWrite(LED1, HIGH);
    Serial.print(F("RX "));
    lora_ReadPacket();
    lora_ReceptionInfo();
    Serial.print("  ");
    lora_AddressInfo();
    Serial.print("  ");

#ifdef LORA_AFC
   check_AFC(); 
#endif

    processPacket();
    lora_RXONLoRa();                                 //ready for next
    digitalWrite(LED1, LOW);
  }

  if (lora_Ltemp == 96)
  {
    Serial.println(F("CRC Error"));
    lora_RXONLoRa();                                 //ready for next
  }
}


void check_AFC()
{
    signed int freqerror;
    freqerror = lora_GetFrequencyError();
    Serial.print(F("FreqErr="));
    Serial.print(freqerror);
    Serial.print("  ");
    ramc_CalibrationOffset = ramc_CalibrationOffset - freqerror;
    Serial.print(F(" CalibrationOffset="));
    Serial.print(ramc_CalibrationOffset);
    Serial.print("  ");
    lora_SetFreq(Frequency1, ramc_CalibrationOffset);
}


void processPacket()
{
  byte lTXpower;
  unsigned int i, tempuint;
  float tempfloat;
  byte tempbyte;


  if (lora_RXPacketType == Testpacket)
  {
    lTXpower = ((lora_RXBUFF[0] - 48) * 10) +  (lora_RXBUFF[1] - 48);

    if (lora_RXBUFF[2] == '1')
    {
      if (Mode1_Cycles > 0)
      {
        Mode1_Packet_Count++;
        i = lora_Test1Count[lTXpower];
        i++;
        lora_Test1Count[lTXpower] = i;
      }

      if (lTXpower > Mode1_Max_Power)
      {
        Mode1_Max_Power = lTXpower;
      }
    }


    if (lora_RXBUFF[2] == '2')
    {
      if (Mode2_Cycles > 0)
      {
        Mode2_Packet_Count++;
        i = lora_Test2Count[lTXpower];
        i++;
        lora_Test2Count[lTXpower] = i;
      }

      if (lTXpower > Mode2_Max_Power)
      {
        Mode2_Max_Power = lTXpower;
      }
    }

    Serial.print(F(" ("));
    Serial.print(lTXpower);
    Serial.print(F("dBm) "));
    Serial.println();
  }


  if (lora_RXPacketType == LocationCSVPacket)
  {
    lora_RXBuffPrint(0);                              //print as ASCII
    FillPayloadArray(lora_RXStart, lora_RXEnd);
    lTXpower = results[3].toInt();                    //read the power field
    Serial.println();
  }


  if (lora_RXPacketType == LocationBinaryPacket)
  {
    tempfloat = Read_Float(0, lora_RXBUFF);
    Serial.print(tempfloat, 5);
    Serial.print(F("  "));
    tempfloat = Read_Float(4, lora_RXBUFF);
    Serial.print(tempfloat, 5);
    Serial.print(F("  "));
    tempuint = Read_UInt(8, lora_RXBUFF);
    Serial.print(tempuint);
    Serial.print(F("  "));
    tempbyte = Read_Byte(10, lora_RXBUFF);
    Serial.print(tempbyte);
    Serial.println();
  }


  if (lora_RXPacketType == HABPacket)
  {
    lora_RXBuffPrint(0);
    Serial.println();
  }


  if (lora_RXPacketType == TestMode1)
  {
    //this is a command to switch to TestMode1
    Serial.println();
    Serial.println();
    Serial.println(F("Switch Mode1"));
    Serial.println();

    init_RXLoRaTest1();

    if (Mode1_Cycles > 0)
    {
      print_Test1Count();
    }

    if (Mode2_Cycles > 0)
    {
      print_Test2Count();
    }


    Serial.println();

    Mode1_Cycles++;

    lora_RXONLoRa();

  }


  if (lora_RXPacketType == TestMode2)
  {
    //this is a command to switch to TestMode2
    Serial.println();
    Serial.println();
    Serial.println(F("Switch Mode2"));
    Serial.println();

    init_RXLoRaTest2();

    if (Mode1_Cycles > 0)
    {
      print_Test1Count();
    }

    if (Mode2_Cycles > 0)
    {
      print_Test2Count();
    }
    Serial.println();

    Mode2_Cycles++;

    lora_RXONLoRa();

  }

}


void print_Test1Count()
{
  //prints running totals of the power of received packets  
  byte i;
  unsigned long j;

  Serial.print(F("Mode1 Test Packets "));
  Serial.println(Mode1_Packet_Count);
  Serial.print(F("Mode1 Test Cycles "));
  Serial.println(Mode1_Cycles);
  Serial.print(F("Mode1 Max Power "));

  if (Mode1_Max_Power == 0)
  {
    Serial.println(F("NA"));
  }
  else
  {
    Serial.print(Mode1_Max_Power);
    Serial.println(F("dBm"));
  }

  Serial.print(F("Mode1  "));
  for (i = 17; i >= 2; i--)
  {
    Serial.print(i);
    Serial.print("dBm,");
    j = lora_Test1Count[i];
    Serial.print(j);
    Serial.print("  ");
  }
  Serial.println();

  Serial.print("Mode1,CSV");
  for (i = 17; i >= 2; i--)
  {
    Serial.print(",");
    j = lora_Test1Count[i];
    Serial.print(j);
  }
  Serial.println();

}


void print_Test2Count()
{
  //prints running totals of the power of received packets
  byte i;
  unsigned long j;

  Serial.print(F("Mode2 Test Packets "));
  Serial.println(Mode2_Packet_Count);
  Serial.print(F("Mode2 Test Cycles "));
  Serial.println(Mode2_Cycles);
  Serial.print(F("Mode2 Max Power "));

  if (Mode2_Max_Power == 0)
  {
    Serial.println(F("NA"));
  }
  else
  {
    Serial.print(Mode2_Max_Power);
    Serial.println(F("dBm"));
  }

  Serial.print(F("Mode2  "));
  for (i = 17; i >= 2; i--)
  {
    Serial.print(i);
    Serial.print("dBm,");
    j = lora_Test2Count[i];
    Serial.print(j);
    Serial.print("  ");
  }
  Serial.println();

  Serial.print("Mode2,CSV");
  for (i = 17; i >= 2; i--)
  {
    Serial.print(",");
    j = lora_Test2Count[i];
    Serial.print(j);
  }
  Serial.println();
}


void FillPayloadArray(byte llora_RXStart, byte llora_RXEnd)
{
  //fill the payload array from the CSV data in the packet
  byte i = 0;
  byte j = 0;
  byte lvar1;
  String tempstring = "";

  for (i = llora_RXStart; i <= llora_RXEnd; i++)
  {
    lvar1 = lora_RXBUFF[i];

    if (lvar1 == ',')
    {
      results[j] = tempstring;
      j++;
      tempstring = "";
      if (j > PayloadArraySize)
      {
        Serial.print(F("ERROR To many Fields"));
        Serial.println(j);
        break;
      }
    }
    else
    {
      tempstring = tempstring + char(lvar1);
    }
  }
}


void systemerror()
{
  while (1)
  {
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
  }
}


void led_FlashStart()
{
  byte i;
  for (i = 0; i <= 4; i++)
  {
    digitalWrite(LED1, HIGH);
    delay(100);
    digitalWrite(LED1, LOW);
    delay(100);
  }
}



void lora_Setup1()
{
  //initialise LoRa device registers and check its responding
  lora_ResetDev();                               //clear all registers to default
  lora_Write(lora_RegOpMode, 0x08);              //RegOpMode, need to set to sleep mode before configure for LoRa mode
  lora_Write(lora_RegOcp, 0x0B);                 //RegOcp
  lora_Write(lora_RegLna, 0x23);                 //RegLna
  lora_Write(lora_RegSymbTimeoutLsb, 0xFF);      //RegSymbTimeoutLsb
  lora_Write(lora_RegPreambleLsb, 0x0C);         //RegPreambleLsb, default
  lora_Write(lora_RegFdevLsb, Deviation);        //LSB of deviation, 5kHz
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
  lora_ResetDev();                                //clear all registers to default
  lora_Write(lora_RegOpMode, 0x08);               //RegOpMode, need to set to sleep mode before configure for LoRa mode
  lora_Write(lora_RegOcp, 0x0B);                  //RegOcp
  lora_Write(lora_RegLna, 0x23);                  //RegLna
  lora_Write(lora_RegSymbTimeoutLsb, 0xFF);       //RegSymbTimeoutLsb
  lora_Write(lora_RegPreambleLsb, 0x0C);          //RegPreambleLsb, default
  lora_Write(lora_RegFdevLsb, Deviation);         //LSB of deviation, 5kHz
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

void init_RXLoRaTest1()
{
  //setup for testmode
  modenumber = 1;
  lora_Setup1();
  lora_SetFreq(Frequency1, ramc_CalibrationOffset);
  lora_SetModem2(test1_Bandwidth, test1_SpreadingFactor, test1_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}


void init_RXLoRaTest2()
{
  //setup for testmode
  modenumber = 2;
  lora_Setup1();
  lora_SetFreq(Frequency2, ramc_CalibrationOffset);
  lora_SetModem2(test2_Bandwidth, test2_SpreadingFactor, test2_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}



void setup()
{
  pinMode(LED1, OUTPUT);		                     //setup pin for PCB LED
  led_FlashStart();

  Serial.begin(115200);                           //setup Serial console ouput
  Serial.println(F(programname));
  Serial.println(F(programversion));
  Serial.println(F(dateproduced));
  Serial.println(F(aurthorname));
  Serial.println();

  Serial.println();
  pinMode(lora_NReset, OUTPUT);		               //setup pin for LoRa device reset line
  digitalWrite(lora_NReset, HIGH);
  pinMode (lora_NSS, OUTPUT);		                 //setup pin for LoRa device slave select
  digitalWrite(lora_NSS, HIGH);
  pinMode(lora_DIO0, INPUT);                     //setup pin to read LoRa device RX ready

  SPI.begin();				                           //initialize SPI
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

  if (lora_CheckDevice() == true)
  {
#ifdef EnableTone
    init_RXLoRaTest1();				                   //Do the initial LoRa Setup
    Serial.println(F("LoRa Tone"));
    digitalWrite(LED1, HIGH);
    lora_Tone(1000, 2500, 2);       //Transmit an FM tone, 1000hz, 3000ms
    digitalWrite(LED1, LOW);
    Serial.println();
#endif
  }
  else
  {
    Serial.println(F("LoRa Device Error"));
    systemerror();
  }

  Serial.println();
  lora_Print();
  Serial.println();

  init_RXLoRaTest1();                            //do the initial LoRa Setup for testmode 1
  lora_RXONLoRa();

  Serial.println(F("Ready"));


}


