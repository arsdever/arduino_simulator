#pragma once

#include "receiver_global.h"
#include <QWidget>

class QTcpServer;
class QTcpSocket;

class RECEIVER_EXPORT Receiver : public QWidget
{

	Q_OBJECT

public:
	Receiver(QWidget* parent = nullptr);
	~Receiver();

private slots:
	void on_new_connection();
	void read_data();

protected:
	virtual void mousePressEvent(QMouseEvent* event) override;

private:
	QTcpServer* __server;
	QTcpSocket* __socket;
};

