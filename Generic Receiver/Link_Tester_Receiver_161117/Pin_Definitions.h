//Pin_Definitions.h

/*
*******************************************************************************************************************************
  Easy Build LoRaTracker Programs for Arduino

  Copyright of the author Stuart Robinson - 17/10/17

  http://www.LoRaTracker.uk

  These programs may be used free of charge for personal, recreational and educational purposes only.

  This program, or parts of it, may not be used for or in connection with any commercial purpose without the explicit permission
  of the author Stuart Robinson.

  The programs are supplied as is, it is up to individual to decide if the programs are suitable for the intended purpose and
  free from errors.

  To Do:

*******************************************************************************************************************************
*/
/*
******************************************************************************************************
  Definitions for pins used to connect to the LoRa device in this program
******************************************************************************************************
*/


/*
#define lora_NSS 10                   //pin number where the chip select for the LoRa device is connected
#define lora_NReset 9                 //pin where LoRa device reset line is connected
#define lora_TonePin 6                //pin number for radio tone generation, connects to LoRa device pin DIO2
#define lora_DIO0 2                   //pin connected to DIO0 on LoRa device
#define LED1 8                        //pin number for LED 
*/


#define lora_TonePin 3               //pin number for radio tone generation, connects to LoRa device pin DIO2
#define Audio_Out 6                  //pin number for Audio tone generation (AFSK) may be same as LoRa tone pin
#define LED1 8                       //pin number for LED on Tracker
#define lora_NReset 9                //pin where LoRa device reset line is connected
#define lora_NSS 10                  //pin number where the NSS line for the LoRa device is connected
#define lora_DIO0 -1                 //not used pin
#define SupplyAD  -1                 //measure supply voltage here
#define ADMultiplier 10              //multiplier for supply volts calculation
