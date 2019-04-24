//#include <arduino_uno>
//#include <receiver>

#include <QApplication>
#include "window.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	
	CWindow widget;
	widget.show();
	return a.exec();
}
