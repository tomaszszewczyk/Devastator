//Interface 1-wire
//Plik onewire.cpp

#include "onewire.h"

//--------------------------------------------------
//Sprawdź stan magistrali
uint8_t oneWireReset()
{
  CLR_ONEWIRE_PORT;

  if (!(IS_SET_ONEWIRE_PIN)) return 0;

  SET_OUT_ONEWIRE_DDR;
  _delay_us(500);
  SET_IN_ONEWIRE_DDR;
  _delay_us(70);

  if(!(IS_SET_ONEWIRE_PIN))
  {
    _delay_us(500);
    return(1);
  }

  _delay_us(500);

return(0);
}

//--------------------------------------------------
//Zapisz bajt
void oneWireWriteByte(uint8_t byte)
{
	uint8_t i;

   CLR_ONEWIRE_PORT;

   for (i=0; i<8; i++)
   {
     SET_OUT_ONEWIRE_DDR;

     if (byte & 0x01)
     {
       _delay_us(7);
       SET_IN_ONEWIRE_DDR;
       _delay_us(70);
     }
     else
     {
        _delay_us(70);
        SET_IN_ONEWIRE_DDR;
        _delay_us(7);
     }

     byte >>= 1;
   }
}

//--------------------------------------------------
//Odczytaj bajt
uint8_t oneWireReadByte()
{
	uint8_t i, byte = 0;

  SET_IN_ONEWIRE_DDR;

  for (i=0; i<8; i++)
  {
     SET_OUT_ONEWIRE_DDR;
     _delay_us(7);
     SET_IN_ONEWIRE_DDR;
     _delay_us(7);
     byte >>= 1;

     if(IS_SET_ONEWIRE_PIN) byte |= 0x80;

     _delay_us(70);
  }

  return byte;
}
