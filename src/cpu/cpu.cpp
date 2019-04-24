//----------------------------------------------------------------------------------------------------------------------
// References
// AVR instruction set		- http://ww1.microchip.com/downloads/en/devicedoc/atmel-0856-avr-instruction-set-manual.pdf
// AVR instruction opcodes	- http://lyons42.com/AVR/Opcodes/AVRAllOpcodes.html
//----------------------------------------------------------------------------------------------------------------------

#include "cpu.h"

#include "io_controller.h"
#include <core>
#include <ram>

#include <QString>
#include <QUuid>

#include <sstream>

#define TICK_DURATION 600

CCPU::CCPU()
	: __uuid("CPU-" + QUuid::createUuid().toString())
	, __tick_count(0)
	, __last_inst_tick_count(0)
	, __inst_duration(0)
	, __creation_time(std::chrono::high_resolution_clock::now())
{
}

QString CCPU::GetUUID() const
{
	return __uuid;
}

void CCPU::XCPU::Step()
{
	__this->Step();
}

void CCPU::XCPU::Run()
{
	__this->Run();
}

void CCPU::Run()
{
	while (GetState()->IsRunning())
	{
		Step();
	}
}

void CCPU::Step()
{
	Fetch();
	Decode();
	SetInstrucitonDuration(Execute());
}

void CCPU::Tick()
{
	CallFunction<vm::ILogger>(vm::ILogger::InfoFunctor(QString("Tick : %1").arg(__tick_count)));
	++__tick_count;
	//if (__tick_count - __last_inst_tick_count >= __inst_duration)
		Step();
}

void CCPU::SetInstrucitonDuration(quint8 ticks)
{
	__inst_duration = ticks;
}

quint8 CCPU::GetInstrucitonDuration() const
{
	return __inst_duration;
}
