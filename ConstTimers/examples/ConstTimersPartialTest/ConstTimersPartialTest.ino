#include <Printing.h>
#include <ConstTimers.h>
//
//	Подходим у неправильной частоте слева и справа и отслеживаем,
//	когда программа откажет (выход из допустимой погрешности)
//
// Частота                    337055      337054     343864    343865
// Точное количесто тактов 47,469997   47,470138  46,530023 46,529888
// Ближайшее целое                47          47         47        47
// Разница                  0,469997    0,470138   0,469977  0,470112
// %                        0,999995    1,000294   0,999950  1,000238
// Результат                   Ok          Fail       Ok       Fail
//

void setup(void) {
	Serial.begin(115200);
	Serial << "*** Частичный тест библиотеки ConstTimers ***\r\n";
	for (uint32_t f = 337054; f < 337056; f++) {
		const uint16_t ticks = timerTicksByFrequency(1, f, 1);
		const uint8_t bits = prescalerBitsByFrequency(1, f);
		Serial << "f=" << f << "; ticks=" << ticks << "; bits=" << bits << "\r\n";
	}
	for (uint32_t f = 343864; f < 343866; f++) {
		const uint16_t ticks = timerTicksByFrequency(1, f, 1);
		const uint8_t bits = prescalerBitsByFrequency(1, f);
		Serial << "f=" << f << "; ticks=" << ticks << "; bits=" << bits << "\r\n";
	}
	Serial << "All done!\r\n";
}

void loop(void) {} 
