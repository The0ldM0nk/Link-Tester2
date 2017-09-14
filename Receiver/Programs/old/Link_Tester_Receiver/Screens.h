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


void writeSNR(byte lcol, byte lrow)
{
  byte lvar1;
  String lstring;
  if (lora_PacketSNR > 127)                 //calculate the SNR
  {
    lvar1 = ((255 - lora_PacketSNR) / 4);
    lstring = "-";
  }
  else
  {
    lvar1 = (lora_PacketSNR / 4);
    lstring = "+";
  }

  display_Setfont(defaultfont);
  display_SetCurPos(lcol, lrow);
  display_Text("SNR ");
  display_Text(lstring);                    //now print the SNR
  display_Printint(lvar1);
  display_Text("dB ");
}


void writePktCount(byte lcol, byte lrow)
{
  display_Setfont(defaultfont);
  display_SetCurPos(lcol, lrow);            //cursor to last two cols of bottom line
  display_Text("Pkts ");
  display_Printint(lora_RXpacketCount);     //send count to LCD

}


void writeRSSI(byte lcol, byte lrow)
{
  byte lvar1;
  lvar1 = 137 - lora_PacketRSSI;
  display_Setfont(defaultfont);
  display_SetCurPos(lcol, lrow);
  display_Text("RSSI  -");
  display_Printint(lvar1);
  display_Text("dB ");
}


void writeBackGroundRSSI(byte lcol, byte lrow)
{
  byte lvar1;
  lvar1 = 137 - lora_BackGroundRSSI;
  display_Setfont(defaultfont);
  display_SetCurPos(lcol, lrow);
  display_Text("Noise -");
  display_Printint(lvar1);
  display_Text("dB ");
}


void updatescreenT()
{
  //prints the test packet
  byte i;
  i = lora_RXBUFF[0];   //get the power byte
  display_Cls();
  display_SetCurPos(0, 2);
  display_Text("Test Packet");
  display_SetCurPos(0, 3);
  display_Printint(i);
  display_Text("dBm");
  writeSNR(0, 4);
  writePktCount(7, 4);
}


void updatescreen0()
{
  //prints the test packet
  byte i;
  display_Cls();
  writeRSSI(0, 0);
  delay(10);
  writeBackGroundRSSI(0, 1);
  delay(10);
  writeSNR(0, 3);
  delay(10);
  writePktCount(0, 5);
  delay(10);
}


void updatescreenLML(int lTXpower)
{
  //prints the test packet, print power if > 0
  int lTXPower;
  display_Cls();
  display_SetCurPos(0, 0);
  display_Text("LML Packet");
  display_SetCurPos(0, 3);
  display_Printint(lTXPower);
  display_Text("dBm");
  writeSNR(0, 4);
  writePktCount(7, 4);
  display_SetCurPos(0, 5);

  if (lTXpower > 0)
  {
    display_Text("Power ");
    display_Printint(lTXpower);
    display_Text("dBm");
  }
}







