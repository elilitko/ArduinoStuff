#ifndef	PRINTING_H
#define	PRINTING_H

// для печати в виде "A=321"
#define printVar(x) do { Serial.print(#x "=");Serial.println(x); } while (false)

template <typename T> inline Print & operator << (Print &s, T n) { s.print(n); return s; }

#endif	//		PRINTING_H
