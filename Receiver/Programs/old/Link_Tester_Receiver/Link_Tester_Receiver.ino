#define programname "LoRa RX Tester"
#define programversion "V1.1"
#define dateproduced "181016"
#define aurthorname "Stuart Robinson"

#define LoRaTracker_Locator2                           //specify PCB type

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

#define Frequency1 434.400                        //main frequency
#define FreqOffset 0.0                            //adjustment for frequency in Mhz, assumed at room temp
#define defaultfont 6
#define PayloadArraySize 6

#include <Arduino.h>
#include <SPI.h>


#include "Tracker_Definitions.h"
#include "LoRa.h"

#include "Test_Parameters.h"
#include "binary.h"


const byte startup_Power = 10;                    //power for startup tone

unsigned int lora_Test1Count[20];	          //buffer where counts of received packets are stored
unsigned int lora_Test2Count[20];	          //buffer where counts of received packets are stored

int SupplyVolts;
String results[5];

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

    digitalWrite(PLED1, HIGH);
    Serial.print(F("RX "));
    lora_ReadPacket();
    lora_RXPKTInfo();                                //print the info on received packet
    Serial.print("  ");
    processPacket();
    lora_RXONLoRa1();                                 //ready for next
    digitalWrite(PLED1, LOW);
  }

  if (lora_Ltemp == 96)
  {
    Serial.println();
    Serial.println(F("CRC Error"));
    lora_RXONLoRa1();                                 //ready for next
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

    lora_RXBUFFPrint(3);                             //print as ASCII
    Serial.print(F(" ("));
    Serial.print(lTXpower);
    Serial.print(F("dBm) "));
    Serial.println();
  }

  if (lora_RXPacketType == LMLPayload)
  {
    lora_RXBUFFPrint(3);                              //print as ASCII
    FillPayloadArray(lora_RXStart, lora_RXEnd);
    lTXpower = results[3].toInt();                    //read the power field
    Serial.println();
  }
  
  
  if (lora_RXPacketType == ShortBinaryPayload)
  {
    tempfloat = readFloat(lora_RXBUFF, 0);
    Serial.print(tempfloat,5);
    Serial.print(F("  "));
    tempfloat = readFloat(lora_RXBUFF, 4);
    Serial.print(tempfloat,5);
    Serial.print(F("  "));
    tempuint = readUint(lora_RXBUFF, 8);
    Serial.print(tempuint);
    Serial.print(F("  "));
    tempbyte = readByte(lora_RXBUFF, 10);
    Serial.print(tempbyte);
    Serial.println();
  }

  if (lora_RXPacketType == TimePacket)
  {
    i = readUint(lora_RXBUFF, 0);
    Serial.print("Up Seconds ");
    Serial.println(i);
  }

  if (lora_RXPacketType == TestMode1)
  {
    //this is a command to switch to TestMode1
    Serial.println();
    Serial.println();
    Serial.println(F("Switch Mode1"));
    Serial.println();
    init_RXLoRaTest1();
    lora_RXONLoRa1();
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
    lora_RXONLoRa2();
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
    digitalWrite(PLED1, HIGH);
    delay(100);
    digitalWrite(PLED1, LOW);
    delay(100);
  }
}


void led_FlashStart()
{
  byte i;
  for (i = 0; i <= 4; i++)
  {
    digitalWrite(PLED1, HIGH);
    delay(100);
    digitalWrite(PLED1, LOW);
    delay(100);
  }
}


void setup()
{
  pinMode(PLED1, OUTPUT);		         //setup pin for PCB LED
  led_FlashStart();

  Serial.begin(38400);                            //setup Serial console ouput
  Serial.println(F(programname));
  Serial.println(F(programversion));
  Serial.println(F(dateproduced));
  Serial.println(F(aurthorname));
  Serial.println();

  Serial.println();
  pinMode(lora_PReset, OUTPUT);		         //setup pin for LoRa device reset line
  digitalWrite(lora_PReset, HIGH);
  pinMode (lora_PNSS, OUTPUT);		         //setup pin for LoRa device slave select
  digitalWrite(lora_PNSS, HIGH);

  SPI.begin();				         //initialize SPI
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setDataMode(SPI_MODE0);
  SPI.setBitOrder(MSBFIRST);

  if (lora_CheckDevice() == true)
  {
    init_RXLoRaTest1();				                  //Do the initial LoRa Setup
    Serial.println(F("LoRa Tone")); 
    digitalWrite(PLED1, HIGH);
    lora_Tone(1000, 3000, startup_Power);        //Transmit an FM tone, 1000hz, 3000ms
    digitalWrite(PLED1, LOW); 
    Serial.println();
  }
  else
  {
    Serial.println(F("LoRa Device Error"));
    systemerror();
  }

  init_RXLoRaTest1();                            //do the initial LoRa Setup for testmode 1
  lora_RXONLoRa1();

}


