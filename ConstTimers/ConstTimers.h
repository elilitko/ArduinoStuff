#ifndef	CONSTTIMERS_H
#define	CONSTTIMERS_H

///////////////////////////////////////////////////////////////////////////////
//
// Модификация недобиблиотеки от ЕвгенийП.
// Выполнена: Ворота, 11.07.2019.
// 
// Основные изменения:
// 1.	К работе с частотами добавлена работа с периодами/интервалами;
// 2.	Добавлено задание максимальной допустимой погрешности количества тактов;
// 3.	Появилась возможность выдавать ошибку компиляции в случае выхода за допустимую погрешность;
// 4.	Добавлены суффиксы для работы с временными интервалами (на основе другого кода от ЕвгенийП);
// 5.	Добавлена поддержка Мега, Леонардо и ATmega32A
// 6.	Все «внутренние» функции спрятаны в своё пространство имён.
// 
// ---------------
//
// Обязательна опция -std=gnu++11 или -std=С++11 (можно 14 или 17)
// 
// Определяются биты конфигурации делителя частоты таймера и количество тактов с данным делителем для 
// получения нужного интервала срабатывания таймера. Интервал задаётся либо частотой, либо периодом 
// (длительностью, если речь идёт об одиночном сигнале, например, по переполнению). 
// Соответственно, имеются четыре функции:
// 
// uint8_t prescalerBitsByPeriod(int8_t nTimer, uint32_t period);
// uint16_t timerTicksByPeriod(int8_t nTimer, uint32_t period, int8_t error = 0);
// 
// uint8_t prescalerBitsByFrequency(int8_t nTimer, uint32_t frequency);
// uint16_t timerTicksByFrequency(int8_t nTimer, uint32_t frequency, int8_t error = 0);
// 
// Первый параметр у всех функций – номер таймера. Параметры period и frequency – соответственно  
// требуемые период в тактах микроконтроллера (см. ниже о суффиксах) и частота (в герцах).   
// Параметр error задаёт допустимое отклонение от запрошенных характеристик в процентах. 
// Если не удаётся получить решение в пределах допустимого отклонения, то timerTicksByХХХ вернут 0.
// 
// Также имеется функция, возвращающая значение делителя частоты заданного таймера по битам конфигурации, 
// полученным от функций prescalerBitsByХХХ.
// 
// int16_t prescalerValue(uint8_t nTimer, uint8_t bits);
// 
// Для удобства работы с временными интервалами добавлены суффиксы _clk, _us, _ms и _sec. Таким образом, 
// второй параметр функций prescalerBitsByPeriod и timerTicksByPeriod (время в тактах микроконтроллера) 
// можно задавать при помощи суффиксов, например:
//
//		100 или 100_clk – сто тактов микроконтроллера;
//		100_us – сто микросекунд;
//		100_ms – сто миллисекунд;
//		2_sec – две секунды.
//		
// Допускаются также арифметические операции, например, запись
//
//		2_ms + 50_us
//
// вполне допустима и означает 2050 микросекунд.
//
//	ИСПОЛЬЗОВАНИЕ
//
// Для использования необходимо включить файл «ConstTimers.h» и определить константы с
// модификатором constexpr для нужных конфигурационных битов и количества тиков, которым
// присвоить значения, возвращаемые функциям prescalerBitsByХХХ и timerTicksByХХХ соответственно.
//
// Если использовать именно так, то код библиотеки целиком исполняется во время компиляции
// и не оказывает никакого влияния ни на размер занимаемой памяти, ни на код программы.
// В коде программы будут вставлены готовые (вычисленные на этапе компиляции) константы.
//
// Это позволяет организовать проверку полученного на этапе компиляции значения (функции
// timerTicksByХХХ возвращают 0 в случае невозможности соблюсти необходимую точность)
// и прервать компиляцию с соответствующим сообщением об ошибке при помощи static_assert.
//
//	Пример рекомендуемого использования:
//
//		/////////////////////////////////////////////////
//		//
//		//	Получаем меандр с частотой 50Гц на 9-ом пине
//		//	для этого заводим его на СТС и инвертированием
//		//	на частоте 100Гц (как раз 100/2 = 50)
//		//
//		#include <ConstTimers.h>
//
//		#define	TIMER	1
//		#define	FREQUENCY	100
//
//		static constexpr uint8_t prescalerBits = prescalerBitsByFrequency(TIMER, FREQUENCY);
//		static constexpr uint16_t timerTicks = timerTicksByFrequency(TIMER, FREQUENCY);
//
//		// проверка установилась ли частота (например, при частоте 101 будет ошибка компиляции)
//		static_assert(timerTicks, "The required frequency with a desired accuracy is unattainable for timer/counter");
//
//		void setup() {
//			pinMode(9, OUTPUT);
//			TCCR1A = bit(COM1A0);	// Инвертирование пина 9 по сравнению
//			TCCR1B = bit(WGM12) | prescalerBits;	// Установить СТС режим и делитель частоты
//			OCR1A = timerTicks;	// установить TOP равным timerTicks
//		}
//
//		void loop() {}
//
//Результат работы будет точно такой же, как если написать константы, вместо вычислений:
//
//		void setup() {
//			pinMode(9, OUTPUT);
//			TCCR1A = bit(COM1A0);
//			TCCR1B = bit(WGM12) | 2;
//			OCR1A = 20000;
//		}
//		void loop() {}
//
// Ни на один байт код не изменится.
//
//	КОНФИГУРАЦИЯ:
//
//	1. расчёт производится для текущей тактовой частоты микроконтроллера. Если нужно
//		считать для какой-то другой частоты, измените константу FCPU.
//
//	2. STimerParameters - массив конфигурации таймеров. Количество элементов массива
//		соответствует  количеству таймеров. Нулевой элемент описывает нулевой таймер,
//		первый элемент – первый таймер и т.д. Если требуется расширить недобиблиотеку для других
//		микроконтроллеров, нужно изменить именно этот массив и больше изменять ничего не надо.
//
//	В массиве для каждого таймера указано:
//		1) разрядность таймера в виде максимально возможного значения количества тиков
//			(для 8-разрядных таймеров – 0xFF, для 16-разрядных – 0xFFFF.
//		2) указатель на массив делителей частоты. Делители начинаются с 1 (делитель 0 писать не нужно)
//		3) количество делителей частоты у данного таймера.
//
// ЛИЦЕНЗИЯ
//
// Данный код поставляется по лицензии ПНХ.
//
// 1. Вы можете свободно использовать или не использовать его в коммерческих,
//    некоммерческих, и любых иных, не запрещённых законом, целях.
//
// 2. Автор не несёт решительно никакой ответственности за любые положительные
//    или отрицательные результаты использования или неиспользования данного кода.
//
// 3. Если Вам таки хочется сделать автору предъяву, то Вы знаете, куда
//    Вам следует обратиться. А если не знаете, то см. название лицензии.
//
// 4. Если данный код вдруг Вам пригодился (как учебник или ещё как что) и Вам
//    почему-либо (ну, приболели, может) захотелось отблагодарить автора рублём,
//    то это всегда пожалуйста – WebMoney, кошелёк № R626206676373
//
// 5. Возникновение или невозникновение у Вас желаний, обозначенных в п.4
//     настоящей лицензии никак не связано с п.1, который действует безусловно
//     и независимо от п.4.
//
// 6. Если данный код нужен Вам с какой-либо другой лицензией, например, с
//     сопровождением или Вы нуждаетесь во внесении изменений, свяжитесь с автором
//     на предмет заключения договора гражданско-правового характера.
//
///////////////////////////////////////////////////////////////////////////////

#define ATtiny85Detected	(defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
#define ATmega328Detected	(defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48PA__) || defined(__AVR_ATmega88A__) \
	|| defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) \
	|| defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__))
#define ATmega2561Detected	(defined(__AVR_ATmega640__) || defined(__AVR_ATmega640V__) || defined(__AVR_ATmega1280__) \
	|| defined(__AVR_ATmega1280V__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega1281V__) \
	|| defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2560V__) || defined(__AVR_ATmega2561__) || defined(__AVR_ATmega2561V__))
#define ATmega32U4Detected (defined(__AVR_ATmega32U4__))
#define ATmega32ADetected (defined(__AVR_ATmega32A__))

#include <ctype.h>

#define	FCPU	F_CPU

namespace constTimersNS {

struct STimerParameters {
	const uint32_t maxValue;
	const int16_t * prescalers;
	const uint8_t totalPrescalers;
};

static constexpr uint32_t	bits8 = 0x000000FFul;
static constexpr uint32_t	bits10 = 0x000003FFul;
static constexpr uint32_t	bits16 = 0x0000FFFFul;


#if ATmega328Detected || ATmega32ADetected

static constexpr  int16_t prescalers01[] = { 1, 8, 64, 256, 1024 };
static constexpr  int16_t prescalers2[] = { 1, 8, 32, 64, 128, 256, 1024 };

static constexpr STimerParameters timerParameters[] = {
	{ bits8, prescalers01, sizeof(prescalers01) / sizeof(prescalers01[0]) },
	{ bits16, prescalers01, sizeof(prescalers01) / sizeof(prescalers01[0]) },
	{ bits8, prescalers2, sizeof(prescalers2) / sizeof(prescalers2[0]) }
};

#elif	ATmega32U4Detected

static constexpr  int16_t prescalers013[] = { 1, 8, 64, 256, 1024 };
static constexpr  int16_t prescalers2[] = { 1, 8, 32, 64, 128, 256, 1024 };
static constexpr  int16_t prescalers4[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384 };

static constexpr STimerParameters timerParameters[] = {
	{ bits8, prescalers013, sizeof(prescalers013) / sizeof(prescalers013[0]) },
	{ bits16, prescalers013, sizeof(prescalers013) / sizeof(prescalers013[0]) },
	{ bits8, prescalers2, sizeof(prescalers2) / sizeof(prescalers2[0]) },
	{ bits16, prescalers013, sizeof(prescalers013) / sizeof(prescalers013[0]) },
	{ bits10, prescalers4, sizeof(prescalers4) / sizeof(prescalers4[0]) }
};

#elif ATmega2561Detected

static constexpr  int16_t prescalers01345[] = { 1, 8, 64, 256, 1024 };
static constexpr  int16_t prescalers2[] = { 1, 8, 32, 64, 128, 256, 1024 };

static constexpr STimerParameters timerParameters[] = {
	{ bits8, prescalers01345, sizeof(prescalers01345) / sizeof(prescalers01345[0]) },
	{ bits16, prescalers01345, sizeof(prescalers01345) / sizeof(prescalers01345[0]) },
	{ bits8, prescalers2, sizeof(prescalers2) / sizeof(prescalers2[0]) },
	{ bits16, prescalers01345, sizeof(prescalers01345) / sizeof(prescalers01345[0]) },
	{ bits16, prescalers01345, sizeof(prescalers01345) / sizeof(prescalers01345[0]) },
	{ bits16, prescalers01345, sizeof(prescalers01345) / sizeof(prescalers01345[0]) }
};

#elif ATtiny85Detected

static constexpr  int16_t prescalers0[] = { 1, 8, 64, 256, 1024 };
static constexpr  int16_t prescalers1[] = { 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384 };

static constexpr STimerParameters timerParameters[] = {
	{ bits8, prescalers0, sizeof(prescalers0) / sizeof(prescalers0[0]) },
	{ bits8, prescalers1, sizeof(prescalers1) / sizeof(prescalers1[0]) }
};

#else

#error The library does not seem to support your MCU

#endif

constexpr int8_t totalTimers = sizeof(timerParameters) / sizeof(timerParameters[0]);

static constexpr uint32_t getPeriod(const uint32_t frequency) {
	return (FCPU + frequency / 2) / frequency;
}

static constexpr uint16_t prValue(const int8_t prescalerId, const int8_t nTimer) {
	return timerParameters[nTimer].prescalers[prescalerId];
}

static constexpr uint32_t getDesiredTicks(const uint32_t period, const int8_t prescalerId, const int8_t nTimer) {
	return (period + prValue(prescalerId, nTimer) / 2) / prValue(prescalerId, nTimer);
}

static constexpr uint32_t correctTicks(uint32_t dTicks, const uint32_t maxValue) {
	//if (dTicks > maxValue) return maxValue; else return dTicks; // начиная с C++14
	return dTicks > maxValue ? maxValue : dTicks;
}

static constexpr uint32_t getTicks(const uint32_t period, const int8_t prescalerId, const int8_t nTimer) {
	return prescalerId >= timerParameters[nTimer].totalPrescalers ? 0x1FFFFFFF :
	correctTicks(getDesiredTicks(period, prescalerId, nTimer), timerParameters[nTimer].maxValue);
}

static constexpr uint32_t getBits(const int8_t prescalerId, const int8_t nTimer) {
	return prescalerId >= timerParameters[nTimer].totalPrescalers ? timerParameters[nTimer].totalPrescalers : prescalerId + 1;
}


static constexpr int32_t absErrorTicks(const uint32_t period, const uint32_t ticks) {
	return period > ticks ? period - ticks : ticks - period;
}

static constexpr int32_t absError(const uint32_t period, const int8_t prescalerId, const int8_t nTimer) {
	return prescalerId >= timerParameters[nTimer].totalPrescalers ? 0x1FFFFFFF :
	absErrorTicks(period, getTicks(period, prescalerId, nTimer) * prValue(prescalerId, nTimer));
}

static constexpr uint8_t getPrescalerId(const uint32_t error, const uint32_t newError, const uint8_t prId, const uint8_t candidate, const uint32_t period, const int8_t nTimer) {
	return
	(prId >= timerParameters[nTimer].totalPrescalers) ? candidate
	: getPrescalerId(newError, absError(period, prId + 1, nTimer), prId + 1, (error <= newError) ? candidate : prId, period, nTimer);
}

static constexpr double calcErrorPercentage(const double fcpu, const double perc) {
	return (fcpu < perc ? 1.0 - fcpu / perc : fcpu / perc - 1.0) * 100.0;
}

static constexpr uint16_t returnTicksOrZero(const uint32_t left, const uint32_t right, const int8_t allowableError, const uint32_t ticks) {
	return left == right || calcErrorPercentage(left, right) <= allowableError ? ticks : 0;
}

static constexpr uint16_t getTimerTicksIntFTicks(const uint32_t ticks, const int8_t nTimer, const uint32_t freq, uint8_t prescalerId, const int8_t allowableError) {
	return returnTicksOrZero(FCPU, freq * ticks * timerParameters[nTimer].prescalers[prescalerId], allowableError, ticks);
}

static constexpr uint16_t getTimerTicksIntF(const int8_t nTimer, const uint32_t freq, uint8_t prescalerId, const int8_t allowableError) {
	return getTimerTicksIntFTicks(getTicks(getPeriod(freq), prescalerId, nTimer), nTimer, freq, prescalerId, allowableError);
}

static constexpr uint16_t getTimerTicksIntTTicks(const uint32_t ticks, const int8_t nTimer, const uint32_t period, uint8_t prescalerId, const int8_t allowableError) {
	return returnTicksOrZero(period, ticks * timerParameters[nTimer].prescalers[prescalerId], allowableError, ticks);
}

static constexpr uint16_t getTimerTicksIntT(const int8_t nTimer, const uint32_t period, uint8_t prescalerId, const int8_t allowableError) {
	return getTimerTicksIntTTicks(getTicks(period, prescalerId, nTimer), nTimer, period, prescalerId, allowableError);
}


//////////////////////////////////////////////////////////////////
//
//	Суффиксы литералов для задания времени
//		100 или 100_clk - время в тактах контроллера (100 тактов)
//		100_us - время в микросекундах (100 микросекунд)
//		100_ms - время в милисекундах (100 милисекунд)
//		5_sec - время в секундах (5 секунд)
//
static constexpr int8_t __sLen(const char * str, const int8_t candidate = 0) { return (!*str) ? candidate : __sLen(str + 1, candidate + 1); }
static constexpr bool __isFin(const char * str, const int8_t lInd) { return lInd < 0 || !isdigit(str[lInd]); }
static constexpr uint32_t __toDig(const char * str, const uint32_t lg, const int8_t lInd) { return (str[lInd] - '0') * lg; }
static constexpr uint32_t ___s2ul(const char * str, const uint32_t, const int8_t);
static constexpr uint32_t __nextL(const char * str, const uint32_t lg, const int8_t lInd) { return __toDig(str, lg, lInd) + ___s2ul(str, lg * 10, lInd - 1); }
static constexpr uint32_t ___s2ul(const char * str, const uint32_t lg, const int8_t lInd) { return __isFin(str, lInd) ? 0 : __nextL(str, lg, lInd); }
static constexpr uint32_t __s2ul(const char * str) { return ___s2ul(str, 1, __sLen(str) - 1); }

} //	namespace constTimersNS

static constexpr uint32_t operator "" _clk(const char * s) { return constTimersNS::__s2ul(s); }
static constexpr uint32_t operator "" _us(const char * s) { return operator "" _clk(s) * (FCPU / 1000000ul); }
static constexpr uint32_t operator "" _ms(const char * s) { return operator "" _us(s) * 1000ul; }
static constexpr uint32_t operator "" _sec(const char * s) { return operator "" _ms(s) * 1000ul; }
//
//////////////////////////////////////////////////////////////////


static constexpr uint8_t prescalerBitsByPeriod(const int8_t nTimer, const uint32_t period) {
	return constTimersNS::getBits(constTimersNS::getPrescalerId(0x1FFFFFul, constTimersNS::absError(period, 0, nTimer), 0, 0, period, nTimer), nTimer);
}

static constexpr uint16_t timerTicksByPeriod(const int8_t nTimer, const uint32_t period, const int8_t allowableError = 0) {
	return constTimersNS::getTimerTicksIntT(nTimer, period, constTimersNS::getPrescalerId(0x1FFFFFul, constTimersNS::absError(period, 0, nTimer), 0, 0, period, nTimer), allowableError);
}

static constexpr uint8_t prescalerBitsByFrequency(const int8_t nTimer, const uint32_t freq) {
	return prescalerBitsByPeriod(nTimer, constTimersNS::getPeriod(freq));
}

static constexpr uint16_t timerTicksByFrequency(const int8_t nTimer, const uint32_t freq, const int8_t allowableError = 0) {
	return constTimersNS::getTimerTicksIntF(nTimer, freq, 
			constTimersNS::getPrescalerId(0x1FFFFFul, constTimersNS::absError(constTimersNS::getPeriod(freq), 0, nTimer), 0, 0, 
			constTimersNS::getPeriod(freq), nTimer), allowableError);
}

static constexpr int16_t prescalerValue(const uint8_t nTimer, const uint8_t bits) {
	return constTimersNS::timerParameters[nTimer].prescalers[bits-1];
}


#endif	//	CONSTTIMERS_H
