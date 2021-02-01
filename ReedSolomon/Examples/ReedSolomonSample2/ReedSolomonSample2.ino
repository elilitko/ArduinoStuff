//
// ReedSolomon пример №2
// Передаём одно число типа long 
//	при этом объявляем его без инициализации
//
#include "ReedSolomon.h"

// допускается до 10 ошибок при передаче
constexpr int8_t errorsAllowed = 10;

//	Пример объявления без инициализации
DECLARE_PAYLOAD(aNumber, long, errorsAllowed)

void setup(void) {
	randomSeed(analogRead(0));
	Serial.begin(115200);
	Serial.println(F("Пример передачи числа типа long"));
	//
	//	Присвоим значение передаваемому числу
	//
	aNumber.payload = 321321321L;
	Serial.print(F("Передаваемое число: "));
	Serial.println(aNumber.payload);
	//	
	//	Кодируем
	//
	const RS_RESULTS res = rsEncode(aNumber);
	if (res != RSE_NO_ERROR) {
		Serial.print(F("Ошибка при кодировании: "));
		Serial.println(res);
		return;
	}
	//
	// Вносим не более errorsAllowed ошибок в случайные места
	//	(может получиться меньше, чем errorsAllowed, если дважды в один и тот же байт внесём)
	//
	for (int8_t i = 0; i < errorsAllowed; i++) {
		const int index = random(0, sizeof(aNumber));
		(reinterpret_cast<uint8_t *>(& aNumber))[index] ++;
	}
	//	
	//	Декодируем
	//
	const int errors = rsDecode(aNumber);
	if (errors < 0) {
		Serial.print(F("Ошибка при декодировании: "));\
		Serial.println(res);
		return;
	}
	//
	Serial.print(F("Полученное число: "));
	Serial.println(aNumber.payload);
	Serial.print(F("Исправлено ошибок: "));
	Serial.println(errors);
	Serial.println(F("Веселье закончилось!"));
}

void loop(void) {}
