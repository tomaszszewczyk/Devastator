//Obsługa DS1820
//Plik ds.h

#include <avr/io.h>
#include <util/delay.h>
#include "onewire.h"

#ifndef DS18B20_H
#define DS18B20_H

//Zainicjuj konwersję temperatury
uint8_t dsConvertT();

//Odczytaj temperaturę z czujnika
int16_t dsRead();

#endif
