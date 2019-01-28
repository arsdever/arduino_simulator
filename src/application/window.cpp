#include "window.h"

#include <cpu>
#include <ram>
#include "../cpu/avr_base.h"
#include <QFile>
#include <QElapsedTimer>
#include <windows.h>

CWindow::CWindow(QWidget* parent)
	: QLabel(parent)
	, __cpu(new CAVRBase(0x8000, 2048, 1024, 160))
	, __logger("./overall.log")
	, __timer(new CTimer(__cpu))
{
	SetThreadPriority(__timer_thread->native_handle(), 15);
	QFile memory_model("./memory");
	if (!memory_model.open(QIODevice::ReadOnly))
		return;

	while (!memory_model.atEnd())
	{
		memory_model.read(1);
		bool ok = false;
		quint8 size = memory_model.read(2).toShort(nullptr, 16);
		quint8 addresshi = memory_model.read(2).toShort(nullptr, 16);
		quint8 addresslo = memory_model.read(2).toShort(nullptr, 16);
		quint16 address = addresshi << 8 | addresslo;

		quint8 record_type = memory_model.read(2).toShort(nullptr, 16);

		if (record_type)
			break;

		for (int i = 0; i < size; ++i)
		{
			__cpu->GetRam()->operator[]<quint8>(address + i) = memory_model.read(2).toShort(&ok, 16);
		}
		memory_model.read(4);
	}

	__cpu->Init();
	connect(__cpu, SIGNAL(IOChanged(quint32)), this, SLOT(UpdateInfo()));
	__timer_thread = new std::thread(&CTimer::run, __timer);
	__timer_thread->detach();
}

CWindow::~CWindow()
{
	delete __timer_thread;
	delete __timer;
}

void CWindow::UpdateInfo()
{
	setText(tr("%1").arg(__cpu->GetIOPorts()[5], 20, 2, QChar(' ')));
}