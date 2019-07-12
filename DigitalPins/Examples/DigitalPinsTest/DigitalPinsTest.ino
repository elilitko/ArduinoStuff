#include <DigitalPins.h>
template <typename T> inline Print & operator << (Print &s, T n) { s.print(n); return s; }

void wait(void) {
	while(!Serial.available());
	while(Serial.available()) Serial.read();
}

void setup() {
	Serial.begin(115200);
	//	1. Пины 2 и 3 должны мигать по очереди, а пин 13 мигать с ними в такт
	_pinMode(2, OUTPUT);
	_pinMode(3, OUTPUT);
	_pinMode(13, OUTPUT);
	_digitalWrite(2, HIGH);
	_pinMode(4, INPUT);
	_pinMode(6, INPUT);
	_pinMode(5, INPUT_PULLUP);
	_pinMode(7, INPUT_PULLUP);
	Serial << "Tests _pinMode & _digitalRead: 0 must be 0, 1 must be 1\n";
	Serial << "0=" << _digitalRead(4) << " 0=" << _digitalRead(6) << " 1=" << _digitalRead(5) << " 1=" << _digitalRead(7) << '\n';
	Serial << "_digitalRead4: 0 must be 0, 1 must be 1\n";
	uint8_t r = _digitalRead4(4,5,6,7); 
	Serial << "0=" << ((r&_pin4)?1:0) << " 0=" << ((r&_pin6)?1:0) << " 1=" << ((r&_pin5)?1:0) << " 1=" << ((r&_pin7)?1:0) << '\n';
	Serial << "Pin 2 LED must be ON (press ENTER)\n";
	wait();
	_digitalWrite(2, LOW);
	Serial << "Pin 2 LED must be OFF (press ENTER)\n";
	wait();
	_digitalWrite2(2, 3, HIGH);
	Serial << "Both pins 2 and 3 LEDa must be ON (press ENTER)\n";
	wait();

/*
	_pinMode(3, INPUT);
	_pinMode(3, OUTPUT);
	_pinMode(3, INPUT_PULLUP);
	_digitalWrite(3, HIGH);
	_digitalWrite(3, LOW);
	_digitalRead(3);
	_pulseHigh(3);
	_pulseLow(3);
	_pinInvert(3);
	_pinPulse(3);
	_digitalWrite7(1,2,3,4,5,6,7, HIGH);
	_digitalWrite7(0,1,2,3,4,5,6, LOW);
	_pulseHigh7(1,2,3,4,5,6,7);
	_pulseLow7(1,2,3,4,5,6,7);
	_pinInvert5(1,2,3,4,5);
	_pinPulse7(1,2,3,4,5,6,7);

	_digitalRead2(2,3);
	*/
}

void loop() {
	//	1. Пины 2 и 3 должны мигать по очереди, а пин 13 мигать с ними в такт
	_pinInvert2(2,3);
	_pinInvert(13);
	delay(200);
}

