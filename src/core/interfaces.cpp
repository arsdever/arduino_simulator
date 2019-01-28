#include "interfaces.h"
#include <QUuid>

namespace vm
{
	IUnknown::~IUnknown() {}

	const QString ILogger::UUID = QUuid::createUuid().toString();
	const QString IIODevice::UUID = QUuid::createUuid().toString();
	const QString IDebugger::UUID = QUuid::createUuid().toString();
	const QString IVMInformation::UUID = QUuid::createUuid().toString();
	const QString ICPU::UUID = QUuid::createUuid().toString();
}