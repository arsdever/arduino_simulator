#include "window.h"

#include <cpu>
#include <QFile>
#include <QElapsedTimer>
#include <windows.h>

CWindow::CWindow(QWidget* parent)
	: QLabel(parent)
	, __cpu(new CCPU(new CRAM(0x7FFF)))
	, __timer(new CTimer(__cpu))
	, __timer_thread(&CTimer::run, __timer)
{
	SetThreadPriority(__timer_thread.native_handle(), 15);
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
			__cpu->Ram()->operator[]<quint8>(address + i) = memory_model.read(2).toShort(&ok, 16);
		}
		memory_model.read(4);
	}

	__cpu->Restart();
	connect(__cpu, SIGNAL(IOChanged()), this, SLOT(UpdateInfo()));
	__timer_thread.detach();
}

CWindow::~CWindow()
{
}

void CWindow::UpdateInfo()
{
	setText(tr("%1").arg(__cpu->m_sState.IOR[5], 20, 2, QChar(' ')));
}