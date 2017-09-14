//Display.h
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

Normally used with Nokia 5110 display and Arduino backapack, this display is 6 lines of 14 characters.

To do;

******************************************************************************************************
*/

const byte DisplayDelay = 0;               //delay to allow display backpack time to complete command
const byte ClearDelay = 0;                 //delay to allow display backpack time to clear screen
const byte contrast = 63;                  //Contrast value for this LCD, varies between modules, range noramlly 35-65, not needed for Digole displays
const byte endcommand = 0;                 //this character value ends a text write. This may be 13 for older digole display modules


void display_Cls()
{
  //clears display
  ss.print("CL");                           //print text
}


void display_Setfont(byte lfont)
{
  //sets the current font number
  ss.print("SF");                           //print text
  ss.write(char(lfont));                    //print data
  delay(DisplayDelay);
}


void display_Text(String Ltext)
{
  //adds text at cursior position
  ss.print("TT");                            //print text
  ss.print(Ltext);                           //print text
  ss.write(endcommand);
  delay(DisplayDelay);
}


void display_SetCurPos(byte lcol, byte lrow)
{
  //Sets the current cursor position
  ss.print("TP");                            //print text
  ss.write(char(lcol));                      //print data
  ss.write(char(lrow));                      //print data
  delay(DisplayDelay);
}


void display_SetContrast(byte lcont)
{
  //Sets the display contrast
  ss.print("CT");                             //print text
  ss.write(char(lcont));                      //print data
  delay(DisplayDelay);
}


void display_Printint(int Ltemp)
{
  //prints an integer at current cursor position
  ss.print("TT");                             //print text
  ss.print(Ltemp);                            //print number
  ss.write(endcommand);
  delay(DisplayDelay);
}


void display_Printfloat(float Ltemp, byte Lnumdecimals)
{
  //prints a float at current cursor position
  ss.print("TT");                            //print text
  ss.print(Ltemp, Lnumdecimals);             //print number
  ss.write(endcommand);
  delay(DisplayDelay);
}


void display_ClearAndHome(byte lfont)
{
  //clears display, sets the current font number and homes the cursor
  display_SetContrast(contrast);
  display_Cls();
  delay(ClearDelay);
  display_Setfont(lfont);
  display_SetCurPos(0, 0);
  delay(DisplayDelay);
}
