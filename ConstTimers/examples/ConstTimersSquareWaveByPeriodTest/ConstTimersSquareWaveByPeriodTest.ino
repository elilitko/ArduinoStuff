/////////////////////////////////////////////////
//
//	Получаем меандр с частотой 50Гц на 9-ом пине
//	для этого заводим его на СТС c инвертированием
//	на частоте 100Гц (как раз 100/2 = 50)
//	Таким образом, период равен 10_ms
//
#include <ConstTimers.h>

#define	TIMER	1
#define	PERIOD	10_ms

static constexpr uint8_t prescalerBits = prescalerBitsByPeriod(TIMER, PERIOD);
static constexpr uint16_t timerTicks = timerTicksByPeriod(TIMER, PERIOD);

// проверка установилась ли частота (например, при периоде 4195_ms будет ошибка компиляции)
static_assert(timerTicks, "The required period with a desired accuracy is unattainable for timer/counter");

void setup() {
	pinMode(9, OUTPUT);
	TCCR1A = bit(COM1A0);						// Инвертирование пина 9 по сравнению
	TCCR1B = bit(WGM12) | prescalerBits;	// Установить СТС режим и делитель частоты
	OCR1A = timerTicks;							// установить TOP равным timerTicks
}

void loop() {}
