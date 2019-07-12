/////////////////////////////////////////////////////////////////////////////////
//
//	Только ATmega328P и ATtiny25/45/85
//
//	1. Не является универсальной библиотекой и не предназначена для распространения
//
//	2. Групповые операции доступны только если оперелить #define GROUP_OPERATIONS
//	  2.1 РИСК групповых операций - они НЕ ПРОВЕРЯЮТ, что все пины от одного и того
//	      же порта, просто берут порт первого пина и сичтают, что остальные пины там же
//   2.2 БЕНЕФИТ групповых операций. Они сделаны так, что все пины изменяются
//       точно одновременно - ОДНОЙ командой процессора. Иногда это важно, например,
//		   если нужно, чтобы два сигнала шли точно в одинаковой фазе. В этих
//       случаях лучше воспользоваться digitalWrite на два пина, чем двумя по пину,
//       хотя последнее и эффективнее. Потому, кстати, нет никакого смысла в
//       мультипортовых групповых операциях - два порта одной командой не изменишь.
//
//   3. Если этот текст как-то попал к Вам, это Ваши проблемы. Автор не несёт никаких
//      обязательств ни перед кем, ни в каком случае.
//		  Текст исключительно для внутреннего использования.
//
// Используем операции с одним подчерком. Операции с большим количество подчерков - внутренняя кухня
//
//	Перечень операций
//
//	_pinMode(pin,state)
//	_digitalWrite(pin,val)
//	_digitalRead(pin)
//	_pinInvert(pin)	// Инвертирование пина
//	_pinPulse(pin)		// Инвертируется на 1 такт
//	_pulseHigh(pin)	// Высокий импульс в 1 такт
//	_pulseLow(pin)		// Низкий импульс в 1 такт
//
// Групповые операции N - 2-8
//	_digitalWriteN(p1,...,pN,val)
//	_pulseHighN(p1,...,pN)
//	_pulseLowN(p1,...,pN)
//	_pinInvertN(p1,...,pN)
//	_pinPulseN(p1,...,pN)
//	_digitalReadN(p1,...,pN) // выдаёт байт, пины можно снимать битовой маской
//
/////////////////////////////////////////



#ifndef DigitalPins_h
#define DigitalPins_h

#define USE_XTAL_PINS
#define USE_RESET_PIN

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))
#define bit(b) (1UL << (b))

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

#define	__AVR_ATtinyDetected__	(defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))

#if (!__AVR_ATtinyDetected__) && (!defined(__AVR_ATmega328P__))
#ifndef IGNORE_CPU_CHECK
#error The library has been well tested for ATmega328P and ATtiny25/45/85 only. Define IGNORE_CPU_CHECK option to ignore this check.
#endif // !IGNORE_CPU_CHECK
#endif // (!__AVR_ATtinyDetected__) && (!defined(__AVR_ATmega328P__))



//////////////////////////////////////////////////////////////
//
//	Маски для битов групповых операций
//
#define __orMask2(m1,m2)	((unsigned char)((m1) | (m2)))
#define __orMask3(m1,m2,m3)	((unsigned char)(__orMask2(m1,m2) | (m3)))
#define __orMask4(m1,m2,m3,m4)	((unsigned char)(__orMask3(m1,m2,m3) | (m4)))
#define __orMask5(m1,m2,m3,m4,m5)	((unsigned char)(__orMask4(m1,m2,m3,m4) | (m5)))
#define __orMask6(m1,m2,m3,m4,m5,m6)	((unsigned char)(__orMask5(m1,m2,m3,m4,m5) | (m6)))
#define __orMask7(m1,m2,m3,m4,m5,m6,m7)	((unsigned char)(__orMask6(m1,m2,m3,m4,m5,m6) | (m7)))
#define __orMask8(m1,m2,m3,m4,m5,m6,m7,m8)	((unsigned char)(__orMask7(m1,m2,m3,m4,m5,m6,m7) | (m8)))


#define	ERROR_BIT	255

#if __AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	У ATtiny есть единственный порт B,
// а номера пинов совпадают с номерами битов
//
#define __pin2Port(pin)	(PORTB)
#define __pin2DirectionPort(pin)	(DDRB)
#define __pin2InputPort(pin)	(PINB)
#define __pin2PortBit(pin)	(pin)

#else	//	Не __AVR_ATtinyDetected__

#define A0	14
#define A1	15
#define A2	16
#define A3	17
#define A4	18
#define A5	19

#ifdef USE_XTAL_PINS
	//	(PCINT6/XTAL1/TOSC1) PB6
	//	(PCINT7/XTAL2/TOSC2) PB7
	#define XTAL1_PIN	20
	#define XTAL2_PIN	21

	#define EXT_PORTB_PINN(pin)	(((pin) == XTAL1_PIN || (pin) == XTAL2_PIN) ? ((pin)-(XTAL1_PIN-6)) :
	#define EXT_PORTB_PINS(pin)	|| ((pin) == XTAL1_PIN) || ((pin) == XTAL2_PIN)
#else
	#define EXT_PORTB_PINS(pin)
	#define EXT_PORTB_PINN(pin)	(
#endif

#ifdef USE_RESET_PIN
//	(PCINT14/RESET) PC6
#define RESET_PIN	22

#define EXT_PORTC_PIN(pin)	|| ((pin) == RESET_PIN)
#define EXT_PORTC_PINN(pin)	(((pin) == RESET_PIN) ? 6 :
#else
#define EXT_PORTC_PIN(pin)
#define EXT_PORTC_PINN(pin) (	
#endif


//////////////////////////////////////////////////////////////
//
//	Выбор порта  и бита по пину для ATmega328
//
#define	ERROR_PORT	_SFR_IO8(255)

#define ___pin2Port(pin, portD, portB, portC)	\
(((pin) < 8) ? (portD) : \
(((pin) >= 8 && (pin) < 14) EXT_PORTB_PINS(pin)  ? (portB) : \
(((pin) >= A0 && (pin) <= A5) EXT_PORTC_PIN(pin) ? (portC) : ERROR_PORT)))

#define __pin2Port(pin) ___pin2Port(pin, PORTD, PORTB, PORTC)
#define __pin2DirectionPort(pin) ___pin2Port(pin, DDRD, DDRB, DDRC)
#define __pin2InputPort(pin) ___pin2Port(pin,PIND,PINB,PINC)

#define	__pin2PortBit(pin)	\
(((pin) < 8) ? (pin) : \
(((pin) >= 8 && (pin) < 14) ? ((pin)-8) : \
EXT_PORTB_PINN(pin)	\
EXT_PORTC_PINN(pin)	\
(((pin) >= A0 && (pin) <= A5) ? ((pin)-A0) : ERROR_BIT)))))

#endif	//	__AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	Битовые маски из номеров пинов
//
#define __pin2Mask(pin)	(1 << __pin2PortBit(pin))
#define __orPin2(p1,p2) __orMask2(__pin2Mask(p1),__pin2Mask(p2))
#define __orPin3(p1,p2,p3) __orMask3(__pin2Mask(p1),__pin2Mask(p2),__pin2Mask(p3))
#define __orPin4(p1,p2,p3,p4) __orMask4(__pin2Mask(p1),__pin2Mask(p2),__pin2Mask(p3),__pin2Mask(p4))
#define __orPin5(p1,p2,p3,p4,p5) __orMask5(__pin2Mask(p1),__pin2Mask(p2),__pin2Mask(p3),__pin2Mask(p4),__pin2Mask(p5))
#define __orPin6(p1,p2,p3,p4,p5,p6) __orMask6(__pin2Mask(p1),__pin2Mask(p2),__pin2Mask(p3),__pin2Mask(p4),__pin2Mask(p5),__pin2Mask(p6))
#define __orPin7(p1,p2,p3,p4,p5,p6,p7) __orMask7(__pin2Mask(p1),__pin2Mask(p2),__pin2Mask(p3),__pin2Mask(p4),__pin2Mask(p5),__pin2Mask(p6),__pin2Mask(p7))
#define __orPin8(p1,p2,p3,p4,p5,p6,p7,p8) __orMask8(__pin2Mask(p1),__pin2Mask(p2),__pin2Mask(p3),__pin2Mask(p4),__pin2Mask(p5),__pin2Mask(p6),__pin2Mask(p7),__pin2Mask(p8))

//////////////////////////////////////////////////////////////
//
//	Внутренняя реализация основных операций
//
#define __pinHigh(port,mask) ((port) |= (mask))
#define __pinLow(port,mask) ((port) &= ~(mask))
#define	__digitalWrite(port,mask,val) (((val)!=LOW) ? (__pinHigh(port,mask)) : (__pinLow(port,mask)))
#define	__pinInvert(port,mask) ((port) |= (mask))
#define __pinModeOutput(dirport,mask) ((dirport) |= (mask))
#define __pinModeInput(dirport, mask)  ((dirport) &= ~(mask))
#define __pinModeInputPullUp(port,dirport,mask) { unsigned char oldSREG = SREG; cli(); (dirport) &= ~(mask); (port) |= (mask); SREG = oldSREG; }
#define __pinMode(pin,state,mask) ((state) == INPUT) ? (__pinModeInput(__pin2DirectionPort(pin), mask)) : \
((state) == INPUT_PULLUP) ? (__pinModeInputPullUp(__pin2Port(pin), __pin2DirectionPort(pin), mask)) : \
((state) == OUTPUT) ? (__pinModeOutput(__pin2DirectionPort(pin), mask)) : (__pinModeOutput(__pin2DirectionPort(pin),0))


//////////////////////////////////////////////////////////////
//
//	Основные операции с единичным пином
//
#define _pinMode(pin,state) __pinMode(pin,state,__pin2Mask(pin))
#define	_digitalWrite(pin,val) __digitalWrite(__pin2Port(pin),__pin2Mask(pin),val)
#define	_digitalRead(pin)	(((__pin2InputPort(pin)) & (__pin2Mask(pin))) ? HIGH : LOW)
#define _pulseHigh(pin) do { unsigned char oldSREG = SREG; cli(); _digitalWrite(pin,HIGH); _digitalWrite(pin,LOW); SREG = oldSREG; } while(false)
#define	_pulseLow(pin)  do { unsigned char oldSREG = SREG; cli(); _digitalWrite(pin,LOW); _digitalWrite(pin,HIGH); SREG = oldSREG; } while(false)
#define	_pinPulse(pin)  do { unsigned char oldSREG = SREG; cli(); _pinInvert(pin); _pinInvert(pin); SREG = oldSREG; } while(false)
#define	_pinInvert(pin) __pinInvert(__pin2InputPort(pin),__pin2Mask(pin))


#if defined(GROUP_OPERATIONS)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//	ГРУППОВЫЕ ОРПЕРАЦИИ
//
//
//	Групповой pulseLow по маске (внутренняя операция)
//
#define	__pulseLow(port,mask)	\
asm volatile (	\
"in __tmp_reg__,__SREG__	\n\t"	\
"cli	\n\t"	\
"in r24,%0	\n\t"	\
"mov r23,r24 \n\t"	\
"or r24,%1	\n\t"	\
"com %1	\n\t"	\
"and r23,%1 \n\t"	\
"out %0,r23 \n\t"	\
"out %0,r24 \n\t"	\
"out __SREG__,__tmp_reg__	\n\t"	\
: : "I" (_SFR_IO_ADDR(port)), "r" ((mask)) : "r24", "r23"	\
)

//////////////////////////////////////////////////////////////
//
//	Групповой pulseHigh по маске (внутренняя операция)
//
#define	__pulseHigh(port,mask)	\
asm volatile (	\
"in __tmp_reg__,__SREG__	\n\t"	\
"cli	\n\t"	\
"in r24,%0	\n\t"	\
"mov r23,r24 \n\t"	\
"or r24,%1	\n\t"	\
"com %1	\n\t"	\
"and r23,%1 \n\t"	\
"out %0,r24 \n\t"	\
"out %0,r23 \n\t"	\
"out __SREG__,__tmp_reg__	\n\t"	\
: : "I" (_SFR_IO_ADDR(port)), "r" ((mask)) : "r24", "r23"	\
)

//////////////////////////////////////////////////////////////
//
//	Групповой Invert по маске (внутренняя операция)
//
#define	__Invert(port,mask)	\
asm volatile (	\
"in __tmp_reg__,__SREG__	\n\t"	\
"cli	\n\t"	\
"in r24,%0	\n\t"	\
"or r24,%1	\n\t"	\
"out %0,r24 \n\t"	\
"out __SREG__,__tmp_reg__	\n\t"	\
: : "I" (_SFR_IO_ADDR(port)), "r" ((mask)) : "r24"	\
)

//////////////////////////////////////////////////////////////
//
//	Групповой двойной Invert по маске (внутренняя операция)
//
#define	__2Invert(port,mask)	\
asm volatile (	\
"in __tmp_reg__,__SREG__	\n\t"	\
"cli	\n\t"	\
"in r24,%0	\n\t"	\
"or r24,%1	\n\t"	\
"out %0,r24 \n\t"	\
"out %0,r24 \n\t"	\
"out __SREG__,__tmp_reg__	\n\t"	\
: : "I" (_SFR_IO_ADDR(port)), "r" ((mask)) : "r24"	\
)


//////////////////////////////////////////////////////////////
//
//	Групповой _digitalWrite
//
#define	_digitalWrite2(p1,p2,val) __digitalWrite(__pin2Port(p1),__orPin2(p1,p2),val)
#define	_digitalWrite3(p1,p2,p3,val) __digitalWrite(__pin2Port(p1),__orPin3(p1,p2,p3),val)
#define	_digitalWrite4(p1,p2,p3,p4,val) __digitalWrite(__pin2Port(p1),__orPin4(p1,p2,p3,p4),val)
#define	_digitalWrite5(p1,p2,p3,p4,p5,val) __digitalWrite(__pin2Port(p1),__orPin5(p1,p2,p3,p4,p5),val)
#define	_digitalWrite6(p1,p2,p3,p4,p5,p6,val) __digitalWrite(__pin2Port(p1),__orPin6(p1,p2,p3,p4,p5,p6),val)
#if !__AVR_ATtinyDetected__
#define	_digitalWrite7(p1,p2,p3,p4,p5,p6,p7,val) __digitalWrite(__pin2Port(p1),__orPin7(p1,p2,p3,p4,p5,p6,p7),val)
#define	_digitalWrite8(p1,p2,p3,p4,p5,p6,p7,p8,val) __digitalWrite(__pin2Port(p1),__orPin8(p1,p2,p3,p4,p5,p6,p7,p8),val)
#endif	//	__AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	Групповой _pulseHigh
//
#define _pulseHigh2(p1,p2) __pulseHigh(__pin2Port(p1),__orPin2(p1,p2))
#define _pulseHigh3(p1,p2,p3) __pulseHigh(__pin2Port(p1),__orPin3(p1,p2,p3))
#define _pulseHigh4(p1,p2,p3,p4) __pulseHigh(__pin2Port(p1),__orPin4(p1,p2,p3,p4))
#define _pulseHigh5(p1,p2,p3,p4,p5) __pulseHigh(__pin2Port(p1),__orPin5(p1,p2,p3, p4, p5))
#define _pulseHigh6(p1,p2,p3,p4,p5,p6) __pulseHigh(__pin2Port(p1),__orPin6(p1,p2, p3, p4, p5, p6))
#if !__AVR_ATtinyDetected__
#define _pulseHigh7(p1,p2,p3,p4,p5,p6,p7) __pulseHigh(__pin2Port(p1),__orPin7(p1, p2, p3, p4, p5, p6, p7))
#define _pulseHigh8(p1,p2,p3,p4,p5,p6,p7,p8) __pulseHigh(__pin2Port(p1),__orPin8(p1, p2, p3, p4, p5, p6, p7, p8))
#endif	//	__AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	Групповой _pulseLow
//
#define _pulseLow2(p1,p2) __pulseLow(__pin2Port(p1),__orPin2(p1,p2))
#define _pulseLow3(p1,p2,p3) __pulseLow(__pin2Port(p1),__orPin3(p1,p2,p3))
#define _pulseLow4(p1,p2,p3,p4) __pulseLow(__pin2Port(p1),__orPin4(p1,p2,p3,p4))
#define _pulseLow5(p1,p2,p3,p4,p5) __pulseLow(__pin2Port(p1),__orPin5(p1,p2,p3, p4, p5))
#define _pulseLow6(p1,p2,p3,p4,p5,p6) __pulseLow(__pin2Port(p1),__orPin6(p1,p2, p3, p4, p5, p6))
#if !__AVR_ATtinyDetected__
#define _pulseLow7(p1,p2,p3,p4,p5,p6,p7) __pulseLow(__pin2Port(p1),__orPin7(p1, p2, p3, p4, p5, p6, p7))
#define _pulseLow8(p1,p2,p3,p4,p5,p6,p7,p8) __pulseLow(__pin2Port(p1),__orPin8(p1, p2, p3, p4, p5, p6, p7, p8))
#endif	//	__AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	Групповой _pinInvert
//
#define _pinInvert2(p1,p2) __Invert(__pin2InputPort(p1),__orPin2(p1,p2))
#define _pinInvert3(p1,p2,p3) __Invert(__pin2InputPort(p1),__orPin3(p1,p2,p3))
#define _pinInvert4(p1,p2,p3,p4) __Invert(__pin2InputPort(p1),__orPin4(p1,p2,p3, p4))
#define _pinInvert5(p1,p2,p3,p4,p5) __Invert(__pin2InputPort(p1),__orPin5(p1,p2, p3, p4, p5))
#define _pinInvert6(p1,p2,p3,p4,p5,p6) __Invert(__pin2InputPort(p1),__orPin6(p1, p2, p3, p4, p5, p6))
#if !__AVR_ATtinyDetected__
#define _pinInvert7(p1,p2,p3,p4,p5,p6,p7) __Invert(__pin2InputPort(p1),__orPin7(p1, p2, p3, p4, p5, p6, p7))
#define _pinInvert8(p1,p2,p3,p4,p5,p6,p7,p8) __Invert(__pin2InputPort(p1),__orPin8(p1, p2, p3, p4, p5, p6, p7, p8))
#endif	//	__AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	Групповой _pinPulse
//
#define _pinPulse2(p1,p2) __2Invert(__pin2InputPort(p1),__orPin2(p1,p2))
#define _pinPulse3(p1,p2,p3) __2Invert(__pin2InputPort(p1),__orPin3(p1,p2,p3))
#define _pinPulse4(p1,p2,p3,p4) __2Invert(__pin2InputPort(p1),__orPin4(p1,p2,p3, p4))
#define _pinPulse5(p1,p2,p3,p4,p5) __2Invert(__pin2InputPort(p1),__orPin5(p1,p2, p3, p4, p5))
#define _pinPulse6(p1,p2,p3,p4,p5,p6) __2Invert(__pin2InputPort(p1),__orPin6(p1, p2, p3, p4, p5, p6))
#if !__AVR_ATtinyDetected__
#define _pinPulse7(p1,p2,p3,p4,p5,p6,p7) __2Invert(__pin2InputPort(p1),__orPin7(p1, p2, p3, p4, p5, p6, p7))
#define _pinPulse8(p1,p2,p3,p4,p5,p6,p7,p8) __2Invert(__pin2InputPort(p1),__orPin8(p1, p2, p3, p4, p5, p6, p7, p8))
#endif	//	__AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	Групповой _digitalRead
//
#define	__read(port, mask) ((port) & (mask))
#define	_digitalRead2(p1,p2) __read(__pin2InputPort(p1),__orPin2(p1,p2))
#define	_digitalRead3(p1,p2,p3) __read(__pin2InputPort(p1),__orPin3(p1,p2,p3))
#define	_digitalRead4(p1,p2,p3,p4) __read(__pin2InputPort(p1),__orPin4(p1,p2,p3,p4))
#define	_digitalRead5(p1,p2,p3,p4,p5) __read(__pin2InputPort(p1),__orPin5(p1,p2,p3,p4,p5))
#define	_digitalRead6(p1,p2,p3,p4,p5,p6) __read(__pin2InputPort(p1),__orPin6(p1,p2,p3,p4,p5,p6))
#if !__AVR_ATtinyDetected__
#define	_digitalRead7(p1,p2,p3,p4,p5,p6,p7) __read(__pin2InputPort(p1),__orPin7(p1,p2,p3,p4,p5,p6,p7))
#define	_digitalRead8(p1,p2,p3,p4,p5,p6,p7,p8) __read(__pin2InputPort(p1),__orPin8(p1,p2,p3,p4,p5,p6,p7,p8))
#endif	//	__AVR_ATtinyDetected__

//////////////////////////////////////////////////////////////
//
//	Маски для проверки отдельных битов после группового _digitalRead
//
#define _pin0	__pin2Mask(0)
#define _pin1	__pin2Mask(1)
#define _pin2	__pin2Mask(2)
#define _pin3	__pin2Mask(3)
#define _pin4	__pin2Mask(4)
#define _pin5	__pin2Mask(5)
#if !__AVR_ATtinyDetected__
#define _pin6	__pin2Mask(6)
#define _pin7	__pin2Mask(7)
#define _pin8	__pin2Mask(8)
#define _pin9	__pin2Mask(9)
#define _pin10	__pin2Mask(10)
#define _pin11	__pin2Mask(11)
#define _pin12	__pin2Mask(12)
#define _pin13	__pin2Mask(13)
#define _pinA0	__pin2Mask(A0)
#define _pinA1	__pin2Mask(A1)
#define _pinA2	__pin2Mask(A2)
#define _pinA3	__pin2Mask(A3)
#define _pinA4	__pin2Mask(A4)
#define _pinA5	__pin2Mask(A5)

#ifdef USE_XTAL_PINS
	#define _pinXTAL1	__pin2Mask(XTAL1_PIN)
	#define _pinXTAL2	__pin2Mask(XTAL2_PIN)
#endif

#ifdef USE_RESET_PIN
	#define _pinRESET	__pin2Mask(RESET_PIN)
#endif

#endif	//	!__AVR_ATtinyDetected__
#endif //	defined(GROUP_OPERATIONS)
#endif	//	DigitalPins_h
