
float readFloat(char temp[], byte addr)
{
  int i;
  byte j;

  union
  {
    byte b[4];
    float f;
  } readdata;

  for (int i = 0; i < 4; i++)
  {
    j = temp[addr + i];
    readdata.b[i] = j;
  }
  return readdata.f;
}


byte readByte(char temp[], byte addr)
{
  return temp[addr];
}


unsigned int readUint(char temp[], byte addr)
{
  byte llowbyte, lhighbyte;
  llowbyte = temp[addr];
  lhighbyte = temp[addr + 1];
  return (llowbyte + (lhighbyte * 256));
}
