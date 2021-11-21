#ifndef SERIALPORTSDIALOG_H
#define SERIALPORTSDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QSerialPort>
#include <QSerialPortInfo>

namespace Ui {
class serialPortsDialog;
}

class serialPortsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit serialPortsDialog(QWidget *parent);
    ~serialPortsDialog();
    void setPort(QSerialPortInfo port);
    void setPortsList(QList<QSerialPortInfo> *list);
    QSerialPort *getPort();

protected:
    QList<QSerialPortInfo> *currentPortsList;
    QSerialPort *currentPort;
    void fillComboBoxes();

protected slots:
    void saveSlot();
    void portChangeSlot(int index);


private:
    Ui::serialPortsDialog *ui;
};

#endif // SERIALPORTSDIALOG_H
