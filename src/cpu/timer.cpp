#include "timer.h"

#include <cpu>
#include <QElapsedTimer>

CTimer::CTimer(CCPU* cpu)
	: __cpu(cpu)
{}

void CTimer::run()
{
	QElapsedTimer timer_core;
	timer_core.start();
	while (1)
	{
		if (timer_core.nsecsElapsed() >= 62)
		{
			++__cpu->GetIOPorts()[0x26];
			__cpu->Step();
			timer_core.restart();
		}
	}
}