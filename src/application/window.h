#pragma once

#include <QMainWindow>
#include "file_logger.h"

class CCPU;
class CTimer;
class CTicker;

class CWindow : public QMainWindow
{

	Q_OBJECT

public:
	CWindow(QWidget* parent = nullptr);
	~CWindow();

public slots:
	void UpdateInfo();
	void updateStatusBar();

private:
	CCPU* __cpu;
	CTimer* __timer;
	CFileLogger __logger;
};
