#pragma once

#include "core_global.h"
#include <QObject>
#include <QMap>
#include <QSet>

namespace vm
{
	class IUnknown;
}

class CORE_EXPORT CCore : public QObject
{
public:
	CCore(QObject* pParent = nullptr);

	void RegisterInterface(vm::IUnknown* pInterface);

	template <typename T>
	QSet<T*>  QueryInterface();

	QSet<vm::IUnknown*> QueryInterface(QString const& strInterfaceUUID);

private:
	static QMap<QString, QSet<vm::IUnknown*>> s_mapPlugins;
};

#include "core_impl.h"

extern "C" CORE_EXPORT CCore* GetCore();