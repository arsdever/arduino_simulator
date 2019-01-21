#pragma once

#include <QWidget>
#include <QLabel>
#include <thread>

class CCPU;
class CTimer;

class CWindow : public QLabel
{

	Q_OBJECT

public:
	CWindow(QWidget* parent = nullptr);
	~CWindow();

public slots:
	void UpdateInfo();

private:
	CCPU* __cpu;
	CTimer* __timer;
	std::thread __timer_thread;
};
