/*
 * Schieberegler.cpp
 *
 * Created: 23.12.2020 09:18:51
 * Author : Dell
 */ 

#ifndef F_CPU
#define F_CPU 16000000UL

#endif

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <lcd.c>

//Variablen f�r die Zeit
volatile unsigned int millisekunden=0;
volatile unsigned int sekunde=0;

void init (void){
	DDRB |= (1<<PORTB2); // Shiftregister PIN 10 Arduino -- PIN 11 Shiftregister
	DDRB |= (1<<PORTB3); // StorePin PIN 11 Arduino -- PIN 12 Shiftregister
	DDRB |= (1<<PORTB5); // Datenregister PIN 13 Arduino -- PIN 14 Shiftregister
	
	// ADC konfigurieren
	ADMUX = (1<<REFS0); // | (0<<REFS1) | (0<<ADLAR);										//linksb�ndig mit AVCC als Referenz
	ADMUX |= (1<<MUX0); // | (0<<MUX3) | (0<<MUX2) | (0<<MUX1); 								//ADC1 Pin Port C PC1
	ADCSRA |= (1<<ADEN) | (1<<ADATE) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADSC); //| (0<<ADPS0) ;	//ADC enable und externe Triggerung 64 als Prescaler
	//	ADCSRB |= (0<<ADTS2) | (0<<ADTS1) | (0<<ADTS0);											//Free running modus, A/D Wandlung wird immer asap durchgef�hrt
	
	/*
	// Timer 0 konfigurieren
	TCCR0A |= (1<<COM0A1);// | (0<<COM0B0) | (0<<COM0B1) | (0<<COM0A0); // Stellt sicher, dass der Timer 0 auf einen Pin gelegt wird
	TCCR0A |= (1<<WGM01); // CTC Modus
	TCCR0B |= (1<<CS01) | (1<<CS00); // Prescaler 64
	// ((16000000/64)/1000) = 250 (counts to 250, and 1ms has passed)
	OCR0A = 250-1;
	  
	// Compare Interrupt A von Timer 0  erlauben
	TIMSK0 |= (1<<OCIE0A);
	TCNT0=0x00;
	  
	// Global Interrupts aktivieren
	sei();
	*/
}

ISR(TIMER0_COMPA_vect)
{
  millisekunden++;
  if(millisekunden == 1000)
  {
    sekunde++;
    millisekunden = 0;
  }
}

int main(void)
{
	init();
	lcd_init(LCD_DISP_ON);
	
	
	int out_IR[16] = {0,0,0,0 ,0,0,0,0 ,0,0,0,0 ,0,0,0,0};	//Array f�r IR LEDs in Sensor
	int in_IR [16] = {0,0,0,0 ,0,0,0,0 ,0,0,0,0 ,0,0,0,0};	//Array f�r gelesenen Sensorwerte
		
	char input[] = "1";
	char input_null[] = "0";
	
	int millisekunden_now = 0;

    while (1) { 
		for (int i=1; i<14; i++)		//Loop f�r Sensor
		{
			if (i!=1) out_IR[i-1] = 0;	//Vorheriges BIT abschalten
			out_IR[i] = 1;				//Jeweils ein BIT Einschalten
			
			
//=================Schieberegister Schreiben ========================
			PORTB &= ~(1 << PORTB3);
			
			for (int i=0; i<13; i++){
				PORTB &= ~(1 << PORTB2);
				if (out_IR[i]==1)
				PORTB |= (1 << PORTB5);
				else
				PORTB &= ~(1 << PORTB5);
				
				PORTB |= (1 << PORTB2);
			}
			
			PORTB |= (1 << PORTB3);
			
//================== Ende Scheiberegister =============================
	_delay_ms(500);
//================== Sensordaten Lesen ================================
		//millisekunden_now = millisekunden;
		//while ((millisekunden-millisekunden_now)<100){
			if(ADC<=200) in_IR[i] = 1;
			else in_IR[i] = 0;
		//}
			
		}
		for (int i = 0; i<13; i++)
		{
			if (in_IR[i] == 1)
			{
				lcd_puts(&input[0]);
				lcd_gotoxy(i+1,1);
			}
			else{
				lcd_puts(&input_null[0]);
				lcd_gotoxy(i+1,1);
			}
		}
		lcd_home();
	}
}

