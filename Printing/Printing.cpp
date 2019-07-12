#include <Arduino.h>
#include <Printing.h>
#include <stdio.h>

static struct Printf {
	static int serial_fputchar(const char ch, FILE *stream) { 
		(void)(stream); 
		Serial.write(ch); 
		return ch; 
	}
	Printf(void) { fdevopen(serial_fputchar, NULL); }
} _;

