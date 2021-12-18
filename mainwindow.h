#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QFileDialog>
#include "usbconnection.h"
#include "connectionindicator.h"
#include "gerberconverter.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected slots:
    void connectSlot();
    void statusSlot(plotterStatus status);
    void disconnectedSlot();
    void sendGCode(QString gCode);
    void sendProgramm(QStringList *program);
    void openSlot();
    void penChangeSlot();
    void zChangeSlot();
    void fChangeSlot();
    void executeSlot();
    void updateViewSlot();
    void errorSlot(QString error, QString title = NULL);
    void stopSlot();
    void pauseSlot();
    void messageSlot(QString message);

protected:
    usbConnection *USBPort;
    connectionIndicator *conIndicator;
    gerberConverter *Gconverter;
    QLabel *portName;
    QVector<float>penDiametersArray;

    void distribForms();


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
