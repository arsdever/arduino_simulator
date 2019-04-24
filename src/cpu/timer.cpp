#include "timer.h"

#include <cpu>
#include <windows.h>

inline uint64_t tickMap(uint64_t requestedTickCount, uint64_t hostFrequency, uint64_t clientFrequency)
{
	return requestedTickCount * clientFrequency / hostFrequency;
}

CTimer::CTimer(CCPU *cpu)
	: __cpu(cpu), __tick_count(0), __sec_elapsed(0), __msec_elapsed(0) {}

void CTimer::run()
{
	LARGE_INTEGER ticks;
	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	while ( 1 )
	{
		__cpu->Tick();
		++__tick_count;
		++__cpu->GetIOPorts()[0x26];
		++__cpu->GetIOPorts()[0xB2];
		++(uint16_t&) __cpu->GetIOPorts()[0x84];
		emit tick();
		if (__tick_count == 1000)
		{
			++__msec_elapsed;

			if (!(__msec_elapsed % 1000))
			{
				++__sec_elapsed;
				QueryPerformanceCounter(&ticks);
				while (ticks.QuadPart / freq.QuadPart > __sec_elapsed)
					QueryPerformanceCounter(&ticks);
			}

			__tick_count = 0;
		}
	}
}
