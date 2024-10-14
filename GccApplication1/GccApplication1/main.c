/*
 * testchip2.c
 *
 * Created: 10/1/2024 12:21:10 PM
 * Author : LENOVO
 */ 

#include <avr/io.h>

#define F_CPU 8000000UL
#include "util/delay.h"
#include <string.h>
#include <stdio.h>

/* Replace with your library code */
int myfunc(void)
{
	DDRB = 0xff;
	while(1){
		PORTB = 0xff;
		_delay_ms(200);
		PORTB=0;	}
}


