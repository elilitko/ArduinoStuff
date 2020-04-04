#include <Printing.h>
#define USE_PINOPS	true
#include <ESP_PSRAM64.h>

class CAnekdot : public Printable {
	char m_origin[16];
	char m_body[100];
	int m_id;

public:
	CAnekdot(void) {}
	CAnekdot(const int id, const char * origin, const char * body) {
		strcpy(m_origin, origin);
		strcpy(m_body, body);
		m_id = id;
	}
	size_t printTo(Print& p) const {
		return 
			p.print(m_id) 
			+ p.print(", (")
			+ p.print(m_origin)
			+ p.println(')')
			+ p.println(m_body);
	}
};

ESP_PSRAM64 psram;

void setup(void) {
	Serial.begin(115200);
	Serial.println("ESP PSRAM64H тест методов put и get\r\n");
	delayMicroseconds(150);
	psram.reset();

	//
	// Число с плавающей точкой
	float pi = M_PI;
	Serial << "Исходное число (float): " << pi << "\r\n";
	psram.put(1234, pi); // запишем
	pi = 0; // загадим
	psram.get(1234, pi); // прочитаем
	Serial << "Прочитанное число (float): " << pi << "\r\n\r\n";

	//
	// Число uint32_t
	uint32_t f_cpu = F_CPU;
	Serial << "Исходное число (uint32_t): " << f_cpu << "\r\n";
	psram.put(1234, f_cpu); // запишем
	f_cpu = 0; // загадим
	psram.get(1234, f_cpu); // прочитаем
	Serial << "Прочитанное число (uint32_t): " << f_cpu << "\r\n\r\n";

	//
	// Массив char[]
	char str[] = "\"Жил один еврей, так он сказал, что всё проходит.\"";
	Serial << "Исходный массив: " << str << "\r\n";
	psram.put(1234, str); // запишем
	str[0] = '\0'; // загадим
	psram.get(1234, str); // прочитаем
	Serial << "Прочитанный массив: " << str << "\r\n\r\n";

	// Исходная структура (запишем её по адресу 1234)
	CAnekdot source(321, "Одесса", "- Сёма, Вы таки куда?\r\n- Да, нет, я домой.");
	psram.put(1234, source);
	Serial.println("::: Исходная структура :::");
	Serial.println(source);

	// Прочитанная из PSRAM структура
	CAnekdot dest;
	psram.get(1234, dest);
	Serial.println("::: Прочитанная структура :::");
	Serial.println(dest);

	Serial.println("=== Тест завершён ===");
}

void loop(void) {}
