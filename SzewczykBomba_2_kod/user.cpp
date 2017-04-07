//Projekt	-Devastator-
//Plik 		user.cpp
//Autor		wieloiksiasty
//Obsługa użytkownika

#include "user.h"

//--------------------------------------------------
//Wyświetlacz LED

//Inicjalizacja wyświetlacza LED
void ledInit()
{
	//Segmenty
	//Kierunek
	set(LEDA_DDR, LEDA_NUM);
	set(LEDB_DDR, LEDB_NUM);
	set(LEDC_DDR, LEDC_NUM);
	set(LEDD_DDR, LEDD_NUM);
	set(LEDE_DDR, LEDE_NUM);
	set(LEDF_DDR, LEDF_NUM);
	set(LEDG_DDR, LEDG_NUM);
	set(LEDDT_DDR, LEDDT_NUM);

	//Stan początkowy
	set(LEDA_PORT, LEDA_NUM);
	set(LEDB_PORT, LEDB_NUM);
	set(LEDC_PORT, LEDC_NUM);
	set(LEDD_PORT, LEDD_NUM);
	set(LEDE_PORT, LEDE_NUM);
	set(LEDF_PORT, LEDF_NUM);
	set(LEDG_PORT, LEDG_NUM);
	set(LEDDT_PORT, LEDDT_NUM);

	//Rejestr przesuwny
	set(DSP_DATA_DDR, DSP_DATA_NUM);
	set(DSP_CLK_DDR, DSP_CLK_NUM);

	clear(DSP_DATA_PORT, DSP_DATA_NUM);
	set(DSP_DATA_PORT, DSP_DATA_NUM);

	//Zerowanie wyjść rejestru
	for(uint8_t i = 0; i < 8; i++)
	{
		set(DSP_CLK_PORT, DSP_CLK_NUM);
		clear(DSP_CLK_PORT, DSP_CLK_NUM);
	}
}

//Obsługa wyświetlacza LED
void ledProcess(uint8_t content[], uint8_t dot)
{
	volatile static uint8_t current = 5;

	//Zgaszenie wyświetlacza w celu uniknięcia poświaty
	off(LEDA_PORT, LEDA_NUM);
	off(LEDB_PORT, LEDB_NUM);
	off(LEDC_PORT, LEDC_NUM);
	off(LEDD_PORT, LEDD_NUM);
	off(LEDE_PORT, LEDE_NUM);
	off(LEDF_PORT, LEDF_NUM);
	off(LEDG_PORT, LEDG_NUM);
	off(LEDDT_PORT, LEDDT_NUM);

	//Wystarczy przejść na następny
	if(current < 5)
	{
		set(DSP_DATA_PORT, DSP_DATA_NUM);

		set(DSP_CLK_PORT, DSP_CLK_NUM);
		clear(DSP_CLK_PORT, DSP_CLK_NUM);

		current++;
	}


	//Jeśli nie, wróć do początku
	else
	{
		clear(DSP_DATA_PORT, DSP_DATA_NUM);

		set(DSP_CLK_PORT, DSP_CLK_NUM);
		clear(DSP_CLK_PORT, DSP_CLK_NUM);

		current = 0;
	}

	//---------------------------
	//Wyświetlenie kolejnej cyfry
	if(dot == current)
		on(LEDDT_PORT, LEDDT_NUM);
	else
		off(LEDDT_PORT, LEDDT_NUM);

	if(content[current] == 1)
	{
		off(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		off(LEDD_PORT, LEDD_NUM);
		off(LEDE_PORT, LEDE_NUM);
		off(LEDF_PORT, LEDF_NUM);
		off(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 2)
	{
		on(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		off(LEDC_PORT, LEDC_NUM);
		on(LEDD_PORT, LEDD_NUM);
		on(LEDE_PORT, LEDE_NUM);
		off(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 3)
	{
		on(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		on(LEDD_PORT, LEDD_NUM);
		off(LEDE_PORT, LEDE_NUM);
		off(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 4)
	{
		off(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		off(LEDD_PORT, LEDD_NUM);
		off(LEDE_PORT, LEDE_NUM);
		on(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 5)
	{
		on(LEDA_PORT, LEDA_NUM);
		off(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		on(LEDD_PORT, LEDD_NUM);
		off(LEDE_PORT, LEDE_NUM);
		on(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 6)
	{
		on(LEDA_PORT, LEDA_NUM);
		off(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		on(LEDD_PORT, LEDD_NUM);
		on(LEDE_PORT, LEDE_NUM);
		on(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 7)
	{
		on(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		off(LEDD_PORT, LEDD_NUM);
		off(LEDE_PORT, LEDE_NUM);
		off(LEDF_PORT, LEDF_NUM);
		off(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 8)
	{
		on(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		on(LEDD_PORT, LEDD_NUM);
		on(LEDE_PORT, LEDE_NUM);
		on(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 9)
	{
		on(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		on(LEDD_PORT, LEDD_NUM);
		off(LEDE_PORT, LEDE_NUM);
		on(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == 0)
	{
		on(LEDA_PORT, LEDA_NUM);
		on(LEDB_PORT, LEDB_NUM);
		on(LEDC_PORT, LEDC_NUM);
		on(LEDD_PORT, LEDD_NUM);
		on(LEDE_PORT, LEDE_NUM);
		on(LEDF_PORT, LEDF_NUM);
		off(LEDG_PORT, LEDG_NUM);
	}
	if(content[current] == '-')
	{
		off(LEDA_PORT, LEDA_NUM);
		off(LEDB_PORT, LEDB_NUM);
		off(LEDC_PORT, LEDC_NUM);
		off(LEDD_PORT, LEDD_NUM);
		off(LEDE_PORT, LEDE_NUM);
		off(LEDF_PORT, LEDF_NUM);
		on(LEDG_PORT, LEDG_NUM);
	}
}

//--------------------------------------------------
//Kropka

//Inicjalizacja
void kropkaInit()
{
	set(KROPKA_DDR, KROPKA_NUM);
	set(KROPKA_PORT, KROPKA_NUM);
}

//Zaświeć
void kropkaOn()
{
	clear(KROPKA_PORT, KROPKA_NUM);
}

//Zgaś
void kropkaOff()
{
	set(KROPKA_PORT, KROPKA_NUM);
}

//--------------------------------------------------
//Przyciski

//Inicjalizacja
void swInit()
{
	//Ustaw odczyt
	clear(SW1_DDR, SW1_NUM);
	clear(SW2_DDR, SW2_NUM);
	clear(SW3_DDR, SW3_NUM);
	clear(SW4_DDR, SW4_NUM);

	//Włącz pull_up
	set(SW1_PORT, SW1_NUM);
	set(SW2_PORT, SW2_NUM);
	set(SW3_PORT, SW3_NUM);
	set(SW4_PORT, SW4_NUM);
}

//Obsługa
uint8_t swProcess()
{
	static uint8_t sw1, sw2, sw3, sw4;

	//Odczytaj i zrób deboucing
	if(readPin(SW1_PIN, SW1_NUM) != 0 && sw1 < DB_TIME) sw1++;
	if(readPin(SW1_PIN, SW1_NUM) == 0) sw1=0;

	if(readPin(SW2_PIN, SW2_NUM) != 0 && sw2 < DB_TIME) sw2++;
	if(readPin(SW2_PIN, SW2_NUM) == 0) sw2=0;

	if(readPin(SW3_PIN, SW3_NUM) != 0 && sw3 < DB_TIME) sw3++;
	if(readPin(SW3_PIN, SW3_NUM) == 0) sw3=0;

	if(readPin(SW4_PIN, SW4_NUM) != 0 && sw4 < DB_TIME) sw4++;
	if(readPin(SW4_PIN, SW4_NUM) == 0) sw4=0;

	//Jeśli przeszedł przez debouncing zwróć
	if(sw1 >= DB_TIME)
		return 1;
	if(sw2 >= DB_TIME)
		return 2;
	if(sw3 >= DB_TIME)
		return 3;
	if(sw4 >= DB_TIME)
		return 4;

	//Brak wciśniętych => zwróc zero
	return 0;
}
