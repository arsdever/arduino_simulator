#pragma once

#include "arduino_uno_global.h"
#include <QWidget>

class QTcpSocket;
class QLineEdit;
class QPushButton;
class CPin;

class CPin : public QWidget
{
public:
	CPin(QWidget* parent = nullptr);

	void connection(QString ip, quint16 port);
	
protected:
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;

private slots:
	void on_connect();

private:
	bool __is_connected;
	QDataStream __stream;
	QTcpSocket* __socket;
	CPin* __pin;
};


class ARDUINO_UNO_EXPORT ArduinoSimulator : public QWidget
{
	Q_OBJECT

public:
	ArduinoSimulator(QWidget *parent = Q_NULLPTR);

private:
	QPushButton* __connect;
	QLineEdit* __ip;
	QLineEdit* __port;

protected:
	virtual void dragEnterEvent(QDragEnterEvent* event) override;
	virtual void dropEvent(QDropEvent* event) override;
};
