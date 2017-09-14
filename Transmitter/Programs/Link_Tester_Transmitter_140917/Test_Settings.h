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

#define CalibrationOffset 0                       //adjustment for frequency in Hz, assumed at room temp
#define Payload_buffer 128
#define Output_len_max 128

#define SendTestpacket                            //Enable send of test packet
//#define SendLocationCSVPacket                   //Enable send of ASCII CSV short location payload
#define SendBinaryLocation                        //Enable send of binary short location payload
//#define SendHABPacket                           //Enable send of HAB Style packet

#define EnableMode1                               //Enable send of mode 1 packets
#define EnableMode2                               //Enable send of mode 2 packets

#define lora_RXBUFF_Size 128
#define lora_TXBUFF_Size 128

//*******  Setup Test Parameters Here ! ***************

//LoRa Modem Parameters mode1 - 1562bps
const unsigned long Frequency1 = 434400000;     //mode1 frequency
#define test1_Bandwidth BW62500                 //LoRa bandwidth
#define test1_SpreadingFactor SF8               //LoRa spreading factor
#define test1_CodeRate CR45                     //LoRa coding rate

const byte start_power = 17;                    //Start power for transmissions
const byte end_power = 2;                       //End power for transmissions

//LoRa Modem Parameters mode2 - 21875bps
const unsigned long Frequency2 = 434400000;     //mode2 frequency
#define test2_Bandwidth BW500000                //LoRa bandwidth
#define test2_SpreadingFactor SF7               //LoRa spreading factor
#define test2_CodeRate CR45                     //LoRa coding rate

const byte Deviation = 0x52;    //typical deviation for tones

//GPS co-ordinates to use for the test location transmission
//const float TestLatitude  = 51.48230;            //GPS co-ordinates to use for test
//const float TestLongitude  = -3.18136;           //Cardiff castle keep, used for testing purposes 
//const float TestAltitude = 25.5;

const float TestLatitude  = 51.511781;            //GPS co-ordinates to use for test
const float TestLongitude  = -3.191054;           //Heath Park, bottom of woods 
const unsigned int TestAltitude = 199;


unsigned char Flight_ID[15] = "LoRaTracker";

unsigned int LoopStartTone_lengthmS = 250;       //length of LoopStartTone in mS
unsigned int Mode1StartDelaymS = 1000;           //delay in mS after sending mode change at start of loop, gives RX time to print packet totals.
const unsigned int mode_delaymS = 1000;          //mS delay between modes
const unsigned int packet_delay = 1200;          //mS delay between packets

const byte ThisNode = '1';                       //node number goes out as part of packet addressing 

byte config_byte = 0;                            //goes out with  
byte lora_TestPower = 10;                        //is also start up tone power
char ramv_ThisNode;
byte modenumber;
byte PayloadArraySize = 6;

//********************************************************


