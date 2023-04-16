#ifndef	PRINTING_H
#define	PRINTING_H

// для печати в виде "A=321"
//#define printVar(x) do { Serial << #x "=" << (x) << "\r\n"; } while (false)
#define printVar(x) do { Serial.print(#x "="); Serial.print(x); Serial.print("; "); } while (false)
#define printVarLn(x) do { Serial.print(#x "="); Serial.println(x); } while (false)
#define printVar16(x) do { Serial.print(#x "=0x"); Serial.print(x, 16); Serial.print("; "); } while (false)
#define printVar16Ln(x) do { Serial.print(#x "=0x"); Serial.println(x, 16); } while (false)
	

template <typename T> inline Print & operator << (Print & s, T  n) { s.print(n); return s; }

// Поддержка __int24 (где-то что-то оказалось не так с версиями компилятора, пока убрал)
//#if	((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7))) 
//	inline Print & operator << (Print &s, __int24 n) { s.print((int32_t)n); return s; }
//	inline Print & operator << (Print &s, __uint24 n) { s.print((uint32_t)n); return s; }
//#endif

#endif	//		PRINTING_H
