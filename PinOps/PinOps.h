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
//	параметром INPUP_PULLUP – в две) команды процессора.
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
//		RESET_PIN - PORTB, 5
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
//		пины с 8 по 15 - PORTD 0-7
//		пины с 16 по 23 - PORTC 0-7
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

#include <SupportedMCU.h>
#include <bitmask.h>

#if mcuATmega328Detected

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

#elif mcuATtiny85Detected

#define A0	5
#define A1	2
#define A2	4
#define A3	3

#define RESET_PIN	5	//	(PCINT5/RESET/ADC0/dW) PB5

#define GET_PORT(port, pin) port##B
#define GET_MASK(pin) (1 << (pin))

#define CLEARPIN	(void)pin;

#define PARAMETER_PIN	const uint8_t pin

#elif mcuATmega32Detected

#define A0	24
#define A1	25
#define A2	26
#define A3	27
#define A4	28
#define A5	29
#define A6	30
#define A7	31

#define GET_PORT(port, pin) (pin < 8) ? port##B : (pin < 16) ? port##D : (pin < 24) ? port##C : port##A
#define GET_MASK(pin) (1 << ((pin) % 8))

#define PARAMETER_PIN	const uint8_t pin

#elif mcuATmega8515Detected

#define GET_PORT(port, pin) (pin < 8) ? port##B : (pin < 16) ? port##D : (pin < 24) ? port##C : port##A
#define GET_MASK(pin) (1 << ((pin) % 8))

#define PARAMETER_PIN	const uint8_t pin

#elif mcuATmega2560Detected

#define	A0		54
#define	A1		55
#define	A2		56
#define	A3		57
#define	A4		58
#define	A5		59
#define	A6		60
#define	A7		61
#define	A8		62
#define	A9		63
#define	A10	64
#define	A11	65
#define	A12	66
#define	A13	67
#define	A14	68
#define	A15	69

#define GET_PORT(port, pin)	\
	(pin > 21 && pin < 30) ? port##A :	\
	((pin > 9 && pin < 14) || (pin > 49 && pin < 54)) ? port##B :	\
	(pin > 29 && pin < 38) ? port##C :	\
	((pin > 17 && pin < 22) || (pin == 38)) ? port##D :	\
	((pin >= 0 && pin < 4) || (pin == 5)) ? port##E :	\
	(pin > 53 && pin < 62) ? port##F :	\
	((pin > 38 && pin < 42) || (pin == 5)) ? port##G :	\
	((pin > 5 && pin < 10) || (pin > 15 && pin < 18)) ? port##H :	\
	(pin > 13 && pin < 16) ? port##J :	\
	(pin > 61 && pin < 70) ? port##K : port##L

#define GET_MASK(pin)	\
	((pin == 0) || (pin == 15) || (pin == 17) || (pin == 21) || (pin == 22) || (pin == 37) || (pin == 41) || (pin == 49) || (pin == 53) || (pin == 54) || (pin == 62)) ? bitMask(0) :	\
	((pin == 1) || (pin == 14) || (pin == 16) || (pin == 20) || (pin == 23) || (pin == 36) || (pin == 40) || (pin == 48) || (pin == 52) || (pin == 55) || (pin == 63)) ? bitMask(1) :	\
	((pin == 19) || (pin == 24) || (pin == 35) || (pin == 39) || (pin == 47) || (pin == 51) || (pin == 56) || (pin == 64)) ? bitMask(2) :	\
	((pin == 5) || (pin == 6) || (pin == 18) || (pin == 25) || (pin == 34) || (pin == 46) || (pin == 50) || (pin == 57) || (pin == 65)) ? bitMask(3) :	\
	((pin == 2) || (pin == 7) || (pin == 10) || (pin == 26) || (pin == 33) || (pin == 45) || (pin == 58) || (pin == 66)) ? bitMask(4) :	\
	((pin == 3) || (pin == 4) || (pin == 8) || (pin == 11) || (pin == 27) || (pin == 32) || (pin == 44) || (pin == 59) || (pin == 67)) ? bitMask(5) :	\
	((pin == 9) || (pin == 12) || (pin == 28) || (pin == 31) || (pin == 43) || (pin == 60) || (pin == 68)) ? bitMask(6) :	bitMask(7)

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

	#if mcuATmega328Detected || mcuATtiny85Detected || mcuATmega2560Detected
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
