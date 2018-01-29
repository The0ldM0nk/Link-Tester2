//Test_Settings.h

/*
******************************************************************************************************

LoRaTracker Programs for Arduino

Copyright of the author Stuart Robinson - 14/09/2017

http://www.LoRaTracker.uk

These programs may be used free of charge for personal, recreational and educational purposes only.

This program, or parts of it, may not be used for or in connection with any commercial purpose without
the explicit permission of the author Stuart Robinson.

The programs are supplied as is, it is up to individual to decide if the programs are suitable for the
intended purpose and free from errors.

This test settinfs file is identical to the transmitter settings file

To Do:

******************************************************************************************************
*/

#define DEBUG
//#define LORADEBUG

#define LORA_AFC

#define CalibrationOffset -0                      //adjustment for frequency in Hz, assumed at room temp
#define Payload_buffer 128
#define Output_len_max 128

#define SendTestpacket                            //Enable send of test packet
//#define SendLocationCSVPacket                     //Enable send of ASCII CSV short location payload
//#define SendBinaryLocation                        //Enable send of binary short location payload
//#define SendHABPacket                             //Enable send of HAB Style packet

#define EnableMode1                               //Enable send of mode 1 packets
//#define EnableMode2                               //Enable send of mode 2 packets
#define EnableTone                              //Enable ident tone at startup, DO NOT ENABLE IF USING LNA ON RECEIVER
#define lora_RXBUFF_Size 128
#define lora_TXBUFF_Size 128

//*******  Setup Test Parameters Here ! ***************

//LoRa Modem Parameters mode1
const unsigned long Frequency1 = 434400000;     //mode1 frequency
#define test1_Bandwidth BW125000                  //LoRa bandwidth
#define test1_SpreadingFactor SF7               //LoRa spreading factor
#define test1_CodeRate CR45                     //LoRa coding rate

const byte start_power = 17;                    //Start power for transmissions
const byte end_power = 2;                       //End power for transmissions

//LoRa Modem Parameters mode2
const unsigned long Frequency2 = 434400000;     //mode2 frequency
#define test2_Bandwidth BW7800                  //LoRa bandwidth
#define test2_SpreadingFactor SF8              //LoRa spreading factor
#define test2_CodeRate CR45                     //LoRa coding rate

const byte Deviation = 0x52;    //typical deviation for tones

//GPS co-ordinates to use for the test location transmission
const float TestLatitude  = 51.48230;            //GPS co-ordinates to use for test
const float TestLongitude  = -3.18136;           //Cardiff castle keep, used for testing purposes 
const float TestAltitude = 25.5;


unsigned char Flight_ID[15] = "LoRaTracker1";    //used for the HAB style   

unsigned int LoopStartTone_lengthmS = 250;       //length of LoopStartTone in mS
unsigned int Mode1StartDelaymS = 500;            //delay in mS after sending mode change at start of loop, gives RX time to print packet totals.
const unsigned int mode_delaymS = 500;           //mS delay between modes
const unsigned int packet_delay = 500;           //mS delay between packets

const byte ThisNode = '1';                       //node number goes out as part of packet addressing 

const byte config_byte = 0;                      //goes out with some packets

byte PayloadArraySize = 6;
//********************************************************


