#ifndef	PINOPS_H
#define	PINOPS_H
//
///////////////////////////////////////////////////////
//
//	Недобиблиотека работы с 3-state пинами некоторых контроллеров AVR
// можно использовать как в среде Ардуино, так и AVR Studio.
//
//	Предоставляются четыре функции. Первые три сходны с аналогичными функциями среды Ардуино.
//
//	void pinMode(const int8_t pin, const uint8_t val);
//		pin – номер пина
//		val – может быть INPUT, OUTPUT или INPUT_PULLUP
//
//	bool digitalRead(const int8_t pin);
//		pin – номер пина
//
//	void digitalWrite(const int8_t pin, const bool val);
//		pin – номер пина
//		val – может быть HIGH или LOW
//
//	void pinToggle(const int8_t pin);
//		pin – номер пина
//
//	Последняя функция инвертирует значение пина одной процессорной командой. Не определена для ATmega32A.
//
//	Все четыре функции - статические методы класса PinOps, потому использовать их необходимо с префиксом 
//	PinOps::
// Например,
//
//		PinOps::digitalWrite(LED_BUILTIN, HIGH);
//
//
//	Если параметры заданы константами, то все функции компилируются в одну (в случае pinMode с
//	параметром INPUP_PULLUP – в две) команду процессора.
//
//	Если же параметры – переменные, то функции разворачиваются в некоторые последовательности команд,
//	заметно более экономичные и по памяти, и по бстродействию), чем штатные функции среды или макросы
//	библиотеки DigitalPins
//	(см. сравнение библиотек - http://arduino.ru/forum/programmirovanie/s-ili-%C2%ABsnova-ob-umenii-gotovit%C2%BB )
//
//
//
//	СООТВЕТСТВИЕ НОМЕРОВ ПИНОВ ПОРТАМ МИКРОКОНТРОЛЛЕРА
//
//	ATtiny25, ATtiny45, ATtiny85
//	----------------------------
//
//		пины 0-4 - PORTB, 0-4
//		PIN_RESET - PORTB, 5
//
//	ATmega48A, ATmega48PA, ATmega88A, ATmega88PA, ATmega168A, ATmega168PA, ATmega328, ATmega328P
//	--------------------------------------------------------------------------------------------
//
//		пины 0-7 - PORTD, 0-7
//		пины 8-13 - PORTB, 0-5
//		пины 14-19 - PORTC, 0-5
//		XTAL1_PIN - PORTB, 6
//		XTAL1_PIN - PORTB, 7
//		RESET_PIN - PORTC, 6
//
//	ATmega32A
//	---------
//
//		пины с 0 по 7 - PORTB 0-7
//		пины с 8 по 15 - PORTС 0-7
//		пины с 16 по 23 - PORTD 0-7
//		пины с 24 по 31 - PORTA 0-7
//
//
// ЛИЦЕНЗИЯ
//
// 1. Вы можете свободно использовать или не использовать его в коммерческих,
//    некоммерческих, и любых иных, не запрещённых законом, целях.
//
// 2. Автор не несёт решительно никакой ответственности за любые положительные
//    или отрицательные результаты использования или неиспользования данного кода.
//
// 3. Если Вам таки хочется сделать автору предъяву, то Вы знаете куда
//    Вам следует обратиться.
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
//////////////////////////////////////////

#include <stdint-gcc.h>

#ifndef INPUT
#define INPUT 0x0
#endif // !INPUT

#ifndef OUTPUT
#define OUTPUT 0x1
#endif // !OUTPUT

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 0x2
#endif // !INPUT_PULLUP

#ifndef HIGH
#define HIGH 0x1
#endif // !HIGH

#ifndef LOW
#define LOW 0x0
#endif // !LOW

#if (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
	#define	ATtiny85Detected	true
#else
	#define	ATtiny85Detected	false
#endif

#if 	(defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48PA__) || defined(__AVR_ATmega88A__) \
		|| defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) \
		|| defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__))
	#define	ATmega328Detected	true
#else
	#define	ATmega328Detected	false
#endif

#if (defined(__AVR_ATmega32A__))
	#define ATmega32ADetected true
#else
	#define ATmega32ADetected false
#endif

#if ATmega328Detected

#define A0	14
#define A1	15
#define A2	16
#define A3	17
#define A4	18
#define A5	19
#define XTAL1_PIN	20	//	(PCINT6/XTAL1/TOSC1) PB6
#define XTAL2_PIN	21	// (PCINT7/XTAL2/TOSC2) PB7
#define RESET_PIN	22	//	(PCINT14/RESET) PC6

#define GET_PORT(port, pin) (pin < 8) ? port##D : (pin < 14 || pin == XTAL1_PIN || pin == XTAL2_PIN) ? port##B : port##C
#define GET_MASK(pin) (1 << ((pin < 8) ? pin : (pin < 14) ? (pin - 8) : (pin < RESET_PIN) ? (pin - 14) : 6))

#define PARAMETER_PIN	const uint8_t pin

#elif ATtiny85Detected

#define RESET_PIN	5	//	(PCINT5/RESET/ADC0/dW) PB5

#define GET_PORT(port, pin) port##B
#define GET_MASK(pin) (1 << (pin))

#define CLEARPIN	(void)pin;

#define PARAMETER_PIN	const uint8_t

#elif ATmega32ADetected

#define A0	24
#define A1	25
#define A2	26
#define A3	27
#define A4	28
#define A5	29
#define A6	30
#define A7	31

#define GET_PORT(port, pin) (pin < 8) ? port##B : (pin < 16) ? port##C : (pin < 24) ? port##D : port##A
#define GET_MASK(pin) (1 << ((pin) % 8))

#define PARAMETER_PIN	const uint8_t pin

#else
#error The library does not seem to support your MCU
#endif

struct PinOps {

	#define AI	__attribute__((always_inline))

	static constexpr volatile uint8_t & getIn(PARAMETER_PIN) AI {
		return GET_PORT(PIN, pin);
	}

	static constexpr volatile uint8_t & getOut(PARAMETER_PIN) AI {
		return GET_PORT(PORT, pin);
	}

	static constexpr volatile uint8_t & getDDR(PARAMETER_PIN) AI {
		return GET_PORT(DDR, pin);
	}

	static constexpr uint8_t getBitMask(const uint8_t pin) AI {
		return  GET_MASK(pin);
	}

	static inline void digitalWrite(const int8_t pin, const bool val) AI {
		if (val) getOut(pin) |= getBitMask(pin);
		else getOut(pin) &= ~getBitMask(pin);
	}

	#if ATmega328Detected || ATtiny85Detected
	static inline void pinToggle(const int8_t pin) AI {
		getIn(pin) |= getBitMask(pin);
	}
	#endif

	static inline bool digitalRead(const int8_t pin) AI {
		return static_cast<bool>(getIn(pin) & getBitMask(pin));
	}

	static inline void pinMode(const int8_t pin, const uint8_t val) AI {
		if (val == OUTPUT) getDDR(pin) |= getBitMask(pin);
		else {
			getDDR(pin) &= ~getBitMask(pin);
			if (val == INPUT_PULLUP) getOut(pin) |= getBitMask(pin);
		}
	}

};

#endif	//	PINOPS_H
