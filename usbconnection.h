#ifndef USBCONNECTION_H
#define USBCONNECTION_H

#include <QWidget>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QByteArray>
#include <QFile>
#include <QApplication>
#include "widgets/serialportsdialog.h"
#include "drawcommand.h"


enum messageType{
    MESSAGE_STATUS,
    MESSAGE_G_CODE,
    MESSAGE_ACKNOWLEDGE
};

struct plotterStatus{
    float posX;
    float posY;
    float posZ;
};


class usbConnection : public QObject
{
    Q_OBJECT

public:
    usbConnection(QWidget *parent);
    ~usbConnection();
    bool selectPort();
    void connectPort();
    void disconnectPort();
    QString getCurrentPortName();
    void sendGCode(QString gCode);
    void writeArray(QByteArray *array);
    bool isConnected();
    void runProgram(QStringList *program);
    void stopProgram();
    void pauseProgram();

signals:
    void connectedSignal(plotterStatus status);
    void disconnectedSignal();

protected:
    QWidget *parentWidget;
    QSerialPort *currentPort;
    QTimer statusTimer;
    QTimer waitTimer;
    QByteArray inputBuffer;
    QStringList *programPointer;
    bool runing;
    int programCounter;

    void deleteCurrentPort();
    void message(QString text);
    quint16 Crc16(char *pcBlock, unsigned short len);
    void decodePacket();
    bool openPortFromFile();
    bool writeCurrentPortInFile();
    void sendNextComand();

protected slots:
    void portError(QSerialPort::SerialPortError error);
    void requestTime();
    void dataReadyRead();
    void waitTimeSlot();

};

#endif // USBCONNECTION_H
