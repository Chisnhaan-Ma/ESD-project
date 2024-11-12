/*
 * Voltmetter.c
 *
 * Created: 9/13/2024 2:01:26 AM
 * Author : Chisnhaan
 */ 

#include <avr/io.h>
#define F_CPU 8000000UL
#include "util/delay.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#define LCD_data_DDR DDRD
#define LCD_data_PORT PORTD
#define LCD_com_DDR DDRB
#define LCD_com_PORT PORTB

#define RS 5
#define RW 6
#define EN 7


#define BUTTON_DDR DDRB
#define	BUTTON_PORT PORTB
#define Single_mode 0
#define Diff_mode 1
#define Gain_1 2
#define Gain_10	3
#define Gain_200 4


void LCD_com(unsigned char	cmnd);
void LCD_char(unsigned char char_data);
void LCD_string(char *str);
void Single();
void Diff();
void Diff_1();
void Diff_10();
void Diff_200();


int main(void)
{	

    LCD_com_DDR |= (1<<RS) | (1<<EN)|(1<<RW)|(0<<Single_mode)|(0<<Diff_mode)|(0<<Gain_1)|(0<<Gain_10)|(0<<Gain_200) ;
	PORTB |= (1<<Single_mode)|(1<<Diff_mode)|(1<<Gain_1)|(1<<Gain_10)|(1<<Gain_200);
	LCD_data_DDR = 0xFF;
	LCD_com(0x38);
	LCD_com(0x0E);
	LCD_com(0x01);
	
	LCD_com(0X83);
	LCD_string("ESD Project");
	LCD_com(0xC4);
	LCD_string("Voltmeter");
	_delay_ms(100);
  
	//BUTTON_DDR = 0x07;
	//BUTTON_PORT = 0xF8;
	
	LCD_com(0x01);
	LCD_com(0x80);
	LCD_string("Click the button");
	LCD_com(0xC0);
	LCD_string("to select mode!");
	_delay_ms(100);
	 
	LCD_com(0x01);
	LCD_string("Single");
	LCD_com(0xC0);
	LCD_string("Differential");
	
    while (1) 
    {
		if((PINB & (1<<Diff_mode)) == 0)
		{
			_delay_ms(10);
			if((PINB & (1<<Diff_mode)) == 0)
			Diff();
		}
		
		if((PINB & (1<<Single_mode)) == 0)
		{
			_delay_ms(10);
			if((PINB & (1<<Single_mode)) == 0)
			Single();
		}
		
	}
}

void LCD_com(unsigned char	cmnd)
{
	LCD_com_PORT &= ~(1<<RW);
	LCD_com_PORT &= ~(1<<RS);
	LCD_data_PORT = cmnd;
	LCD_com_PORT |= (1<<EN);
	_delay_ms(1);
	LCD_com_PORT &= ~(1<<EN);
	_delay_ms(5);
}

void LCD_char(unsigned char char_data)
{
		LCD_com_PORT &= ~(1<<RW);
		LCD_com_PORT |= (1<<RS);
		LCD_data_PORT = char_data;
		LCD_com_PORT |= (1<<EN);
		_delay_ms(1);
		LCD_com_PORT &= ~(1<<EN);
		_delay_ms(5);
}

void LCD_string(char *str)
{
	int i;
	for(i = 0; str[i]!=0;i++)
	{
		LCD_char(str[i]);
	}
}

void Single()
{
	float  Vin;
	int V, V1, V2, V3, V4;
	LCD_com(0x01);
	LCD_string("Single mode");
	while(1)
	{
	ADMUX |= (1<<REFS0);
	ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE) |(1<<ADPS2) |(1<<ADPS1)|(1<<ADPS0);
	SFIOR |= 0;
	while(!(ADCSRA & (1<<ADIF)));
	ADCSRA |= (1<<ADIF);
	Vin = (ADC*4.8828);
	V = Vin;
	V1 = (V/1000);
	V2 = (V%1000)/100;
	V3 = ((V%1000)%100)/10;
	V4 = (((V%1000)%100))%10;
	LCD_com(0Xc5);
	LCD_char(V1+0x30);
	LCD_char(',');
	LCD_char(V2+0x30);
	LCD_char(V3+0x30);
	LCD_char(V4+0x30);
	}
}

void Diff()
{
	LCD_com(0x01);
	LCD_string("Diff mode");
	_delay_ms(100);
	LCD_com(0x01);
	LCD_string("select Gain");
	
	while(1)
	{
		
		if((PINB & (1<<Gain_200)) == 0)
		{
			_delay_ms(10);
			if((PINB & (1<<Gain_200)) == 0)
			{
				Diff_200();
			}
		}
	
		if((PINB & (1<<Gain_10)) == 0)
		{
			_delay_ms(10);
			if((PINB & (1<<Gain_10)) == 0)
			{
			Diff_10();
			}
		}
		
		if((PINB & (1<<Gain_1)) == 0)
		{
			_delay_ms(10);
			if((PINB & (1<<Gain_1)) == 0)
			{
				Diff_1();
			}
		}				
		
		
	}
}

void Diff_1()
{
	double  Vin = 0;
	int32_t V = 0, V1, V2, V3, V4, V5;
	LCD_com(0x01);
	LCD_string("Diff x1");
	ADMUX |= (1<<REFS0)|(1<<MUX4);	//vi sai G = 1 PA0(+), PA1(-)
	ADCSRA |= (1<<ADEN)|(1<<ADPS2) |(1<<ADPS1)|(1<<ADPS0);
	SFIOR |= 0;
	while(1)
	{
		ADCSRA |= (1<<ADSC);
		while(!(ADCSRA & (1<<ADIF))); //check co ADIF
		ADCSRA |= (1<<ADIF);			// xoa co ADIF
		Vin = abs(ADC*9.76525/2);
		//Vin = Vin*1000;			// Vin = ADC*5000/512 ;lay 3 so thap phan sau dau phay
		
			V = (int)Vin;
			V1 = (V/10000);
			V2 = (V%10000)/1000;
			V3 = ((V%10000)%1000)/100;
			V4 = ((((V%10000)%1000))%100)/10;
			V5 = ((((V%10000)%1000))%100)%10;
			LCD_com(0Xc5);
			LCD_char(V1+0x30);
			LCD_char(V2+0x30);
			LCD_char(',');
			LCD_char(V3+0x30);
			LCD_char(V4+0x30);
			LCD_char(V5+0x30);
		
	}
}
void Diff_10()
{
	 double  Vin = 0;
	int32_t V = 0, V1, V2, V3, V4, V5;
	LCD_com(0x01);
	LCD_string("Diff x10");
	ADMUX |= (1<<REFS0)|(1<<MUX3)|(1<<MUX0);	//vi sai G = 1 PA0(+), PA1(-)
	ADCSRA |= (1<<ADEN)|(1<<ADPS2) |(1<<ADPS1)|(1<<ADPS0);
	SFIOR |= 0;
	while(1)
	{
		ADCSRA |= (1<<ADSC);
		while(!(ADCSRA & (1<<ADIF))); //check co ADIF
		_delay_ms(1);
		ADCSRA |= (1<<ADIF);			// xoa co ADIF
		Vin = abs(ADC*5/5120); // Vin = ADC*5000/512 ;lay 3 so thap phan sau dau phay
		Vin = Vin*1000;
		V = (int)Vin;
		V1 = (V/10000);
		V2 = (V%10000)/1000;
		V3 = ((V%10000)%1000)/100;
		V4 = ((((V%10000)%1000))%100)/10;
		V5 = ((((V%10000)%1000))%100)%10;
		LCD_com(0Xc5);
		LCD_char(V1+0x30);
		LCD_char(',');
		LCD_char(V2+0x30);
		LCD_char(V3+0x30);
		LCD_char(V4+0x30);
		LCD_char(V5+0x30);
	}
}

void Diff_200()
{
	long double  Vin = 0;
	int32_t V, V1, V2, V3, V4, V5; 
	LCD_com(0x01);
	LCD_string("Diff x200");
	ADMUX |= (1<<REFS0)|(1<<MUX3)|(1<<MUX1)|(1<<MUX0);
	ADCSRA |= (1<<ADEN) |(1<<ADPS2) |(1<<ADPS1)|(1<<ADPS0);	//mode chuyen doi 1 lan
	while(1)
	{
		ADCSRA |= (1<<ADSC);
		while(!(ADCSRA & (1<<ADIF)));
		ADCSRA |= (1<<ADIF);
		Vin = abs(ADC*19534.125); // Vin = ADC*50/512
		if(Vin==0)
			{
				LCD_com(0xC0);
				LCD_string("nguoc chieu V");
				
			}
		else
			{
				V = Vin;
				V1 = (V/10000);
				V2 = (V%10000)/1000;
				V3 = ((V%10000)%1000)/100;
				V4 = ((((V%10000)%1000))%100)/10;
				V5 = ((((V%10000)%1000))%100)%10;
				LCD_com(0Xc5);
				LCD_char(V1+0x30);
				LCD_char(V2+0x30);
				LCD_char(V3+0x30);
				LCD_char(V4+0x30);
				LCD_char(',');
				LCD_char(V5+0x30);
			}
	}
}