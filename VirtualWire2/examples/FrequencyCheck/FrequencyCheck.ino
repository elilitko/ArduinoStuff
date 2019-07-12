// FrequencyCheck.ino
//
// Служебный скетч для проверки частоты таймера и параметров таймера
//
// Если в файле VirtualWire2_Config.h назначить пин FREQUENCY_CHECK_PIN
// (будет сконфигурирован на OUTPUT в vw_setup)
// То при работе данного скетча на FREQUENCY_CHECK_PIN можно замерять частоту
// 
// Если в файле VirtualWire2_Config.h назначить 
//		#define	REPORT_TIMER_PARAMETERS	Serial
// То при работе данного скетча в мониторе порта можно будет увидет параметры таймера
// (делитель частоты и количество тиков)
// 
#include <VirtualWire2.h>

void setup(void) {
	Serial.begin(115200);
	vw_setup(2000);	// Здесь нужно задать скорость (бит/сек)
}

void loop(void) {}
