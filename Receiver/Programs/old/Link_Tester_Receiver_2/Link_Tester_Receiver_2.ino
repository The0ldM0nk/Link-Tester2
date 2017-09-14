#define programname "Link Test Receiver 140917"
#define programversion "V2.1"
#define dateproduced "140917"
#define aurthorname "Stuart Robinson"
#include <Arduino.h>

/*
LoRaTracker Programs

Copyright of the author Stuart Robinson

These programs may be used free of charge for personal, recreational and educational purposes only.

This program, or parts of it, may not be used for or in connection with any commercial purpose without
the explicit permission of the author Stuart Robinson.

The programs are supplied as is, it is up to individual to decide if the programs are suitable for the
intended purpose and free from errors.


To Do:


*/

//#define Frequency1 434.400                        //main frequency
//#define FreqOffset 0.0                            //adjustment for frequency in Mhz, assumed at room temp
//#define defaultfont 6
//#define PayloadArraySize 6

int SupplyVolts;
String results[5];
byte keypress;

#define LoRaTracker_Locator2                           //specify PCB type
#include "Tracker_Definitions.h"

#include "Program_Definitions.h"  
#include "Test_Settings.h" 
#include <SPI.h>
#include "LoRa3.h"
#include "Binary2.h"

/*
#include "Program_Definitions.h"  
#include "Test_Settings.h" 
#include <SPI.h>


#include "LoRa3.h"

#include "Test_Parameters.h"
#include "Binary2.h"
*/



unsigned int lora_Test1Count[20];	          //buffer where counts of received packets are stored
unsigned int lora_Test2Count[20];	          //buffer where counts of received packets are stored


void loop()
{
  checkforpacket();
}


void checkforpacket()
{
  //check if a packet has been received
  byte lora_Ltemp;
  byte i;

  lora_Ltemp = lora_readRXready();

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
    processPacket();
    lora_RXONLoRa();                                 //ready for next
    digitalWrite(LED1, LOW);
  }

  if (lora_Ltemp == 96)
  {
    Serial.println();
    Serial.println(F("CRC Error"));
    lora_RXONLoRa();                                 //ready for next
  }
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
      i = lora_Test1Count[lTXpower];
      i++;
      lora_Test1Count[lTXpower] = i;
    }
    else
    {
      i = lora_Test2Count[lTXpower];
      i++;
      lora_Test2Count[lTXpower] = i;
    }

    lora_RXBuffPrint(3);                             //print as ASCII
    Serial.print(F(" ("));
    Serial.print(lTXpower);
    Serial.print(F("dBm) "));
    Serial.println();
  }

  if (lora_RXPacketType == LMLCSVPacket)
  {
    lora_RXBuffPrint(3);                              //print as ASCII
    FillPayloadArray(lora_RXStart, lora_RXEnd);
    lTXpower = results[3].toInt();                    //read the power field
    Serial.println();
  }
  
  
  if (lora_RXPacketType == LocationBinaryPacket)
  {
    tempfloat = Read_Float(0, lora_RXBUFF);
    Serial.print(tempfloat,5);
    Serial.print(F("  "));
    tempfloat = Read_Float(4, lora_RXBUFF);
    Serial.print(tempfloat,5);
    Serial.print(F("  "));
    tempuint = Read_UInt(8, lora_RXBUFF);
    Serial.print(tempuint);
    Serial.print(F("  "));
    tempbyte = Read_Byte(10, lora_RXBUFF);
    Serial.print(tempbyte);
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
    lora_RXONLoRa();
    print_Test1Count();
    print_Test2Count();
    Serial.println();
  }

  if (lora_RXPacketType == TestMode2)
  {
    //this is a command to switch to TestMode1
    Serial.println();
    Serial.println();
    Serial.println(F("Switch Mode2"));
    Serial.println();
    init_RXLoRaTest2();
    lora_RXONLoRa();
    print_Test1Count();
    print_Test2Count();
    Serial.println();
  }

}


void print_Test1Count()
{
  //prints running totals of the power of received packets
  byte i;
  unsigned long j;
  Serial.print(F("Mode1 "));
  for (i = 17; i >= 2; i--)
  {
    Serial.print(i);
    Serial.print(",");
    j = lora_Test1Count[i];
    Serial.print(j);
    Serial.print("  ");
  }
  Serial.println();
}


void print_Test2Count()
{
  //prints running totals of the power of received packets
  byte i;
  unsigned long j;
  Serial.print(F("Mode2 "));
  for (i = 17; i >= 2; i--)
  {
    Serial.print(i);
    Serial.print(",");
    j = lora_Test2Count[i];
    Serial.print(j);
    Serial.print("  ");
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
  lora_ResetDev();        //clear all registers to default
  lora_Write(lora_RegOpMode, 0x08);   //RegOpMode, need to set to sleep mode before configure for LoRa mode
  lora_Write(lora_RegOcp, 0x0B);    //RegOcp
  lora_Write(lora_RegLna, 0x23);    //RegLna
  lora_Write(lora_RegSymbTimeoutLsb, 0xFF); //RegSymbTimeoutLsb
  lora_Write(lora_RegPreambleLsb, 0x0C);  //RegPreambleLsb, default
  lora_Write(lora_RegFdevLsb, Deviation);  //LSB of deviation, 5kHz
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
  lora_Write(lora_RegPreambleLsb, 0x0C);  //RegPreambleLsb, default
  lora_Write(lora_RegFdevLsb, Deviation);  //LSB of deviation, 5kHz
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
  pinMode(LED1, OUTPUT);		         //setup pin for PCB LED
  led_FlashStart();

  Serial.begin(38400);                            //setup Serial console ouput
  Serial.println(F(programname));
  Serial.println(F(programversion));
  Serial.println(F(dateproduced));
  Serial.println(F(aurthorname));
  Serial.println();

  Serial.println();
  pinMode(lora_NReset, OUTPUT);		         //setup pin for LoRa device reset line
  digitalWrite(lora_NReset, HIGH);
  pinMode (lora_NSS, OUTPUT);		         //setup pin for LoRa device slave select
  digitalWrite(lora_NSS, HIGH);

  SPI.begin();				         //initialize SPI
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  if (lora_CheckDevice() == true)
  {
    init_RXLoRaTest1();				                  //Do the initial LoRa Setup
    Serial.println(F("LoRa Tone")); 
    digitalWrite(LED1, HIGH);
    lora_Tone(1000, 3000, startup_Power);        //Transmit an FM tone, 1000hz, 3000ms
    digitalWrite(LED1, LOW); 
    Serial.println();
  }
  else
  {
    Serial.println(F("LoRa Device Error"));
    systemerror();
  }

  init_RXLoRaTest1();                            //do the initial LoRa Setup for testmode 1
  lora_RXONLoRa();

}


