#ifndef	PRINTING_H
#define	PRINTING_H

// для печати в виде "A=321"
#define printVar(x) do { Serial << #x "=" << (x) << "\r\n"; } while (false)

template <typename T> inline Print & operator << (Print &s, T n) { s.print(n); return s; }

#if	((__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7))) 
	inline Print & operator << (Print &s, __int24 n) { s.print((int32_t)n); return s; }
	inline Print & operator << (Print &s, __uint24 n) { s.print((uint32_t)n); return s; }
#endif


#endif	//		PRINTING_H
