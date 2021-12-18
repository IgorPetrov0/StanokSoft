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
    MESSAGE_COMPLETE,
    MESSAGE_ACKNOWLEDGE,
    MESSAGE_ERROR
};

struct plotterStatus{
    float posX;
    float posY;
    float posZ;
    bool swX;
    bool swY;
    bool swZ;
};


class usbConnection : public QObject
{
    Q_OBJECT

public:
    usbConnection(QWidget *parent);
    ~usbConnection();
    bool selectPort();
    bool connectPort();
    void disconnectPort();
    QString getCurrentPortName();
    void sendGCode(QString gCode);
    void writeArray(QByteArray *array);
    bool isConnected();
    void runProgram(QStringList *program);
    void stopProgram();
    void pauseProgram();
    void decodeStatus();

signals:
    void statusSignal(plotterStatus status);
    void disconnectedSignal();
    void messageSignal(QString message);

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
