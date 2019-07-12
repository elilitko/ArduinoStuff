#ifndef	PRINTING_H
#define	PRINTING_H

template <typename T> inline Print & operator << (Print &s, T n) { s.print(n); return s; }

#endif	//		PRINTING_H