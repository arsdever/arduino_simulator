//#include <arduino_uno>
//#include <receiver>
#include <cpu>
#include <ram>
#include <QApplication>
#include <QFile>
#include "window.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	CWindow widget;
	widget.show();
	return a.exec();
}
