#include "arduino_uno.h"

#include <QTcpSocket>
#include <QLineEdit>
#include <QPushButton>
#include <QLayout>
#include <QHostAddress>
#include <QDebug>

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QMimeData>

ArduinoSimulator::ArduinoSimulator(QWidget *parent)
	: QWidget(parent)
	, __connect(new QPushButton("connect"))
{
	setAcceptDrops(true);
	setLayout(new QVBoxLayout);
	layout()->addWidget(__connect);
	layout()->addWidget(__ip);
	layout()->addWidget(__port);
	connect(__connect, SIGNAL(clicked()), this, SLOT(on_connect()));
}

CPin::CPin(QWidget* parent)
	: __socket(new QTcpSocket)
{
	__stream.setDevice(__socket);
}

void CPin::on_connect()
{
	connection(QHostAddress(QHostAddress::LocalHost).toString(), 8080);
}

void CPin::connection(QString ip, quint16 port)
{
	__socket->abort();
	ip = "localhost";

	qDebug() << "connecting";
	__socket->connectToHost(ip, port);

	while (!__socket->waitForConnected(3000))
	{
		qDebug() << "reconnecting";
		__socket->connectToHost(ip, port);
	}

	__socket->write("helloworld");
	__socket->flush();
	__socket->waitForBytesWritten(1000);
	if (__socket->waitForReadyRead(3000))
	{
		qDebug() << __socket->readAll();
	}
}

void CPin::dragEnterEvent(QDragEnterEvent * event)
{
	if (event->mimeData()->data("type") == "connection")
		event->accept();
}

void CPin::dropEvent(QDropEvent * event)
{
	connection(event->mimeData()->data("ip_address"), event->mimeData()->data("port").toUShort());
	event->accept();
}


void ArduinoSimulator::dragEnterEvent(QDragEnterEvent * event)
{
	if (event->mimeData()->data("type") == "connection")
		event->accept();
}

void ArduinoSimulator::dropEvent(QDropEvent * event)
{/*
	connection(event->mimeData()->data("ip_address"), event->mimeData()->data("port").toUShort());*/
	event->accept();
}
