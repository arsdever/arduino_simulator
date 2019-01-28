//== CPU Model base class ======================================================
// This class declares the interface for CPU type objects
// 
// Arsen Gharagyozyan © 2019
//==============================================================================

#pragma once

#include "cpu_global.h"

#include <exception>
#include <core>

#include <QObject>
#include <QMap>

class CRAM;

class CPU_EXPORT CCPU : public QObject
{

	Q_OBJECT

	IMPLEMENT_BEGIN(CCPU, CPU)
		void Step() override;
		void Run() override;
	IMPLEMENT_END(CPU)

public:
	struct SState
	{
		virtual bool GetFlag(quint32) const = 0;
		virtual void SetFlag(quint32, bool) = 0;
		
		virtual quint32 GetFlagRegister() const = 0;
		virtual void SetFlagRegister(quint32) = 0;

		virtual quint8* GetGeneralRegisterSet() const = 0;
		virtual quint8 GetGeneralRegisterCount() const = 0;
		virtual quint8* GetAddressRegisterSet() const = 0;
		virtual quint8 GetAddressRegisterCount() const = 0;

		virtual quint32& ProgramCounter() const = 0;
		virtual quint8* InstructionRegister() const = 0;

		virtual bool IsRunning() const = 0;
	};


public:
	CCPU();

	virtual CRAM* GetRam() const = 0;
	virtual void SetRam(CRAM*) = 0;

	virtual quint8* GetIOPorts() const = 0;

	virtual SState* GetState() const = 0;

	QString GetUUID() const;

	virtual void Run();
	virtual void Step();
	virtual void Tick();

	virtual void Init() = 0;

protected:
	virtual void Fetch() = 0;
	virtual quint8 Decode(quint16) = 0;
	virtual quint8 Decode() = 0;
	virtual quint8 Execute() = 0;

	void SetInstrucitonDuration(quint8 ticks);
	quint8 GetInstrucitonDuration() const;

private:
	QString __uuid;
	quint8 __inst_duration;
	quint64 __last_inst_tick_count;
	quint64 __tick_count;

signals:
	void IOChanged(quint32);

public:
	struct cpu_exception : public std::exception {};
	struct breakpoint_exception : cpu_exception {};
	struct ram_not_found_exception : cpu_exception {};
	struct invalid_register_exception : cpu_exception {};
	struct invalid_function_call : cpu_exception {};
};