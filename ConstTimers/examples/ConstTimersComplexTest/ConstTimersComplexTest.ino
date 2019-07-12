///////////////////////////////////////////////////////////
//
//	Тест на ВСЕ возможные точные значения периодов и частот
//
//	используются внутренние функции простанства имён constTimersNS,
//	которые при обычной работе не нужны.
//
#include <Printing.h>
#include <ConstTimers.h>

void setup(void) {
	const uint32_t startTime = millis();
	Serial.begin(115200);
	Serial << "*** Комплексный тест библиотеки ConstTimers ***\r\n";

	uint32_t	errorCounter = 0;
	uint32_t	periodCounter = 0;
	uint32_t	frequencyCounter = 0;
	for (uint8_t timerId = 0; timerId < constTimersNS::totalTimers; timerId ++) {
		const constTimersNS::STimerParameters & timer = constTimersNS::timerParameters[timerId];
		Serial << "Таймер/счётчик №" << timerId << "\r\n";
		for (uint8_t prescalerId = 0; prescalerId < timer.totalPrescalers; prescalerId++) {
			const int prescaler = timer.prescalers[prescalerId];
			Serial << "Делитель " << prescaler << " таймера/счётчика №" << timerId << "\r\n";
			for (uint32_t prPeriod = 1; prPeriod <= timer.maxValue; prPeriod ++) {
				const uint32_t period = prPeriod * prescaler;
				const uint8_t bits = prescalerBitsByPeriod(timerId, period);
				const uint16_t ticks = timerTicksByPeriod(timerId, period);
				const uint32_t realPeriod = static_cast<uint32_t>(ticks) * prescalerValue(timerId, bits);
				periodCounter++;
				if (realPeriod != period) {
					errorCounter++;
					Serial << "*** Error with \"by period\" calculation: timer/counter:" << timerId << "; period:" << period
						 << "; bits:" << bits << '(' << prescalerValue(timerId, bits) << "); ticks:" 
						 << ticks << "; realPeriod:" << realPeriod << "\r\n";
				}
				if (FCPU % period == 0) {	// Если периоду соответсвует целая частота
					frequencyCounter++;
					const uint32_t frequency = FCPU / period;
					const uint8_t bitsF = prescalerBitsByFrequency(timerId, frequency);
					const uint16_t ticksF = timerTicksByFrequency(timerId, frequency);
					const uint32_t realPeriodF = static_cast<uint32_t>(ticksF) * prescalerValue(timerId, bitsF);
					if (realPeriodF != period) {
						errorCounter++;
						Serial << "*** Error with \"by frequency\" calculation: timer/counter:" 
							<< timerId << "; period:" << period << "; frequency:" << frequency
							 << "; bits:" << bitsF << '(' << prescalerValue(timerId, bitsF) << "); ticks:" 
							 << ticksF << "; realPeriod:" << realPeriodF << "\r\n";
					}
				}
			}
			Serial << "Делитель " << prescaler << " таймера/счётчика №" << timerId << " - завершено\r\n";
		}
		Serial << "Таймер/счётчик #" << timerId << " - завершено!\r\n";
	}
	const uint32_t totalTime = millis()  - startTime;
	uint16_t seconds = (totalTime + 500) / 1000;
	const uint16_t minutes = seconds / 60;
	seconds -= minutes * 60;
	Serial << "Тест завершён!\r\n\r\nПроверено вариантов:\r\n\tпо периоду:"
		<< periodCounter << "\r\n\tпо частоте:" << frequencyCounter << "\r\nОшибок:" << errorCounter
		 << "\r\n\r\nВремя выполнения: " << (minutes < 10 ? "0":"") << minutes << ':' << (seconds < 10 ? "0":"") << seconds
		 << "\r\n";
}

void loop(void) {} 
