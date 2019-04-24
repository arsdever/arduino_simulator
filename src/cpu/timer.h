#pragma once

#include "cpu_global.h"
#include <QThread>

class CCPU;

class CPU_EXPORT CTimer: public QThread
{
	
	Q_OBJECT

public:
	CTimer(CCPU *cpu);

	uint64_t getTickCount() const { return __tick_count; }
	uint64_t msecElapsed() const { return __msec_elapsed; }
	uint64_t secElapsed() const { return __sec_elapsed; }

protected:
	void run();

signals:
	void tick();

private:
	CCPU *__cpu;
	uint64_t __tick_count;
	uint64_t __sec_elapsed;
	uint64_t __msec_elapsed;
};
