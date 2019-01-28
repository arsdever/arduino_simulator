#include "timer.h"

#include <cpu>
#include <chrono>
#include <thread>

CTimer::CTimer(CCPU* cpu)
	: __cpu(cpu)
{}

void CTimer::run()
{
	while (1)
	{
		__cpu->Tick();
		std::this_thread::sleep_for(std::chrono::microseconds(1));
		++__cpu->GetIOPorts()[0x26];
	}
}