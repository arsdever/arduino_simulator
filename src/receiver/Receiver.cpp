#include "Receiver.h"

#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>

#include <QDrag>
#include <QMimeData>

Receiver::Receiver(QWidget* parent)
	: QWidget(parent)
	, __server(new QTcpServer(this))
{
	if(!__server->listen(QHostAddress::LocalHost, 8080))
		return;

	qDebug() << "Server IP: " << __server->serverAddress().toString();
	qDebug() << "Port: " << __server->serverPort();

	connect(__server, SIGNAL(newConnection()), this, SLOT(on_new_connection()));
}

void Receiver::on_new_connection()
{
	qDebug() << "new connection";
	__socket = __server->nextPendingConnection();
	connect(__socket, SIGNAL(readyRead()), this, SLOT(read_data()));
}

void Receiver::read_data()
{
	qDebug() << __socket->readAll();
}

Receiver::~Receiver()
{
}

void Receiver::mousePressEvent(QMouseEvent* event)
{
	QDrag* drag = new QDrag(this);
	QMimeData* mime_data = new QMimeData();
	mime_data->setData("type", "connection");
	mime_data->setData("ip_address", __server->serverAddress().toString().toLatin1());
	mime_data->setData("port", QString::number(__server->serverPort()).toLatin1());
	drag->setMimeData(mime_data);
	Qt::DropAction act = drag->exec();
}