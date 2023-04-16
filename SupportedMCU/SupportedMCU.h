#ifndef	SUPPORTED_MCU_H
#define	SUPPORTED_MCU_H

#if (defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__))
	#define	mcuATtiny85Detected	true
#else
	#define	mcuATtiny85Detected	false
#endif

#if (defined(__AVR_ATmega8515__) || defined(__AVR_ATmega8515L__))
	#define	mcuATmega8515Detected	true
#else
	#define	mcuATmega8515Detected	false
#endif

#if 	(defined(__AVR_ATmega48A__) || defined(__AVR_ATmega48PA__) || defined(__AVR_ATmega88A__) \
		|| defined(__AVR_ATmega88PA__) || defined(__AVR_ATmega168A__) || defined(__AVR_ATmega168PA__) \
		|| defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__))
	#define	mcuATmega328Detected	true
#else
	#define	mcuATmega328Detected	false
#endif

#if 	(defined(__AVR_ATmega640__) || defined(__AVR_ATmega640V__) || defined(__AVR_ATmega1280__) \
		|| defined(__AVR_ATmega1280V__) || defined(__AVR_ATmega1281__) || defined(__AVR_ATmega1281V__) \
		|| defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2560V__) || defined(__AVR_ATmega2561__) \
		|| defined(__AVR_ATmega2561V__))
	#define mcuATmega2560Detected true
#else
	#define mcuATmega2560Detected false
#endif

#if (defined(__AVR_ATmega32A__) || defined (__AVR_ATmega32__))
	#define mcuATmega32Detected true
#else
	#define mcuATmega32Detected false
#endif

#if defined(__AVR_ATmega32U4__)
	#define mcuATmega32U4Detected true
#else
	#define mcuATmega32U4Detected false
#endif

#endif	//	SUPPORTED_MCU_H
