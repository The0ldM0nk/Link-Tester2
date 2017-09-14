/*
**************************************************************************************************

Easy Build LoRaTracker Programs for Arduino ATMEGA328

Copyright of the author Stuart Robinson - 16/07/2016

HTTP://WWW.LORATRACKER.UK

These programs may be used free of charge for personal, recreational and educational purposes only.

This program, or parts of it, may not be used for or in connection with any commercial purpose without
the explicit permission of the author Stuart Robinson.

The programs are supplied as is, it is up to individual to decide if the programs are suitable for the
intended purpose and free from errors.

To do;

******************************************************************************************************
*/


#define lora_RXBUFF_Size 128
#define lora_TXBUFF_Size 128

//*******  Setup TX Test Parameters Here ! ***************

//LoRa Modem Parameters
const byte test1_Bandwidth = BW62500;              //LoRa bandwidth
const byte test1_SpreadFactor = SF8;              //LoRa spreading factor
const byte test1_CodeRate = CR45;                //LoRa coding rate
//const byte test1_RateOptimisation = lora_LowDoptOFF;   //LoRa data rate optimisation


//LoRa Modem Parameters
const byte test2_Bandwidth = BW50000;              //LoRa bandwidth
const byte test2_SpreadFactor = SF7;              //LoRa spreading factor
const byte test2_CodeRate = CR45;                //LoRa coding rate
//const byte test2_RateOptimisation = lora_LowDoptOFF;   //LoRa data rate optimisation

//GPS co-ordinates to use for transmission
#define Latitude  "51.48230"                           //GPS co-ordinates to use for test
#define Longitude  "-3.18136"
#define Altitude "25"

const byte start_power = 17;                           //Start power for transmissions
const byte end_power = 2;                              //End power for transmissions
const byte ThisNode = '1';

//#define TXLMLpayload                                 //enable to send the LML lat,lon,alt payload
#define TXTestpacket                                   //enable to send the test packet

#define Frequency1 434.400                             //Test1 frequency
#define Frequency2 434.400                             //Test2 frequency
//********************************************************

byte modenumber;



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
  lora_SetModem(test1_Bandwidth, test1_SpreadFactor, test1_CodeRate, lora_Explicit, test1_RateOptimisation);	//setup the LoRa modem parameters for test
}


void init_RXLoRaTest2()
{
  //setup for testmode
  modenumber = 2;
  lora_Setup1();
  lora_SetFreq(Frequency2, FreqOffset);
  lora_SetModem(test2_Bandwidth, test2_SpreadFactor, test2_CodeRate, lora_Explicit, test2_RateOptimisation);	//setup the LoRa modem parameters for test
}
