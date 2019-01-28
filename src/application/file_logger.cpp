#include "file_logger.h"

CFileLogger::CFileLogger(QString const& path)
	: __file(path)
{
	if (!__file.open(QIODevice::WriteOnly))
		throw file_exception();
}

CFileLogger::~CFileLogger()
{
	__file.close();
}

void CFileLogger::XLogger::Info(QString const& msg)
{
	__this->__file.write(QString("[Info] %1\n").arg(msg).toLatin1());
	__this->__file.flush();
}

void CFileLogger::XLogger::Warning(QString const& msg)
{
	__this->__file.write(QString("[Warning] %1\n").arg(msg).toLatin1());
	__this->__file.flush();
}

void CFileLogger::XLogger::Error(QString const& msg)
{
	__this->__file.write(QString("[Error] %1\n").arg(msg).toLatin1());
	__this->__file.flush();
}