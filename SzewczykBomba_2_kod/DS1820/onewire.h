//Interface 1-wire
//Plik onewire.h

#include <avr/io.h>
#include <util/delay.h>

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

//Interface realizowany na PC0
#define SET_ONEWIRE_PORT     PORTC  |=  _BV(0)
#define CLR_ONEWIRE_PORT     PORTC  &= ~_BV(0)
#define IS_SET_ONEWIRE_PIN   PINC   &   _BV(0)
#define SET_OUT_ONEWIRE_DDR  DDRC   |=  _BV(0)
#define SET_IN_ONEWIRE_DDR   DDRC   &= ~_BV(0)

//Sprawdź stan magistrali
uint8_t oneWireReset();

//Zapisz bajt
void oneWireWriteByte(uint8_t byte);

//Odczytaj bajt
uint8_t oneWireReadByte();

#endif
