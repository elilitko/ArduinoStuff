#include <arduino.h>
#include <string.h>
#include "brainfuck.h"

#define	PINS_MEMORY_SIZE	12
#define	DB_MEMORY_SIZE	(DB_MEMORY + PINS_MEMORY_SIZE)

BFResult BrainFuck(char * bfProgram) {
	static char bfMemory[DB_MEMORY_SIZE];
	int memPtr = PINS_MEMORY_SIZE;
	int loopCounter = 0;
   //
   // Инициализация памяти
	memset(bfMemory + PINS_MEMORY_SIZE, 0, DB_MEMORY);
	for (int8_t i = 2; i < 14; i++) bfMemory[PINS_MEMORY_SIZE + 1 - i] = digitalRead(i);
   //
   // удаление из строки-программы всех символов кроме <>+-.,[]
	static const char * bfCommands = "<>+-.,[]";
	char *p = bfProgram;
	for (int ncmd = strspn(p, bfCommands); ncmd < (int)strlen(p); ncmd = strspn(p, bfCommands)) {
		p += ncmd;
		strcpy(p, p + strcspn(p, bfCommands));
	}
   //
   // Собственно цикл интерпретатора
	const int programSize = strlen(bfProgram);
	for (int progPtr = 0; progPtr < programSize; progPtr++) {
		switch (bfProgram[progPtr]) {
			case '>':
				memPtr++;
				break;
			case '<':
				memPtr--;
				break;
			case '+':
				bfMemory[memPtr]++;
				break;
			case '-':
				bfMemory[memPtr]--;
				break;
			case '.':
				if (memPtr < PINS_MEMORY_SIZE) digitalWrite(PINS_MEMORY_SIZE + 1 - memPtr, bfMemory[memPtr]);
				else Serial.print(bfMemory[memPtr]);
				break;
			case ',':
				if (memPtr < PINS_MEMORY_SIZE) bfMemory[memPtr] = digitalRead(PINS_MEMORY_SIZE + 1 - memPtr);
				else bfMemory[memPtr] = Serial.read();
				break;
			case '[':
				if (!bfMemory[memPtr]) {
					loopCounter++;
					while (loopCounter) {
						if (++progPtr >= programSize) return BF_PROGRAM_EXHAUSTED;
						if (bfProgram[progPtr] == '[') loopCounter++;
						else if (bfProgram[progPtr] == ']') loopCounter--;
					}
				}
				break;
			case ']':
				if (bfMemory[memPtr]) {
					loopCounter++;
					while (loopCounter) {
						if (--progPtr < 0) return BF_PROGRAM_UNDERFLOW;
						if (bfProgram[progPtr] == '[') loopCounter--;
						else if (bfProgram[progPtr] == ']') loopCounter++;
					}
					if (--progPtr < -1) return BF_PROGRAM_UNDERFLOW;
				}
				break;
		}
		if (memPtr < 0) return BF_MEMORY_UNDERFLOW;
		if (memPtr >= DB_MEMORY_SIZE) return BF_MEMORY_EXHAUSTED;
	}
	return BF_SUCCESS;
}
