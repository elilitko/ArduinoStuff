#include <Printing.h>
#define USE_PINOPS	true
#include <ESP_PSRAM64.h>

ESP_PSRAM64 psram;

static constexpr TSize oneKil = 1024;
static constexpr TSize halfMemory = psram.totalBytes / 2;

//
// Буфер в 1 килобайт (и два буфера половинного размера)
// Размеры буферов
static uint8_t largeBuffer[oneKil];
static constexpr uint16_t bufSize = sizeof(largeBuffer) / sizeof(largeBuffer[0]);
static constexpr uint16_t halfBufSize = bufSize / 2;
static uint8_t * buf1 = largeBuffer;
static uint8_t * buf2 = buf1 + halfBufSize;
//
static constexpr TSize halfMemoryButOneBuffer = halfMemory - bufSize;

//
//	Заполнение буфера псевдослучайными значениями
//
static inline void fillOutBufferWithRandomValues(void) {
	for (uint16_t i = 0; i < bufSize; largeBuffer[i++] = random(256));
}

//
//	Запись всей (8 мегабайт) памяти устройста псеводслучайными значениями
//	так, чтобы младшие 4МБ были копией старших 4МБ.
//	При этом используем Операции как блочной, так и побайтовой записи
//
static void writeEightMegabytes(void) {
	fillOutBufferWithRandomValues();
	TAddress addr = 0;
	Serial << "Writing ";
	//
	//	Запись первой половины памяти
	// Сначала пишем случайно количество байтов побайтово
	//	Затем, с произвольной границы пишем поблочно, сколько можем
	//	Наконец, дожимаем до конца опять побайтово
	uint16_t totalBytes = random(bufSize - bufSize / 4) + 1;
	for (uint16_t i = 0; i < totalBytes; addr++) psram.write(addr, largeBuffer[i++]);
	for (; addr < halfMemoryButOneBuffer; addr +=  bufSize) psram.write(addr, largeBuffer, bufSize);
	for (; addr < halfMemory; addr++) psram.write(addr, largeBuffer[addr % bufSize]);
	Serial << '+';
	//
	//	Копирование первой половины памяти во вторую
	// в этот моментт addr равен halfMemory (вот в него и пишем)
	for (TAddress lowPart = 0; addr < psram.totalBytes; lowPart += bufSize, addr +=  bufSize) {
		psram.read(lowPart, largeBuffer, bufSize);
		psram.write(addr, largeBuffer, bufSize);
	}
	Serial << " done.";
}

//
//	Чтение памяти и сравнение старшей и младшей половин
//	При этом используем Операции как блочного, так и побайтового чтения
//
static void readAndCheckEightMegabytes(void) {
	TSize totalErrors = 0;
	TAddress addr = 0;
	Serial << " *** Reading";
	//
	//	Побайтовое чтение
	for (uint16_t bPtr = 0; bPtr < halfBufSize; addr++, bPtr++) {
		buf1[bPtr] = psram.read(addr);
		buf2[bPtr] = psram.read(addr + halfMemory);
	}
	for (uint16_t i = 0; i < halfBufSize; i ++) {
		totalErrors += (int)(buf1[i] != buf2[i]);
	}
	//
	//	Блочное чтение
	for (; addr < halfMemory; addr +=  halfBufSize) {
		psram.read(addr, buf1, halfBufSize);
		psram.read(addr + halfMemory, buf2, halfBufSize);
		for (uint16_t i = 0; i < halfBufSize; i ++) {
			totalErrors += (int)(buf1[i] != buf2[i]);
		}
	}
	Serial << " done. Errors: " << totalErrors << (totalErrors ? " !!!!!" : ".") << "\r\n";
}

void setup(void) {
	randomSeed(analogRead(0));
	Serial.begin(115200);
	Serial << "ESP PSRAM64H тест на корректность чтения и записи\r\n";
	delayMicroseconds(150); // См. раздел №3 даташита
	psram.reset();
}

void loop(void) {
	writeEightMegabytes();
	readAndCheckEightMegabytes();
	_delay_ms(3000);
}
