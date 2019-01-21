#pragma once

#include "cpu_global.h"

class CCPU;

class CPU_EXPORT CTimer
{
public:
	CTimer(CCPU* cpu);
	void run();

private:
	CCPU* __cpu;
};