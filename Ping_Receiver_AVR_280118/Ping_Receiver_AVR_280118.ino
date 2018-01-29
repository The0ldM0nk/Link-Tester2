//Ping_Receiver_AVR_280118.ino
#define programname "Ping_Receiver_AVR_280118"
#define programversion "V1.0"
#define dateproduced "28/01/18"
#define aurthorname "Stuart Robinson"
#include <Arduino.h>
#define LoRa_Device_in_MB1                           //required if board is using Mikrobus sockets

/*
*****************************************************************************************************************************
  LoRaTracker Test Programs

  Copyright of the author Stuart Robinson - 28/01/18

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

  

  Do not forget to fit an antenna to the LoRa device, you can destroy it if you use it without and antenna
********************************************************************************************************************************
*/

/*
********************************************************************************************************************************
  Connections

  The program uses the hardware SPI interface on the Arduino to connect to the LoRa device, so the SPI SCK, MOSI and MISO pins are
  assumed to be connected. The test program needs pins connected to act as chip select, read the LoRa DIO0 pin and to generate the tones.
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

  Select (include) the board definition file you require by removing the // characters before the appropriate include line in the list below.
  Soem of the boards below do not have a pin for the buzzer connection, so in these cases the GPSTX pin (A2) is used since its next to a gnd
  pin making it easy to connect the buzzer.

***********************************************************************************************************************************************
*/

//#include "HAB2_Board_Definitions.h"
#include "Locator2_Board_Definitions.h"
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
//#include "Heltec_ESP32_WiFi_LoRa_Board_Definitions.h"

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


String results[5];
unsigned long Packet_Count = 0;
byte keypress;

#include "Test_Settings.h"
#include <SPI.h>
#include "LoRa_Test.h"
#include "Binary_Test.h"

#include <Wire.h>
#include "SSD1306Ascii.h"                            //https://github.com/greiman/SSD1306Ascii
#include "SSD1306AsciiAvrI2c.h"
SSD1306AsciiAvrI2c disp;

#define I2C_ADDRESS 0x3C                             //address will be 0x3C or 0x3D

#define Serial_Monitor_Baud 115200                   //this is baud rate used for the Arduino IDE Serial Monitor


void loop()
{
#ifdef RSSI_Meter
  byte i;
  int noise_level = 0;
  for (i = 1; i <= 100; i++)
  {
    lora_BackGroundRSSI = lora_returnRSSI(lora_Read(lora_RegRssiValue));    //get the background noise level
    noise_level = noise_level + lora_BackGroundRSSI;
  }

  noise_level = noise_level / 100;

  disp.set2X();
  disp.setCursor(30, 2);
  disp.print(noise_level);
  disp.print("dBm  ");
  
#endif

  checkforpacket();
}


void checkforpacket()
{
  //check if a packet has been received
  byte lora_Ltemp;

#ifdef USEDIO_RXReady
  lora_Ltemp = lora_readRXready2();                  //this reads the DIO0 pin for RX Ready, creates less EMI than polling the LoRa device registers.
#else
  lora_Ltemp = lora_readRXready();                   //polling the LoRa device registers for RX Ready
#endif

  if (lora_Ltemp == 64)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(BUZZ, HIGH);
    lora_RXOFF();                                    //stop more packets comming in
    Serial.print(F("RX "));
    lora_ReadPacket();
    lora_ReceptionInfo();
    Serial.print("  ");
    lora_AddressInfo();
    Serial.print("  ");
    processPacket();
    delay(alert_lengthms);
    lora_RXONLoRa();                                 //ready for next
    digitalWrite(LED1, LOW);
    digitalWrite(BUZZ, LOW);
  }

  if (lora_Ltemp == 96)
  {
    digitalWrite(LED1, HIGH);
    digitalWrite(BUZZ, HIGH);
    delay(alert_lengthms);
    digitalWrite(LED1, LOW);
    digitalWrite(BUZZ, LOW);
    delay(400);
    digitalWrite(LED1, HIGH);
    digitalWrite(BUZZ, HIGH);
    delay(alert_lengthms);
    digitalWrite(LED1, LOW);
    digitalWrite(BUZZ, LOW);
    Serial.println(F("CRC Error"));
    lora_RXONLoRa();                                 //ready for next
  }
}


void processPacket()
{
  byte lTXpower;
  unsigned int tempuint;
  float tempfloat;
  byte tempbyte;

  if (lora_RXPacketType == Testpacket)
  {
    lTXpower = ((lora_RXBUFF[0] - 48) * 10) +  (lora_RXBUFF[1] - 48);

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

  lora_RXONLoRa();
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


void lora_Custom1()
{
  //used to have a custom set of LoRa register settings for each test mode
}


void init_RXLoRaTest1()
{
  //setup for testmode
  lora_Setup1();
  lora_SetFreq(Frequency1, CalibrationOffset);
  lora_SetModem2(test1_Bandwidth, test1_SpreadingFactor, test1_CodeRate, Explicit);  //setup the LoRa modem parameters for test
}


void setup()
{
  pinMode(LED1, OUTPUT);		                     //setup pin for PCB LED
  pinMode(BUZZ, OUTPUT);                         //setup pin for buzzer
  pinMode(lora_DIO0, INPUT);                     //setup pin for reading RX ready

  led_FlashStart();

  Serial.begin(Serial_Monitor_Baud);             //setup Serial console ouput
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

  SPI.begin();				                           //initialize SPI
  SPI.beginTransaction(SPISettings(16000000, MSBFIRST, SPI_MODE0));

  if (lora_CheckDevice() == true)
  {
    Serial.println(F("LoRa Device Found"));

#ifdef EnableStartupTone
    init_RXLoRaTest1();				                   //Do the initial LoRa Setup
    Serial.println(F("LoRa Tone"));
    digitalWrite(LED1, HIGH);
    lora_Tone(1000, 3000, 2);                   //Transmit an FM tone, 1000hz, 3000ms
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
  
  Wire.begin();
  disp.begin(&Adafruit128x64, I2C_ADDRESS);
  disp.setFont(System5x7);


}


