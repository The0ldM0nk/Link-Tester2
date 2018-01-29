//Test_Settings.h
/*
******************************************************************************************************

LoRaTracker Programs for Arduino

Copyright of the author Stuart Robinson - 28/01/18

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
#define RSSI_Meter                                //enable this define if you want to use the RSSI meter function, needs connected SD1306 display

#define CalibrationOffset 0                       //adjustment for frequency in Hz, assumed at room temp
#define Payload_buffer 128
#define Output_len_max 128

#define SendTestpacket                            //Enable send of test packet
//#define SendLocationCSVPacket                   //Enable send of ASCII CSV short location payload
//#define SendBinaryLocation                      //Enable send of binary short location payload
//#define SendHABPacket                           //Enable send of HAB Style packet
#define TransmittTestTone                         //Enable transmitt of test tone 
//#define EnableStartupTone                       //Enable a frequency calibration tone at startup, do not use if LNS connected !
#define USEDIO_RXReady                            //if there is access to the LoRa device DIO pin you can enable this option.       

const byte transmit_power = 2;                    //power for transmissions

#define lora_RXBUFF_Size 128
#define lora_TXBUFF_Size 128

//*******  Setup Test Parameters Here ! ***************

//LoRa Modem Parameters
const unsigned long Frequency1 = 434400000;       //mode1 frequency
#define test1_Bandwidth BW62500                   //LoRa bandwidth
#define test1_SpreadingFactor SF8                 //LoRa spreading factor
#define test1_CodeRate CR45                       //LoRa coding rate

const byte Deviation = 0x52;                      //typical deviation for tones

const unsigned int transmission_delaymS = 1000;   //mS delay between transmissions
const unsigned int tone_lengthmS = 2000;          //tone length in mS
const unsigned int tone_frequencyHz = 1000;       //tone frequency in hertz
const unsigned int alert_lengthms = 250;          //length of alert tones in mS

//GPS co-ordinates to use for the test location transmission
const float TestLatitude  = 51.48230;             //GPS co-ordinates to use for test
const float TestLongitude  = -3.18136;            //Cardiff castle keep, used for testing purposes 
const float TestAltitude = 25.5;

unsigned char Flight_ID[15] = "LoRaTracker1";     //used for the HAB style   

const byte ThisNode = '1';                        //node number goes out as part of packet addressing 

const byte config_byte = 0;                       //goes out with some packets
byte PayloadArraySize = 6;                        //used for setting size of buffer to read CSV packets

//********************************************************


