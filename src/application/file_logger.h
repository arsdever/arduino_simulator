#pragma once

#include <core>
#include <exception>
#include <QFile>

class CFileLogger
{

	IMPLEMENT_BEGIN(CFileLogger, Logger)
		void Info(QString const&) override;
		void Warning(QString const&) override;
		void Error(QString const&) override;
	IMPLEMENT_END(Logger)

public:
	CFileLogger(QString const& path);
	~CFileLogger();

private:
	QFile __file;

public:
	struct logger_exception : public std::exception {};
	struct file_exception : logger_exception {};
};