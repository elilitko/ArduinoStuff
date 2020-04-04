#include <Printing.h>
#define USE_PINOPS	true
#include <ESP_PSRAM64.h>

ESP_PSRAM64 psram;

void printReport(const char * title, const uint32_t duration) {
	uint32_t bytesPerSecond = (static_cast<uint64_t>(psram.totalBytes) * 1000 + duration / 2) / duration;
	const uint32_t spiBytesPerSecond = F_CPU / 16;
	uint32_t percent = (bytesPerSecond * 100 + spiBytesPerSecond / 2) / spiBytesPerSecond;
	Serial << title << "\r\n   " << (duration / 1000) << "с " 
		<< (duration % 1000) << "мс; " << bytesPerSecond 
		<< " байтов в секунду - " << percent << "% от скорости SPI\r\n";
}

void setup(void) {
	Serial.begin(115200);
	Serial.println("=== Тест на скорость чтения и записи ===");
	delayMicroseconds(150);
	psram.reset();
	//
	//	Тест на скорость заполнения (заполняем 8МБ нулями)
	uint32_t start = millis();
	psram.fill(0, psram.totalBytes);
	printReport("Заполнение 8МБ", millis() - start);
	Serial.flush();
	//
	//	Тест на скорость записи (пишем 8МБ блоками по 1кБ)
	uint8_t buffer[1024];
	const uint16_t sz = sizeof(buffer);
	start = millis();
	for (TAddress addr = 0; addr < psram.totalBytes; addr += sz) {
		psram.write(addr, buffer, sz);
	}
	printReport("Запись 8МБ блоками по 1кБ", millis() - start);
	Serial.flush();
	//
	//	Тест на скорость чтения (читаем 8МБ блоками по 1кБ)
	start = millis();
	for (TAddress addr = 0; addr < psram.totalBytes; addr += sz) {
		psram.read(addr, buffer, sz);
	}
	printReport("Чтение 8МБ блоками по 1кБ", millis() - start);
	//
	Serial.println("=== Тест завершён ===");
}

void loop(void) {}
