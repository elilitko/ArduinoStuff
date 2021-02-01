#include <Arduino.h>
#include "ReedSolomon.h"

namespace ReedSolomonEncoding {

static constexpr uint8_t ErrorMax = 127;	// Максимально возможно количество ошибок для GF256

static uint8_t arraySize;
static uint8_t * pGlobal = nullptr;

//
//	Смещения данных, общих для кодирования и декодирования
#define	pBuffer (* reinterpret_cast<uint8_t **>(pGlobal))	// глобальный буфер
#define	errorsAmount (pGlobal[sizeof(uint8_t *)])	//	допустимое количество ошибок
#define	doubledErrorsAmount (pGlobal[sizeof(uint8_t *) + sizeof(uint8_t)])	// удвоенное errorsAmount
#define	bufferLength (pGlobal[sizeof(uint8_t *) + 2 * sizeof(uint8_t)])	//	длина буфера данных
#define	syndrome	(pGlobal + (sizeof(uint8_t *) + 3 * sizeof(uint8_t)))	// синдром ошибки
//
#define commonDataLen(e)	(sizeof(uint8_t *) + 3 * sizeof(uint8_t) + (e) * 2 + 1)
//
//	Смещения начала уникальных для кодирования и декодирования данных
#define	dUnique	(sizeof(uint8_t *) + 3 * sizeof(uint8_t) + arraySize)
//
//	Смещения данных для кодирования
#define	mainPolynome	(pGlobal + (dUnique))
//
//	Размер данных для кодирования
#define	encodingDataSize(e) 	(commonDataLen(e) + (e) * 2 + 1)
//
//	Смещения данных для декокодирования
#define	degLoc1	(* reinterpret_cast<int16_t *>(pGlobal + dUnique))	// степень полинома eLoc1
#define	degLoc2	(* reinterpret_cast<int16_t *>(pGlobal + dUnique + sizeof(int16_t)))	// степень полинома eLoc2
#define	degEuqP1	(* reinterpret_cast<int16_t *>(pGlobal + dUnique + 2 * sizeof(int16_t)))	// степень полинома euqP1
#define	degEuqP2	(* reinterpret_cast<int16_t *>(pGlobal + dUnique + 3 * sizeof(int16_t)))	// степень полинома euqP2
//
#define	dArrStart (dUnique + 4 * sizeof(int16_t))
#define	eLoc1	(pGlobal + dArrStart)
#define	eLoc2	(pGlobal + dArrStart + 1 * arraySize)
#define	euqP1	(pGlobal + dArrStart + 2 * arraySize)
#define	euqP2	(pGlobal + dArrStart + 3 * arraySize)
#define	errorLocators	(pGlobal + dArrStart + 4 * arraySize)	// локаторы ошибок
#define	errorValues	(pGlobal + dArrStart + 5 * arraySize)	// значения обшибок
//
//	Размер данных для декодирования
#define	decodingDataSize(e)	(commonDataLen(e) + 4 * sizeof(int16_t) + 6 * ((e) * 2 + 1))

///////////////////////////////////////////////////////////
//
//	Арифметика в поле F256
//	
///////////////////////////////////////////////////////////
//
//	Таблицы логарифмов и антилогарифмов
static const uint8_t PROGMEM logs256[]  = {
	0xff, 0x00, 0x01, 0x19, 0x02, 0x32, 0x1a, 0xc6, 0x03, 0xdf, 0x33, 0xee, 0x1b, 0x68, 0xc7, 0x4b, 
	0x04, 0x64, 0xe0, 0x0e, 0x34, 0x8d, 0xef, 0x81, 0x1c, 0xc1, 0x69, 0xf8, 0xc8, 0x08, 0x4c, 0x71, 
	0x05, 0x8a, 0x65, 0x2f, 0xe1, 0x24, 0x0f, 0x21, 0x35, 0x93, 0x8e, 0xda, 0xf0, 0x12, 0x82, 0x45, 
	0x1d, 0xb5, 0xc2, 0x7d, 0x6a, 0x27, 0xf9, 0xb9, 0xc9, 0x9a, 0x09, 0x78, 0x4d, 0xe4, 0x72, 0xa6, 
	0x06, 0xbf, 0x8b, 0x62, 0x66, 0xdd, 0x30, 0xfd, 0xe2, 0x98, 0x25, 0xb3, 0x10, 0x91, 0x22, 0x88, 
	0x36, 0xd0, 0x94, 0xce, 0x8f, 0x96, 0xdb, 0xbd, 0xf1, 0xd2, 0x13, 0x5c, 0x83, 0x38, 0x46, 0x40, 
	0x1e, 0x42, 0xb6, 0xa3, 0xc3, 0x48, 0x7e, 0x6e, 0x6b, 0x3a, 0x28, 0x54, 0xfa, 0x85, 0xba, 0x3d, 
	0xca, 0x5e, 0x9b, 0x9f, 0x0a, 0x15, 0x79, 0x2b, 0x4e, 0xd4, 0xe5, 0xac, 0x73, 0xf3, 0xa7, 0x57, 
	0x07, 0x70, 0xc0, 0xf7, 0x8c, 0x80, 0x63, 0x0d, 0x67, 0x4a, 0xde, 0xed, 0x31, 0xc5, 0xfe, 0x18, 
	0xe3, 0xa5, 0x99, 0x77, 0x26, 0xb8, 0xb4, 0x7c, 0x11, 0x44, 0x92, 0xd9, 0x23, 0x20, 0x89, 0x2e, 
	0x37, 0x3f, 0xd1, 0x5b, 0x95, 0xbc, 0xcf, 0xcd, 0x90, 0x87, 0x97, 0xb2, 0xdc, 0xfc, 0xbe, 0x61, 
	0xf2, 0x56, 0xd3, 0xab, 0x14, 0x2a, 0x5d, 0x9e, 0x84, 0x3c, 0x39, 0x53, 0x47, 0x6d, 0x41, 0xa2, 
	0x1f, 0x2d, 0x43, 0xd8, 0xb7, 0x7b, 0xa4, 0x76, 0xc4, 0x17, 0x49, 0xec, 0x7f, 0x0c, 0x6f, 0xf6, 
	0x6c, 0xa1, 0x3b, 0x52, 0x29, 0x9d, 0x55, 0xaa, 0xfb, 0x60, 0x86, 0xb1, 0xbb, 0xcc, 0x3e, 0x5a, 
	0xcb, 0x59, 0x5f, 0xb0, 0x9c, 0xa9, 0xa0, 0x51, 0x0b, 0xf5, 0x16, 0xeb, 0x7a, 0x75, 0x2c, 0xd7, 
	0x4f, 0xae, 0xd5, 0xe9, 0xe6, 0xe7, 0xad, 0xe8, 0x74, 0xd6, 0xf4, 0xea, 0xa8, 0x50, 0x58, 0xaf 
};
static const uint8_t PROGMEM alogs256[] = {
	0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1d, 0x3a, 0x74, 0xe8, 0xcd, 0x87, 0x13, 0x26, 
	0x4c, 0x98, 0x2d, 0x5a, 0xb4, 0x75, 0xea, 0xc9, 0x8f, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 
	0x9d, 0x27, 0x4e, 0x9c, 0x25, 0x4a, 0x94, 0x35, 0x6a, 0xd4, 0xb5, 0x77, 0xee, 0xc1, 0x9f, 0x23, 
	0x46, 0x8c, 0x05, 0x0a, 0x14, 0x28, 0x50, 0xa0, 0x5d, 0xba, 0x69, 0xd2, 0xb9, 0x6f, 0xde, 0xa1, 
	0x5f, 0xbe, 0x61, 0xc2, 0x99, 0x2f, 0x5e, 0xbc, 0x65, 0xca, 0x89, 0x0f, 0x1e, 0x3c, 0x78, 0xf0, 
	0xfd, 0xe7, 0xd3, 0xbb, 0x6b, 0xd6, 0xb1, 0x7f, 0xfe, 0xe1, 0xdf, 0xa3, 0x5b, 0xb6, 0x71, 0xe2, 
	0xd9, 0xaf, 0x43, 0x86, 0x11, 0x22, 0x44, 0x88, 0x0d, 0x1a, 0x34, 0x68, 0xd0, 0xbd, 0x67, 0xce, 
	0x81, 0x1f, 0x3e, 0x7c, 0xf8, 0xed, 0xc7, 0x93, 0x3b, 0x76, 0xec, 0xc5, 0x97, 0x33, 0x66, 0xcc, 
	0x85, 0x17, 0x2e, 0x5c, 0xb8, 0x6d, 0xda, 0xa9, 0x4f, 0x9e, 0x21, 0x42, 0x84, 0x15, 0x2a, 0x54, 
	0xa8, 0x4d, 0x9a, 0x29, 0x52, 0xa4, 0x55, 0xaa, 0x49, 0x92, 0x39, 0x72, 0xe4, 0xd5, 0xb7, 0x73, 
	0xe6, 0xd1, 0xbf, 0x63, 0xc6, 0x91, 0x3f, 0x7e, 0xfc, 0xe5, 0xd7, 0xb3, 0x7b, 0xf6, 0xf1, 0xff, 
	0xe3, 0xdb, 0xab, 0x4b, 0x96, 0x31, 0x62, 0xc4, 0x95, 0x37, 0x6e, 0xdc, 0xa5, 0x57, 0xae, 0x41, 
	0x82, 0x19, 0x32, 0x64, 0xc8, 0x8d, 0x07, 0x0e, 0x1c, 0x38, 0x70, 0xe0, 0xdd, 0xa7, 0x53, 0xa6, 
	0x51, 0xa2, 0x59, 0xb2, 0x79, 0xf2, 0xf9, 0xef, 0xc3, 0x9b, 0x2b, 0x56, 0xac, 0x45, 0x8a, 0x09, 
	0x12, 0x24, 0x48, 0x90, 0x3d, 0x7a, 0xf4, 0xf5, 0xf7, 0xf3, 0xfb, 0xeb, 0xcb, 0x8b, 0x0b, 0x16, 
	0x2c, 0x58, 0xb0, 0x7d, 0xfa, 0xe9, 0xcf, 0x83, 0x1b, 0x36, 0x6c, 0xd8, 0xad, 0x47, 0x8e, 0x01
};
//
//	Арифметические операции в поле GF256
//	
static inline uint8_t log_F256(const uint8_t x) { return pgm_read_byte_near(logs256 + x); }
static inline uint8_t alog_F256(const uint8_t x) { return pgm_read_byte_near(alogs256 + x); }
static inline uint8_t modulo255(const uint16_t x) { return x >= 255 ? x - 255 : x; }
static inline uint8_t mulDeg_F256(const uint16_t x, const uint16_t k) { return x == 0 ? 0 : alog_F256(modulo255(log_F256(x) + k)); }
static inline uint8_t reverse_F256(const uint8_t x) { return alog_F256(255 - log_F256(x)); }
static inline uint8_t mul_F256(const uint8_t a, const uint8_t b) { 
	return a == 0 || b==0 ? 0 : alog_F256(modulo255(static_cast<uint16_t>(log_F256(a)) + log_F256(b))); 
}

///////////////////////////////////////////////////////////
//
//	Запрос памяти
//	
///////////////////////////////////////////////////////////
static inline uint8_t * getMemory(const size_t amount) {
	return reinterpret_cast<uint8_t *>(calloc(amount, sizeof(uint8_t)));	
}

///////////////////////////////////////////////////////////
//
//	Работа с полиномами
//	
///////////////////////////////////////////////////////////
//
//	Обнулить полином
static inline void clearPolynom(uint8_t * p, const uint8_t maxDegree) { memset(p, 0, maxDegree + 1); }
//
//	Вернуть спепень полинома (-1 если он пустой)
static inline int16_t polynomDegree(const uint8_t * p, const int maxDegree) { 
	for (int i = maxDegree; i >= 0; i--) if (p[i]) return i;
	return -1;
}
//
//
//	Меняем полиномы местами, если степень первого меньше степени второго
//
#define swapNumbers(a,b)	do { (a) ^= (b); (b) ^= (a); (a) ^= (b); } while (false)
static void swapPolynomes(void) {
	if (degEuqP1 < degEuqP2) {
		for (uint8_t i = 0; i <= doubledErrorsAmount; i++) {
			swapNumbers(euqP1[i], euqP2[i]);
			swapNumbers(eLoc1[i], eLoc2[i]);
		};
		swapNumbers(degEuqP1, degEuqP2);
		swapNumbers(degLoc1, degLoc2);
	}
}


///////////////////////////////////////////////////////////
//
//	Общая подготовка к кодированию и декодированию
//	
///////////////////////////////////////////////////////////
//
//	Подготовка к собственно операциям кодировани - декодирования
//	Возвращает 
//		RSE_BUFFER_TOO_SMALL
//		RSE_LARGE_AMOUNT_OF_ERRORS
//		RSE_NO_ERROR
//
static RS_RESULTS setupValues(const uint8_t amountOfErrors, const uint8_t bufsize) {
	if (bufsize < 3) return RSE_BUFFER_TOO_SMALL;
	bufferLength = bufsize;
	if (2 * (amountOfErrors + 1) >= bufsize) return RSE_LARGE_AMOUNT_OF_ERRORS;
	if (amountOfErrors > ErrorMax) return RSE_LARGE_AMOUNT_OF_ERRORS;
	errorsAmount = amountOfErrors;
	arraySize = amountOfErrors * 2 + 1;
	doubledErrorsAmount = errorsAmount * 2;
	return RSE_NO_ERROR;
}


///////////////////////////////////////////////////////////
//
//	Декодирование
//	
///////////////////////////////////////////////////////////
//
//	Один шаг алгоритма Евклида
//
static inline void EuclidAlgorithmStep(void) {
	for (int16_t i = 0; i <= degEuqP2; i++) {
		euqP1[i + degEuqP1 - degEuqP2] ^= euqP2[i];
		eLoc1[i + degEuqP1 - degEuqP2] ^= eLoc2[i];
	}
	degEuqP1 = polynomDegree(euqP1, doubledErrorsAmount);
	degLoc1 = polynomDegree(eLoc1, doubledErrorsAmount);
	const uint8_t reveresed = reverse_F256(euqP1[degEuqP1]);
	for (int16_t i = 0; i <= degEuqP1; i++) euqP1[i] = mul_F256(euqP1[i], reveresed);
	for (int16_t i = 0; i <= degLoc1; i++) eLoc1[i] = mul_F256(eLoc1[i], reveresed);
	swapPolynomes();
}
//
//	Заполняем "синдром"
//
static inline void fillSyndrome(void) {
	for (uint8_t i = 0; i < bufferLength; i++) {
		const uint8_t bCont = pBuffer[i];
		if (! bCont) continue;
		syndrome[0] ^= bCont;
		uint16_t syn = log_F256(bCont);
		for (uint8_t j = 1; j < doubledErrorsAmount; j++) {
			syn += i;
			if (syn > 254) syn -= 255;
			syndrome[j] ^= alog_F256(syn);
		}
	}
}
//
//	Подготовка данных для декодирования
//
static inline bool initDecode(void) {
	fillSyndrome();
	const int16_t degree1 = polynomDegree(syndrome, doubledErrorsAmount - 1);
	if (degree1 < 0) return true;
	const int16_t degree2 = polynomDegree(syndrome, degree1 - 1);
	if (degree2 < 0) return true;
	//
	degLoc1 = doubledErrorsAmount - degree1;
	degEuqP1 = degree2 + degLoc1;
	uint8_t content = eLoc1[degLoc1] = reverse_F256(syndrome[degree2]);
	for (int16_t i = 0; i <= degree2; i++) euqP1[i + degLoc1] = mul_F256(syndrome[i], content);
	degLoc2 = 0;
	degEuqP2 = degree1;
	content = eLoc2[0] = reverse_F256(syndrome[degree1]);
	for (int16_t i = 0; i <= degree1; i++) euqP2[i] = mul_F256(syndrome[i], content);
	swapPolynomes();
	return false;
}
//
//	Поиск корней
//	возвращает true, если корней слишком мало
//
static inline bool rootsDecode(void) {
	while ((degEuqP2 >= errorsAmount) && (degLoc2 <= errorsAmount)) EuclidAlgorithmStep();
	if (!(errorsAmount >= degLoc2 && degLoc2 > degEuqP2)) return -1;
	if (!eLoc2[0]) return -1;
	uint8_t content = reverse_F256(eLoc2[0]);
	for (int16_t i = 0; i <= degLoc2; i++) eLoc2[i] = mul_F256(eLoc2[i], content);
	for (int16_t i = 0; i <= degEuqP2; i++) euqP2[i] = mul_F256(euqP2[i], content);
	for (int16_t i = 0, j = 0; j < bufferLength; j++) {
		content = eLoc2[0]; 
		for (int16_t k = 1; k <= degLoc2; k++) content = mulDeg_F256(content, j) ^ eLoc2[k];
		if (!content) {
			errorLocators[i++] = j;
			if (i >= degLoc2) return false; 
		}
	}
	return true;
}
//
//	Собственно декодирование
//
static inline uint8_t theDecoding(void) {
	for (int16_t i = 0; i < degLoc2; i++) {
		uint16_t j = 2 * (255 - errorLocators[i]);
		if (j > 255) j -= 255;
		uint8_t content1 = euqP2[degEuqP2];
		for (int16_t k = degEuqP2 - 1; k >= 0; k--) content1 = mulDeg_F256(content1, 255 - errorLocators[i]) ^ euqP2[k];
		uint8_t content2 = eLoc2[(degLoc2 & (-2)) + 1];
		for (int16_t k = (degLoc2 & (-2)) - 1; k >= 0; k -= 2) content2 = mulDeg_F256(content2, j) ^ eLoc2[k];
		if (!content2) return -1;
		errorValues[i] = mul_F256(content1, reverse_F256(content2));
		errorValues[i] = mulDeg_F256(errorValues[i], errorLocators[i]);
		pBuffer[errorLocators[i]] ^= errorValues[i];
	}
	return degLoc2;
}
//
//	Полный цикл декодирования
//
int rsDecode(void * buf, const uint8_t amountOfErrors, const uint8_t bufsize) {
	pGlobal = getMemory(decodingDataSize(amountOfErrors));
	if (! pGlobal) return static_cast<int>(RSE_NOT_ENOUGH_MEMORY);
	RS_RESULTS res = setupValues(amountOfErrors, bufsize);
	if (res != RSE_NO_ERROR) {
		free(pGlobal);
		return static_cast<int>(res);
	}
	pBuffer = reinterpret_cast<uint8_t *>(buf);
	int gotErrors = 0;
	const bool initResult = initDecode();
	if (! initResult) {
		const bool rootsResult = rootsDecode();
		gotErrors = rootsResult ? static_cast<int>(RSE_TOO_MANY_ERRORS) : theDecoding();
	}
	free(pGlobal);
	return gotErrors;
}



///////////////////////////////////////////////////////////
//
//	Кодирование
//	
///////////////////////////////////////////////////////////
//	Вычисляем полином вида
//		x^o2 + g[o2-1]*x^(o-1) + ... + g[1]*x + g[0]
//	где o2 - удвоенное количество допустимых ошибок
static inline void createMainPolynome(void) {
	clearPolynom(mainPolynome, doubledErrorsAmount);
	mainPolynome[0] = mainPolynome[1] = 1;
	for (uint8_t i = 2; i <= doubledErrorsAmount; i++) {
		for (uint8_t j = i; j > 0; j--) 
			mainPolynome[j] = mainPolynome[j - 1] ^ mulDeg_F256(mainPolynome[j], i - 1);
		mainPolynome[0] = mulDeg_F256(mainPolynome[0], i - 1);
	}
}
//
//	Собственно кодирование
//
RS_RESULTS rsEncode(void *buf, const uint8_t amountOfErrors, const uint8_t bufsize) {
	pGlobal = getMemory(encodingDataSize(amountOfErrors));
	if (! pGlobal) return RSE_NOT_ENOUGH_MEMORY;
	RS_RESULTS res = setupValues(amountOfErrors, bufsize);
	if (res != RSE_NO_ERROR) {
		free(pGlobal);
		return res;
	}
	pBuffer = reinterpret_cast<uint8_t *>(buf);
	createMainPolynome();
	memset(pBuffer, 0, doubledErrorsAmount);
	for (int i = bufferLength - 1; i >= 0; i--) {
		const uint8_t bCont = syndrome[doubledErrorsAmount - 1];
		for (uint8_t j = doubledErrorsAmount - 1; j > 0; j--)
			syndrome[j] = syndrome[j - 1] ^ mul_F256(mainPolynome[j], bCont);
		syndrome[0] = pBuffer[i] ^ mul_F256(mainPolynome[0], bCont);
	}
	memcpy(pBuffer, syndrome, doubledErrorsAmount);
	free(pGlobal);
	return RSE_NO_ERROR;
}

}	// namespace ReedSolomonEncoding
