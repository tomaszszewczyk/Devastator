//Projekt 	-Devastator-
//Plik 		main.cpp
//Autor		wieloiksiasty

//----------
//Nagłówki
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include <util/atomic.h>
#include <string.h>
#include "RTC/twi.h"
#include "RTC/rtc.h"
#include "user.h"
#include "DS1820/ds.h"

//Glogalna struktura czasu
time now;
time alarm;
bool alarm_on;

//Dane budzika w EEPROM
EEMEM time eealarm_data;
EEMEM bool eealarm_on;

//----------
//Funkcja main
int main()
{
	//Konfiguracja peryferiów:
	ledInit();		//Wyświetlacza
	swInit();		//Przycisków
	kropkaInit();	//Kropki
	rtcInit();		//Zegara RTC
	buzzerInit();	//Głośniczka

	//Załadowanie danych budzika z EEPROM
	eeprom_read_block(&alarm, &eealarm_data, sizeof(time));
	eeprom_read_block(&alarm_on, &eealarm_on, sizeof(bool));

	//Konfiguracja timer2 do pętli głównej
	TCCR2 |= (1 << WGM21);
	TCCR2 |= (1 << CS20) | (1 << CS21);	//Tryb CTC, preskaler 32, ok. 1kHz
	OCR2 = 249;
	TIMSK |= (1 << OCIE2);	//Przerwanie przy porównaniu

	time tmp_time;
	tmp_time = rtcGetTime();
	rtcSetTime(tmp_time);

	kropkaOn();	//Zaświeć kropkę
	sei();	//Włącz przerwania

	while(1)
	{
		//Loopend :-)
		//Taki weekend dla mikrokontrolera :-D
	}
}

//----------
//Przerwanie Timer2 ok. 1kHz
//Pętla główna całego programu
ISR(TIMER2_COMP_vect)
{
	//--------------------------------------------------
	//Zmienna statyczne
	static uint8_t content[6];	//Zawartość wyświetlacza
	static uint8_t button = 0;	//Aktualny przycisk
	static uint8_t last = 0;	//Ostatni przycisk
	static uint8_t mode = 0;	//Aktualny tryb
	static uint8_t last_mode = 0;	//Ostatni tryb
	static uint16_t buzzer = 0;	//Odliczanie budzika (post działania)
	static bool budzenie = false;	//Flaga zadziałania budzika

	static uint16_t hold_time = 0;	//Odliczanie 3 sek. do wejścia w ust.

	uint8_t dot;	//Którą kropkę zapalić?

	//Sygnalizacja aktywacji budzika
	if(alarm_on == true)
		dot = 0;
	else
		dot = 0xff;


	//--------------------------------------------------
	//Pre- działania
	button = swProcess();	//Wczytaj przycisk

	if(now.msec < 999)	//Generator milisekund
		now.msec++;

	//Przełączanie funcji
	if(button == 4 && button != last && mode != 255)
	{
		mode++;
		if(mode > 2)
			mode = 0;
	}
	//Wchodzenie w ustawienia godziny
	if(button == 4)
	{
		if(hold_time <= 3000)
			hold_time++;
		if(hold_time > 3000)
			mode = 255;
	}
	else
		hold_time=0;

	//Piszczenie przy naciśnięciu przycisku
	if(button != last && button != 0)
		buzzer = 100;

	//Sprawdzenie czy nie należy uruchomić budzika
	if(now.hour == alarm.hour &&
	   now.min == alarm.min &&
	   now.sec == alarm.sec &&
	   alarm_on == true)
		budzenie = true;

	//Aktywny budzik wymusz tryb zegara
	if(budzenie == true)
		mode = 0;

	//--------------------------------------------------
	//Funkcja 0 => Zegar
		if(mode == 0)
		{
			//----------
			//Obsługa budzenia
			if(budzenie == true)
			{
				if(button == 0)	//Nic nie jest wciśnięte
				{
					//Sygnały akustyczne
					if(now.msec == 1)
						buzzer = 500;
					if(now.msec < 500)
					{
						kropkaOn();
						content[0] = now.sec%10;	//Wyświetl godzinę
						content[1] = now.sec/10;
						content[2] = now.min%10;
						content[3] = now.min/10;
						content[4] = now.hour%10;
						content[5] = now.hour/10;
					}
					else
					{
						kropkaOff();
						content[0] = 0xFF;	//Sygnały wizualne, miganie wyświetlacza
						content[1] = 0xFF;
						content[2] = 0xFF;
						content[3] = 0xFF;
						content[4] = 0xFF;
						content[5] = 0xFF;
					}
				}

				//Wciśnięcie czegokolwiek dezaktywuje budzenie
				else
					budzenie = false;
			}

			//----------
			//Normalne działanie
			else
			{
				//----------
				//Obsługa przycisków
				if(button == 0)	//Nic nie jest wciśnięte
				{
					if(now.msec < 500)
						kropkaOn();
					else
						kropkaOff();

					content[0] = now.sec%10;	//Wyświetl godzinę
					content[1] = now.sec/10;
					content[2] = now.min%10;
					content[3] = now.min/10;
					content[4] = now.hour%10;
					content[5] = now.hour/10;
				}
				else if(button == 1) //Przycisk 1
				{
					kropkaOff();
					content[0] = now.year%10;	//Wyświetl datę
					content[1] = now.year/10;
					content[2] = now.month%10;
					content[3] = now.month/10;
					content[4] = now.day%10;
					content[5] = now.day/10;
				}
				else if(button == 2) //Przycisk 2, temperatura
				{
					static uint16_t temp;
					static bool readed = false;

					//Przy pierwszej iteracji zapoczątkuj konwersję
					if(last != 2)
						dsConvertT();

					//W następnych odczytuj
					temp = dsRead();

					kropkaOff();	//Wyłącz dwukropki

					content[0] = 0;
					content[1] = (temp/1)%10;	//Wyświetl temperaturę
					content[2] = (temp/10)%10;
					content[3] = (temp/100)%10;

					//Zadbaj o znak - jeśli konieczne
					if(temp < 0) content[4] = '-';

					else content[4] = 0xFF;
					content[5] = 0xFF;

					dot = 2;	//Przecinek na 2 pozycji
				}
				else if(button == 3 && last != button) // Przycisk 3
				{
					if(alarm_on == true)	//Włącz, wyłącz budzik
						alarm_on = false;
					else
						alarm_on = true;

					//Zapisz wynik do eeprom
					eeprom_write_block(&alarm_on, &eealarm_on, sizeof(bool));

					kropkaOn();
					content[0] = alarm.sec%10;	//Wyświetl godzinę budzika
					content[1] = alarm.sec/10;
					content[2] = alarm.min%10;
					content[3] = alarm.min/10;
					content[4] = alarm.hour%10;
					content[5] = alarm.hour/10;
				}
			}
		}

	//--------------------------------------------------
	//Funkcja 1 - stoper
		//----------
		//Zmienne stopera
		static bool stopwatchStarted = false;
		static bool stopwatchFreezen;
		static time stopwatchStartTime;
		static time stopwatchOffset;
		static time stopwatchOutput;

		stopwatchFreezen = false;

		//----------
		//Obsługa przycisków
		if(mode == 1)
		{
			//Przycisk 1 - start/pause
			if(button == 1 && button != last)
			{
				stopwatchStarted ^= true;

				if(stopwatchStarted == true)	//Przy starcie załaduj czas startu
					stopwatchStartTime = now;
				else
					stopwatchOffset = stopwatchOutput;	//Przy pauzie załaduj offset
			}

			//Przycisk 2 - reset
			else if(button == 2 && button != last)
			{
				stopwatchStarted = false;					//Wyzeruj wszystko
				memset(&stopwatchOutput, 0, sizeof(time));
				memset(&stopwatchOffset, 0, sizeof(time));
			}

			//Przycisk 3 - lap - zamrożenie wyświetlacza
			else if(button == 3)
				stopwatchFreezen = true;
		}

		//----------
		//Obliczanie czasu do wyświetlenia
		if(stopwatchStarted == true)
		{
			//Oblicz czas od ostatniego startu
			stopwatchOutput = now;
			rtcDiff(&stopwatchOutput, &stopwatchStartTime);

			//Dodaj offset
			rtcSum(&stopwatchOutput, &stopwatchOffset);
		}

		//----------
		//Obsługa wyjścia stopera
		if(mode == 1)	//Włączona funkcja stopera
		{
			//Miganie kropkami
			if(stopwatchOutput.msec < 500 || stopwatchFreezen == true || stopwatchStarted == false)
						kropkaOn();
					else
						kropkaOff();

			//Sygnały dzwiękowe stopera
			if(stopwatchOutput.msec == 0 && stopwatchStarted == true)
				buzzer = 100;
			if(stopwatchOutput.msec == 0 && stopwatchOutput.sec == 0 && stopwatchStarted == true)
				buzzer = 500;

			//Przekazywanie danych do wyświetlenia
			if(stopwatchFreezen == false)
			{
				if(stopwatchOutput.hour > 0)
				{
					content[0] = stopwatchOutput.sec%10;	//Wyświetl czas
					content[1] = stopwatchOutput.sec/10;
					content[2] = stopwatchOutput.min%10;
					content[3] = stopwatchOutput.min/10;
					content[4] = stopwatchOutput.hour%10;
					content[5] = stopwatchOutput.hour/10;
				}
				else
				{
					content[0] = (stopwatchOutput.msec/10)%10;	//Wyświetl czas,
					content[1] = stopwatchOutput.msec/100;		//nie pokazuj zera w godzinach tylko m. sekundy
					content[2] = stopwatchOutput.sec%10;
					content[3] = stopwatchOutput.sec/10;
					content[4] = stopwatchOutput.min%10;
					content[5] = stopwatchOutput.min/10;
				}
			}
		}
	//--------------------------------------------------
	//Funckja 2 - minutnik/timer

		//Zmienne statyczne minutnika
		static time timerOutput;
		static time timerUserSet;
		static time timerTarget;
		static bool timerStarted = false;
		static uint8_t timerHighlighted = 3;	//Tryb normalny na start

		//Jeśli wybrano minutnik
		if(mode == 2)
		{
			//Tryb normalny
			if(timerHighlighted >= 3)
			{
				//Start/stop
				if(button == 1 && button != last)
				{
					timerStarted ^= true;

					timerTarget = timerOutput;
					rtcSum(&timerTarget, &now);	//Oblicz czas docelowy
				}

				//Reset
				if(button == 2 && button != last)
				{
					timerStarted = false;		//Wyzeruj
					timerOutput = timerUserSet;
				}

				//Wejdź w ustawianie czasu
				if(button == 3 && button != last)
				{
					timerStarted = false;

					timerOutput = timerUserSet;	//Załaduj wstępne dane
					timerHighlighted = 0;
				}

				if(timerStarted == true)
				{
					//Oblicz czas do wyświetlenia
					timerOutput = timerTarget;
					rtcDiff(&timerOutput, &now);

					//Zasygnalizuj koniec czasu i zatrzymaj
					if(timerOutput.msec == 0 &&
					   timerOutput.sec == 0 &&
					   timerOutput.min == 0 &&
					   timerOutput.hour == 0)
					{
						timerStarted = false;
						buzzer = 2000;
					}
				}

				//Wyświetlanie
				if (timerOutput.hour == 0)
				{
					//Jeśli nie trzeba godzin pokaż milisekundy
					content[0] = (timerOutput.msec/10)%10;
					content[1] = (timerOutput.msec/100)%10;
					content[2] = (timerOutput.sec/1)%10;
					content[3] = (timerOutput.sec/10)%10;
					content[4] = (timerOutput.min/1)%10;
					content[5] = (timerOutput.min/10)%10;
				}
				else
				{
					content[0] = (timerOutput.sec/1)%10;
					content[1] = (timerOutput.sec/10)%10;
					content[2] = (timerOutput.min/1)%10;
					content[3] = (timerOutput.min/10)%10;
					content[4] = (timerOutput.hour/1)%10;
					content[5] = (timerOutput.hour/10)%10;
				}

				//Buzzer
				//Gdy mniej niż 10s pikaj częściej
				if(timerOutput.hour == 0 &&
				   timerOutput.min == 0 &&
				   timerOutput.sec <= 10 &&
				   timerStarted == true)
				{
					if(timerOutput.msec % 200 == 0) //co 200ms
						buzzer = 50;
				}
				//Gdy więcej niż 10s pikaj co 1s
				else if(timerOutput.msec % 1000 == 0 && timerStarted == true)
					buzzer = 100;

				//Miganie kropką
				if(timerOutput.msec > 500)
					kropkaOn();
				else
					kropkaOff();

				if(timerStarted == false)
					kropkaOn();
			}

			//Tryb ustawiania czasu
			else if(timerHighlighted < 3)
			{
				kropkaOn();
				//Przcisk 1 - inkrementacja podświetlonej wartości
				if(button == 1 && button != last)
				{
					//Sekundy
					if(timerHighlighted == 0)
					{
						timerUserSet.sec++;
						if(timerUserSet.sec >= 60)
							timerUserSet.sec = 0;
					}

					//Minuty
					if(timerHighlighted == 1)
					{
						timerUserSet.min++;
						if(timerUserSet.min >= 60)
							timerUserSet.min = 0;
					}

					//Godziny
					if(timerHighlighted == 2)
					{
						timerUserSet.hour++;
						if(timerUserSet.hour >= 24)
							timerUserSet.hour = 0;
					}
				}
				//Przycisk 2 - dekrementacja podświetlonego
				if(button == 2 && button != last)
				{
					//Sekundy
					if(timerHighlighted == 0)
					{
						if(timerUserSet.sec == 0)
							timerUserSet.sec = 60;
						timerUserSet.sec--;
					}

					//Minuty
					if(timerHighlighted == 1)
					{
						if(timerUserSet.min == 0)
							timerUserSet.min = 60;
						timerUserSet.min--;
					}

					//Godziny
					if(timerHighlighted == 2)
					{
						if(timerUserSet.hour == 0)
							timerUserSet.hour = 24;
						timerUserSet.hour--;
					}
				}

				//Zmiana podświetlenia na następne lub wyjście
				if(button == 3 && button != last)
				{
					timerHighlighted++;
					timerOutput = timerUserSet;
				}

				//Wyświetlanie
				content[0] = (timerUserSet.sec/1)%10;
				content[1] = (timerUserSet.sec/10)%10;
				content[2] = (timerUserSet.min/1)%10;
				content[3] = (timerUserSet.min/10)%10;
				content[4] = (timerUserSet.hour/1)%10;
				content[5] = (timerUserSet.hour/10)%10;

				//Miganie podświetlonego
				if(now.msec < 500)
				{
					if(timerHighlighted == 0)
					{
						content[0] = 0xFF;
						content[1] = 0xFF;
					}

					else if(timerHighlighted == 1)
					{
						content[2] = 0xFF;
						content[3] = 0xFF;
					}

					else if(timerHighlighted == 2)
					{
						content[4] = 0xFF;
						content[5] = 0xFF;
					}
				}
			}
		}

	//--------------------------------------------------
	//Funckja 255 - ustawienia godziny, daty i budzika
		static uint8_t highlighted = 5;
		if(mode == 255)
		{
			//Wstępna konfiguracja
			kropkaOn();
			static time new_time;

			//Załadowanie wstępnych danych
			if(last_mode != mode)
			{
				new_time = now;
				new_time.msec = 0;
				new_time.sec = 0;
				highlighted = 8;
			}

			//--------------------------------------------------
			//Obsługa przycisków

			//----------
			//Zmień podświetlenie
			if(button == 3 && last != button)
			{
				if(highlighted == 0)
					highlighted = 9;
				highlighted--;
			}

			//----------
			//Dodaj wartość do podświetlonego
			else if(button == 1 && last != button)
			{
				//Sekundy
				if(highlighted == 0)
				{
					new_time.sec++;
					if(new_time.sec >= 60)
						new_time.sec = 0;
				}

				//Minuty
				else if(highlighted == 1)
				{
					new_time.min++;
					if(new_time.min >= 60)
						new_time.min = 0;
				}

				//Godziny
				else if(highlighted == 2)
				{
					new_time.hour++;
					if(new_time.hour >= 60)
						new_time.hour = 0;
				}

				//Dni
				else if(highlighted == 8)
				{
					new_time.day++;
					if(new_time.day >= 32)
						new_time.day = 0;
				}

				//Miesiące
				else if(highlighted == 7)
				{
					new_time.month++;
					if(new_time.month >= 13)
						new_time.month = 0;
				}

				//Lata
				else if(highlighted == 6)
				{
					new_time.year++;
					if(new_time.year >= 100)
						new_time.year = 0;
				}

				//Budzik sekundy
				else if(highlighted == 3)
				{
					alarm.sec++;
					if(alarm.sec >= 60)
						alarm.sec = 0;
				}

				//Budzik minuty
				else if(highlighted == 4)
				{
					alarm.min++;
					if(alarm.min >= 60)
						alarm.min = 0;
				}

				//Budzik godziny
				else if(highlighted == 5)
				{
					alarm.hour++;
					if(alarm.hour >= 24)
						alarm.hour = 0;
				}
			}

			else if(button == 2 && last != button)	//Odejmin wartość od podświetlonego
			{
				//Sekundy
				if(highlighted == 0)
				{
					if(new_time.sec == 0)
						new_time.sec = 60;
					new_time.sec--;
				}

				//Minuty
				else if(highlighted == 1)
				{
					if(new_time.min == 0)
						new_time.min = 60;
					new_time.min--;
				}

				//Godziny
				else if(highlighted == 2)
				{
					if(new_time.hour == 0)
						new_time.hour = 60;
					new_time.hour--;
				}

				//Dni
				else if(highlighted == 8)
				{
					if(new_time.day == 0)
						new_time.day = 32;
					new_time.day--;
				}

				//Miesiące
				else if(highlighted == 7)
				{
					if(new_time.month == 0)
						new_time.month = 13;
					new_time.month--;
				}

				//Lata
				else if(highlighted == 6)
				{
					if(new_time.year == 0)
						new_time.year = 100;
					new_time.year--;
				}

				//Budzik sekundy
				else if(highlighted == 3)
				{
					if(alarm.sec == 0)
						alarm.sec = 60;
					alarm.sec--;
				}

				//Budzik minuty
				else if(highlighted == 4)
				{
					if(alarm.min == 0)
						alarm.min = 60;
					alarm.min--;
				}

				//Budzik godziny
				else if(highlighted == 5)
				{
					if(alarm.hour == 0)
						alarm.hour = 24;
					alarm.hour--;
				}

			}
			else if(button == 4 && last != button)	//Zapisz do DSa i wyjdź z trybu
			{
				rtcSetTime(new_time);
				eeprom_write_block(&alarm, &eealarm_data, sizeof(time));
				mode = 0;
			}

			//Wyświetlanie
			if(highlighted <= 2)	//Podświetlone sek/min/godz
			{
				content[0] = new_time.sec%10;	//Wyświetl czas
				content[1] = new_time.sec/10;
				content[2] = new_time.min%10;
				content[3] = new_time.min/10;
				content[4] = new_time.hour%10;
				content[5] = new_time.hour/10;
			}
			else if(highlighted >= 3 && highlighted <= 5)	//Budzik
			{
				content[0] = alarm.sec%10;		//Wyświetl budzik
				content[1] = alarm.sec/10;
				content[2] = alarm.min%10;
				content[3] = alarm.min/10;
				content[4] = alarm.hour%10;
				content[5] = alarm.hour/10;
			}
			else if(highlighted >= 6 && highlighted <= 8)	//Podświetlone dni/mies/lata
			{
				content[0] = new_time.year%10;	//Wyświetl datę
				content[1] = new_time.year/10;
				content[2] = new_time.month%10;
				content[3] = new_time.month/10;
				content[4] = new_time.day%10;
				content[5] = new_time.day/10;
			}

			if(now.msec < 500)	//Miganie podświetlonego
			{
				if(highlighted == 0 || highlighted == 3 || highlighted == 6)
				{
					content[0] = 0xff;
					content[1] = 0xff;
				}
				if(highlighted == 1 || highlighted == 4 || highlighted == 7)
				{
					content[2] = 0xff;
					content[3] = 0xff;
				}
				if(highlighted == 2 || highlighted == 5 || highlighted == 8)
				{
					content[4] = 0xff;
					content[5] = 0xff;
				}
			}

			dot = (highlighted/3);	//Kropka pomaga w orientacji
		}

	//--------------------------------------------------
	//Post- działania

	//Obsługa wyświetlacza
	ledProcess(content, dot);

	//Załaduj ostatni przycisk i ostatni tryb
	last = button;
	last_mode = mode;

	//Opóźnione odliczanie buzzera
	if(buzzer > 0)
	{
		buzzer--;
		buzzerOn();
	}
	else
		buzzerOff();

}

//----------
//Przerwanie zewnętrzne INT0 1Hz
//Pętla główna aktualizacji czasu
//Nieblokowanie pozwala na elimicję migotania wyśw.
ISR(INT0_vect, ISR_NOBLOCK)
{
	time buffer;			//Odczyt może być przerywany
	buffer = rtcGetTime();


	ATOMIC_BLOCK(ATOMIC_FORCEON)	//Bezpośrednia aktualizacja
	{								//jest operacją atomową
		now = buffer;
		now.msec = 0;
	}
}
