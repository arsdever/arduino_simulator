#include "window.h"

#include <cpu>
#include <ram>
#include "../cpu/avr_base.h"
#include <QFile>
#include <QLabel>
#include <QElapsedTimer>
#include <QStatusBar>
#include <windows.h>

CWindow::CWindow(QWidget* parent)
	: QMainWindow(parent)
	, __cpu(new CAVRBase(0x8000, 2048, 1024, 160))
	, __logger("./overall.log")
	, __timer(new CTimer(__cpu))
{
	QFile memory_model("./memory");
	if (!memory_model.open(QIODevice::ReadOnly))
		return;

	setCentralWidget(new QLabel);
	setStatusBar(new QStatusBar);
	connect(__timer, SIGNAL(tick()), this, SLOT(updateStatusBar()));

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
	__timer->start();
}

CWindow::~CWindow()
{
	delete __timer;
	delete __cpu;
}

void CWindow::UpdateInfo()
{
	QLabel* label = qobject_cast<QLabel *>(centralWidget());
	label->setText(tr("%1").arg(__cpu->GetIOPorts()[5], 20, 2, QChar(' ')));
}

void CWindow::updateStatusBar()
{
	statusBar()->showMessage(QString("Time elapsed : %2ms").arg(__timer->msecElapsed()));
}
