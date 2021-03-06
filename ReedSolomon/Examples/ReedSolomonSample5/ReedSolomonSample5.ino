//
// ReedSolomon пример №5
// Передаём сложную структуру
//	при этом объявляем её c инициализацией
//
#include "ReedSolomon.h"

// Тип данных для передачи
struct BlackJackAndGirls {
	char name[10];
	struct {
		float temperature;
		float humidity;
	} kitchen;
	struct {
		float temperature;
		float humidity;
		long luminosity;
	} bathroom;
	int appNum;

	size_t print(Print& p) const {
		size_t res = p.print(F("Имя владельца: "));
		res += p.println(name);
		res += p.print(F("Номер квартиры: "));
		res += p.println(appNum);
		res += p.println(F("кухня"));
		res += p.print(F("\tтемпература: "));
		res += p.println(kitchen.temperature);
		res += p.print(F("\tвлажность: "));
		res += p.println(kitchen.humidity);
		res += p.println(F("ванная"));
		res += p.print(F("\tтемпература: "));
		res += p.println(bathroom.temperature);
		res += p.print(F("\tвлажность: "));
		res += p.println(bathroom.humidity);
		res += p.print(F("\tосвещённость: "));
		res += p.println(bathroom.luminosity);
		return res;
	}

};

// допускается до 30 ошибок при передаче
constexpr int8_t errorsAllowed = 30;

//	Пример объявления с инициализацией
DECLARE_PAYLOAD_INIT(blackJackAndGirls, BlackJackAndGirls, errorsAllowed)
{ "Вася", { 36.6, 78.1 }, { 40.0, 60.2, 78}, 102 }
END_OF_DECLARE_PAYLOAD_INIT

void setup(void) {
	randomSeed(analogRead(0));
	Serial.begin(115200);
	Serial.println(F("Пример передачи сложной структуры"));
	//
	Serial.println(F("Передаваемая структура:"));
	blackJackAndGirls.payload.print(Serial);
	//	
	//	Кодируем
	//
	const RS_RESULTS res = rsEncode(blackJackAndGirls);
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
		const int index = random(0, sizeof(blackJackAndGirls));
		(reinterpret_cast<uint8_t *>(& blackJackAndGirls))[index] ++;
	}
	//	
	//	Декодируем
	//
	const int errors = rsDecode(blackJackAndGirls);
	if (errors < 0) {
		Serial.print(F("Ошибка при декодировании: "));\
		Serial.println(res);
		return;
	}
	//
	Serial.println(F("Полученная структура:"));
	blackJackAndGirls.payload.print(Serial);
	Serial.print(F("\r\nИсправлено ошибок: "));
	Serial.println(errors);
	Serial.println(F("Веселье закончилось!"));
}

void loop(void) {}
