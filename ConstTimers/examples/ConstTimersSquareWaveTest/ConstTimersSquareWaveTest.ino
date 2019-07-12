/////////////////////////////////////////////////
//
//	Получаем меандр с частотой 50Гц на 9-ом пине
//	для этого заводим его на СТС c инвертированием
//	на частоте 100Гц (как раз 100/2 = 50)
//
#include <ConstTimers.h>

#define	TIMER	1
#define	FREQUENCY	100

static constexpr uint8_t prescalerBits = prescalerBitsByFrequency(TIMER, FREQUENCY);
static constexpr uint16_t timerTicks = timerTicksByFrequency(TIMER, FREQUENCY);

// проверка установилась ли частота (например, при частоте 101 будет ошибка компиляции)
static_assert(timerTicks, "The required frequency with a desired accuracy is unattainable for timer/counter");

void setup() {
	pinMode(9, OUTPUT);
	TCCR1A = bit(COM1A0);						// Инвертирование пина 9 по сравнению
	TCCR1B = bit(WGM12) | prescalerBits;	// Установить СТС режим и делитель частоты
	OCR1A = timerTicks;							// установить TOP равным timerTicks
}

void loop() {}
