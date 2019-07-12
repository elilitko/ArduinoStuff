
#include "CommonIncludes.h"

#define DO_ONE(pin)	_digitalWrite(pin,HIGH); _delay_ms(350);_digitalWrite(pin,LOW);

void doIt(void) {
	DO_ONE(0);
	DO_ONE(1);
	DO_ONE(2);
	DO_ONE(3);
	DO_ONE(4);
	DO_ONE(5);
	DO_ONE(6);
	DO_ONE(7);
	DO_ONE(8);
	DO_ONE(9);
	DO_ONE(10);
	DO_ONE(11);
	DO_ONE(12);
	DO_ONE(13);
#ifdef USE_XTAL_PINS
	DO_ONE(XTAL1_PIN);
	DO_ONE(XTAL2_PIN);
#endif
	DO_ONE(A0);
	DO_ONE(A1);
	DO_ONE(A2);
	DO_ONE(A3);
	DO_ONE(A4);
	DO_ONE(A5);
#ifdef USE_RESET_PIN
	DO_ONE(RESET_PIN);
#endif
}


int main(void) {
	_pinMode(0, OUTPUT);
	_pinMode(1, OUTPUT);
	_pinMode(2, OUTPUT);
	_pinMode(3, OUTPUT);
	_pinMode(4, OUTPUT);
	_pinMode(5, OUTPUT);
	_pinMode(6, OUTPUT);
	_pinMode(7, OUTPUT);
	_pinMode(8, OUTPUT);
	_pinMode(9, OUTPUT);
	_pinMode(10, OUTPUT);
	_pinMode(11, OUTPUT);
	_pinMode(12, OUTPUT);
	_pinMode(13, OUTPUT);
#ifdef USE_XTAL_PINS
	_pinMode(XTAL1_PIN, OUTPUT);
	_pinMode(XTAL2_PIN, OUTPUT);
#endif
	_pinMode(A0, OUTPUT);
	_pinMode(A1, OUTPUT);
	_pinMode(A2, OUTPUT);
	_pinMode(A3, OUTPUT);
	_pinMode(A4, OUTPUT);
	_pinMode(A5, OUTPUT);
#ifdef USE_RESET_PIN
	_pinMode(RESET_PIN, OUTPUT);
#endif
	while(1) doIt();
}
