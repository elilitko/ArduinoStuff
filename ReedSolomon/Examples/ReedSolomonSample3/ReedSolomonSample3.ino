//
// ReedSolomon пример №3
// Передаём массив из 10 элементов типа int 
//	при этом объявляем его c инициализацией
//
#include "ReedSolomon.h"

// Тип данных для передачи
typedef int TArray10[10];

// допускается до 20 ошибок при передаче
constexpr int8_t errorsAllowed = 20;

//	Пример объявления без инициализации
DECLARE_PAYLOAD_INIT(anArray, TArray10, errorsAllowed)
{ 321, 123, 901, 120, 999, 666, 333, 412, 362, 10110 }
END_OF_DECLARE_PAYLOAD_INIT

void setup(void) {
	randomSeed(analogRead(0));
	Serial.begin(115200);
	Serial.println(F("Пример передачи массива из 10 элементов типа int"));
	//
	Serial.print(F("Передаваемый массив:"));
	for (int n : anArray.payload) {
		Serial.print(' ');
		Serial.print(n);
	}
	Serial.println();
	//	
	//	Кодируем
	//
	const RS_RESULTS res = rsEncode(anArray);
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
		const int index = random(0, sizeof(anArray));
		(reinterpret_cast<uint8_t *>(& anArray))[index] ++;
	}
	//	
	//	Декодируем
	//
	const int errors = rsDecode(anArray);
	if (errors < 0) {
		Serial.print(F("Ошибка при декодировании: "));\
		Serial.println(res);
		return;
	}
	//
	Serial.print(F("  Полученный массив:"));
	for (int n : anArray.payload) {
		Serial.print(' ');
		Serial.print(n);
	}
	Serial.println();
	Serial.print(F("Исправлено ошибок: "));
	Serial.println(errors);
	Serial.println(F("Веселье закончилось!"));
}

void loop(void) {}
