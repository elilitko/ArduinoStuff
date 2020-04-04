#include <Printing.h>
#define	USE_PINOPS true
#include <ESP_PSRAM64.h>

ESP_PSRAM64 psram;

void setup(void) {
	Serial.begin(115200);
	Serial.println("Простой тест - пишем по 6 байтов в случайный адрес и читаем оттуда");
	delayMicroseconds(150);
	randomSeed(analogRead(0));
	psram.reset();
}

void loop(void) {
	// Буффер со случайным содержимым
	byte buffer[6];
	constexpr TSize size = sizeof(buffer);
	for (TSize i=0; i < size; i++) buffer[i] = random(0, 255);
	
	// Случайный адрес
	const TAddress address = random(0, psram.totalBytes - size);
	Serial << "Адрес: " << address << "\r\n";

	Serial << " Пишем: ";
	psram.write(address, buffer, size);
	for (TSize i=0; i < size; i++) Serial << (unsigned) buffer[i] << ' ';
	Serial.println();

	Serial << "Читаем: ";
	psram.read(address, buffer, size);
	for (TSize i=0; i < size; i++) Serial << (unsigned) buffer[i] << ' ';
	Serial.println();

	delay(2000);
}
