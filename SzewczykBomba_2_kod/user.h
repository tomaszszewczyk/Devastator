//Projekt	-Devastator-
//Plik 		user.h
//Autor		wieloiksiasty
//Obsługa użytkownika

#include <avr/io.h>
#include <util/delay.h>

#ifndef USER_H_
#define USER_H_

//Przydatne makra
#define set(reg, num) ((reg) |= (1 << (num)))
#define clear(reg, num) ((reg) &= ~(1 << (num)))

#define on(reg, num) ((reg) &= ~(1 << (num)))
#define off(reg, num) ((reg) |= (1 << (num)))

//--------------------------------------------------
//Wyświetlacz LED

//Definicje segmentów
#define LEDA_DDR	DDRB
#define LEDA_PORT	PORTB
#define LEDA_NUM	PB0

#define LEDB_DDR	DDRD
#define LEDB_PORT	PORTD
#define LEDB_NUM	PD7

#define LEDC_DDR	DDRD
#define LEDC_PORT	PORTD
#define LEDC_NUM	PD4

#define LEDD_DDR	DDRB
#define LEDD_PORT	PORTB
#define LEDD_NUM	PB6

#define LEDE_DDR	DDRB
#define LEDE_PORT	PORTB
#define LEDE_NUM	PB7

#define LEDF_DDR	DDRD
#define LEDF_PORT	PORTD
#define LEDF_NUM	PD6

#define LEDG_DDR	DDRD
#define LEDG_PORT	PORTD
#define LEDG_NUM	PD5

#define LEDDT_DDR	DDRD	//Kropka
#define LEDDT_PORT	PORTD
#define LEDDT_NUM	PD3

//Definicje wyprowadzeń rejestru przesuwnego
#define DSP_DATA_DDR	DDRD	//Data
#define DSP_DATA_PORT	PORTD
#define DSP_DATA_NUM	PD1

#define DSP_CLK_DDR		DDRD	//Zegar
#define DSP_CLK_PORT	PORTD
#define DSP_CLK_NUM		PD0

//Inicjalizacja wyświetlacza
void ledInit();

//Obsługa wyświetlacza
void ledProcess(uint8_t content[], uint8_t dot);

//--------------------------------------------------
//Kropka
#define KROPKA_DDR	DDRC
#define KROPKA_PORT	PORTC
#define KROPKA_NUM	PC3

//Inicjalizacja kropki
void kropkaInit();

//Zaświeć/gaś kropkę
void kropkaOn();
void kropkaOff();


//--------------------------------------------------
//Przyciski

//Definicje
#define DB_TIME 10	//Stała filtracji drgań
					//Więcej dłużej/dokładniej
					//Mniej szybciej/bardziej pobieżnie

#define SW1_DDR		DDRB
#define SW1_PORT 	PORTB
#define SW1_PIN		PINB
#define SW1_NUM		PB4

#define SW2_DDR		DDRB
#define SW2_PORT 	PORTB
#define SW2_PIN		PINB
#define SW2_NUM		PB3

#define SW3_DDR		DDRB
#define SW3_PORT 	PORTB
#define SW3_PIN		PINB
#define SW3_NUM		PB2

#define SW4_DDR		DDRB
#define SW4_PORT 	PORTB
#define SW4_PIN		PINB
#define SW4_NUM		PB5

//Przydatne makro
#define readPin(pin, num) (~pin & (1<<num))

//Inicjalizacja przycisków
void swInit();

//Obsługa przycisków
//Zwraca wciśnięty przycisk
uint8_t swProcess();

//--------------------------------------------------
//Buzzer

#define BUZZER_DDR	DDRB
#define BUZZER_PORT	PORTB
#define BUZZER_NUM	PB1

//Inicjalizacja buzzera
inline void buzzerInit() {set(BUZZER_DDR, BUZZER_NUM);}

//Włącz/wyłącz buzzer
inline void buzzerOn() {set(BUZZER_PORT, BUZZER_NUM);}
inline void  buzzerOff() {clear(BUZZER_PORT, BUZZER_NUM);}

#endif /* USER_H_ */
