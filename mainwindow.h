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
    void connectedSlot(plotterStatus status);
    void disconnectedSlot();
    void sendGCode(QString gCode);
    void openSlot();
    void penChangeSlot();
    void zChangeSlot();
    void fChangeSlot();
    void executeSlot();
    void updateViewSlot();

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
