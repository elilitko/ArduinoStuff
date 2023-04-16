#ifndef	BITMASK_H
#define	BITMASK_H

inline constexpr uint8_t bitMask(const uint8_t m1) { return 1 << m1; }
inline constexpr uint8_t bitMask(const uint8_t m1, const uint8_t m2) { return bitMask(m1) | bitMask(m2); }
inline constexpr uint8_t bitMask(const uint8_t m1, const uint8_t m2, const uint8_t m3) { return bitMask(m2, m3) | bitMask(m1); }
inline constexpr uint8_t bitMask(const uint8_t m1, const uint8_t m2, const uint8_t m3, const uint8_t m4) { return bitMask(m2, m3, m4) | bitMask(m1); }
inline constexpr uint8_t bitMask(const uint8_t m1, const uint8_t m2, const uint8_t m3, const uint8_t m4, const uint8_t m5) { return bitMask(m2, m3, m4, m5) | bitMask(m1); }
inline constexpr uint8_t bitMask(const uint8_t m1, const uint8_t m2, const uint8_t m3, const uint8_t m4, const uint8_t m5, const uint8_t m6) { return bitMask(m2, m3, m4, m5, m6) | bitMask(m1); }
inline constexpr uint8_t bitMask(const uint8_t m1, const uint8_t m2, const uint8_t m3, const uint8_t m4, const uint8_t m5, const uint8_t m6, const uint8_t m7) { return bitMask(m2, m3, m4, m5, m6, m7) | bitMask(m1); }
inline constexpr uint8_t bitMask(const uint8_t m1, const uint8_t m2, const uint8_t m3, const uint8_t m4, const uint8_t m5, const uint8_t m6, const uint8_t m7, const uint8_t m8) { return bitMask(m2, m3, m4, m5, m6, m7, m8) | bitMask(m1); }

#endif	//		BITMASK_H
