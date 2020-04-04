#ifndef	ESP_PSRAM64_H
#define	ESP_PSRAM64_H

#include <SPI.h>

// Если CE пин не задан, используем стандартный SS
#ifndef	PSRAM64_CE_PIN
	#define	PSRAM64_CE_PIN	PIN_SPI_SS
#endif

#if USE_PINOPS
	#include <PinOps.h>
	#define	DigitalWrite PinOps::digitalWrite		
	#define	DigitalRead PinOps::digitalRead		
	#define	PinMode PinOps::pinMode
#else	// #if USE_PINOPS
	#define	DigitalWrite digitalWrite		
	#define	DigitalRead digitalRead		
	#define	PinMode pinMode
#endif	// #if USE_PINOPS

// __int24/__uint24 доступны с версии 4.7 GCC
#if ((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)))
	typedef __uint24 TAddress;
	typedef __uint24 TSize;
#else
	typedef uint32_t TAddress;
	typedef uint32_t TSize;
#endif

#define AI	__attribute__((always_inline))

class ESP_PSRAM64 {
public:
	static constexpr TSize totalBytes = 8L * 1024L * 1024L;

	inline ESP_PSRAM64(void) {
		//	При инициализации чипа, CE# должен быть HIGH
		//	а всё остальное - LOW (раздел №3 даташита)
		PinMode(PSRAM64_CE_PIN, OUTPUT);
		PinMode(PIN_SPI_SCK, OUTPUT);
		PinMode(PIN_SPI_MOSI, OUTPUT);
		PinMode(PIN_SPI_MISO, OUTPUT);
		DigitalWrite(PSRAM64_CE_PIN, HIGH);
		DigitalWrite(PIN_SPI_SCK, LOW);
		DigitalWrite(PIN_SPI_MOSI, LOW);
		DigitalWrite(PIN_SPI_MISO, LOW);
	}

	inline void beginTransaction(void) const AI {
		SPI.beginTransaction(SPISettings(F_CPU / 2, MSBFIRST, SPI_MODE0));
	}

	inline void endTransaction(void) const AI {
		SPI.endTransaction();
	}

	inline void reset(void) const AI {
		SPI.begin();
		beginTransaction();
		highCE();
		lowCE();
		SPI.transfer(opResetEnable);
		highCE();
		lowCE();
		SPI.transfer(opReset);
		highCE();
	}

	inline void write(const TAddress address, const uint8_t data) const AI {
		opStart(opWrite, address);
		SPI.transfer(data);
		highCE();
	}

	inline uint8_t read(const TAddress address) const AI {
		opStart(opRead, address);
		const uint8_t res = SPI.transfer(0);
		highCE();
		return res;
	}

	template <class T> inline void get(const TAddress addr, T & t) const {
		read(addr, (uint8_t *)& t, sizeof(T));
	}

	template <class T> inline void put(const TAddress addr, const T &t) const {
		write(addr, (uint8_t *)& t, sizeof(T));
	}

#define MULTIBYTE_OPERATION(_operation, _dataOp) \
	for (TSize i = 0; i < size; ) { \
		opStart(_operation, address + i); \
		for (uint8_t j = 0; j < dataBlockSize && i < size; j++, i++) _dataOp; \
		highCE(); \
	}

	inline void fill(const TAddress address, const TSize size, const uint8_t data = 0) const AI {
		MULTIBYTE_OPERATION(opWrite, SPI.transfer(data));
	}

	inline void write(const TAddress address, const uint8_t * const buffer, const TSize size) const AI {
		MULTIBYTE_OPERATION(opWrite, SPI.transfer(buffer[i]));
	}

	inline void read(const TAddress address, uint8_t * const buffer, const TSize size) const AI {
		MULTIBYTE_OPERATION(opRead, buffer[i] = SPI.transfer(0));
	}
#undef MULTIBYTE_OPERATION

protected:
	static constexpr uint8_t opRead = 3, opWrite = 2, opResetEnable = 0x66, opReset = 0x99;
	static constexpr uint8_t dataBlockSize = 32;

	inline void opStart(const uint8_t command, const TAddress address) const AI {
		lowCE();
		SPI.transfer(command);
		SPI.transfer(address >> 16);
		SPI.transfer(address >> 8);
		SPI.transfer(address);
	}

	inline void lowCE(void) const AI { DigitalWrite(PSRAM64_CE_PIN, LOW); }
	inline void highCE(void) const AI { DigitalWrite(PSRAM64_CE_PIN, HIGH); }
};

#undef AI
#endif	//	ESP_PSRAM64_H