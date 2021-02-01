//
// ReedSolomon пример №7
// Передаём данные напрямую без использования макросов
//	это бывает нужно, когда длина данных заранее неизвестна
//	и нельзя определить тип для них.
//
#include "ReedSolomon.h"

//
// Будем передавать вот эту строку, но считаем, что мы заранее её не знаем
//	а откуда-то получили и на лету посчитали её длину
//
const char mk24_4 [] = 
R"(«Всякому взбрыкнувшему да отбрыкнется и ещё добавится, ибо нехрен!»
(От Марка гл. 4, стих 24, перевод мой))";

// допускается до 20 ошибок при передаче
constexpr int8_t errorsAllowed = 20;

	
void setup(void) {
	randomSeed(analogRead(0));
	Serial.begin(115200);
	Serial.println(F("Пример передачи без импользования типа Payload"));
	//
	//	Собираемся передавать строку mk24_4
	//	Для начала узнаем её длину в байтах, вкл. конечный 0
	//
	const size_t dataLen = sizeof(mk24_4);
	//
	// Плюс, нам понадобятся 2 * errorsAllowed служебных байтов
	//	Таким образом полная длина пакета
	//
	const size_t packageLength = dataLen + 2 * errorsAllowed;
	//
	//	запрашиваем память
	//
	char * pPackage = reinterpret_cast<char *>(malloc(packageLength));
	if (! pPackage) {
		Serial.println(F("Нету памяти :-("));
		return;
	}
	//
	//	Наши данные идут ПОСЛЕ служебных. Туда их и копируем
	//
	char * const payloadBegins = pPackage + 2 * errorsAllowed;
	memcpy(payloadBegins, mk24_4, dataLen);
	//
	//	Всё, теперь мы готовы
	//
	Serial.println(F("Передаваемые данные"));
	Serial.println(payloadBegins);
	//
	// Кодируем
	//
	const RS_RESULTS res = ReedSolomonEncoding::rsEncode(pPackage, errorsAllowed, packageLength);
	if (res != RSE_NO_ERROR) {
		Serial.print(F("Ошибка при кодировании: "));
		Serial.println(res);
		free(pPackage);
		return;
	}
	//
	// Вносим не более errorsAllowed ошибок в случайные места
	//	(может получиться меньше, чем errorsAllowed, если дважды в один и тот же байт внесём)
	//
	for (int8_t i = 0; i < errorsAllowed; i++) {
		const int index = random(0, packageLength);
		pPackage[index] ++;
	}
	//	
	//	Декодируем
	//
	const int errors = ReedSolomonEncoding::rsDecode(pPackage, errorsAllowed, packageLength);
	if (errors < 0) {
		Serial.print(F("Ошибка при декодировании: "));
		Serial.println(res);
		free(pPackage);
		return;
	}
	
	Serial.println(F("Полученные данные"));
	Serial.println(payloadBegins);
	Serial.print(F("\r\nИсправлено ошибок: "));
	Serial.println(errors);
	Serial.println(F("Веселье закончилось!\r\n"));
	free(pPackage);
}

void loop(void) {}
