//Obsługa DS1820
//Plik ds.cpp

#include "ds.h"

//--------------------------------------------------
//Zainicjuj konwersję temperatury
uint8_t dsConvertT()
{
  if (!oneWireReset()) return 0;

  oneWireWriteByte(0xcc); //Wybierz wszystkie
  oneWireWriteByte(0x44); //Konwertuj

  return -1;
}

//--------------------------------------------------
//Zainicjuj konwersję temperatury
int16_t dsRead()
{
  uint16_t i, tmp;

  if (!oneWireReset()) return 0;

  oneWireWriteByte(0xcc); //Wybierz wszystkie
  oneWireWriteByte(0xbe); //Odczytaj

  i = oneWireReadByte();	//Odczytaj piewszy bajt (wartość)
  tmp = oneWireReadByte();	//Odczytaj drugi bajt (znak)


  //Konwersja temperatury
  i *= 10;
  i /= 2;
  if(tmp != 0)
  	  i *= -1;
//
  return i;
}
