#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    USBPort = new usbConnection(this);
    conIndicator = new connectionIndicator(tr("Plotter"));
    Gconverter = new gerberConverter(this);
    conIndicator->setMinimumSize(30,10);

    portName = new QLabel;
    portName->setText(tr("Порт не подключен."));

    statusBar()->addPermanentWidget(portName,1);
    statusBar()->addPermanentWidget(conIndicator,0);
    statusBar()->setSizeGripEnabled(true);

    penDiametersArray.append(1);

    connect(ui->conntcnAction,SIGNAL(triggered(bool)),this,SLOT(connectSlot()));
    connect(USBPort,SIGNAL(statusSignal(plotterStatus)),this,SLOT(statusSlot(plotterStatus)));
    connect(USBPort,SIGNAL(disconnectedSignal()),this,SLOT(disconnectedSlot()));
    connect(USBPort,SIGNAL(messageSignal(QString)),this,SLOT(messageSlot(QString)));
    connect(ui->DMWidget,SIGNAL(sendGCode(QString)),this,SLOT(sendGCode(QString)));
    connect(ui->DMWidget,SIGNAL(errorSignal(QString)),this,SLOT(errorSlot(QString)));
    connect(ui->DMWidget,SIGNAL(sendProgram(QStringList*)),this,SLOT(sendProgramm(QStringList*)));
    connect(ui->openAction,SIGNAL(triggered(bool)),this,SLOT(openSlot()));
    connect(ui->penDSpinBox,SIGNAL(valueChanged(double)),this,SLOT(penChangeSlot()));
    connect(ui->ZSpinBox,SIGNAL(valueChanged(double)),this,SLOT(zChangeSlot()));
    connect(ui->FSpinBox,SIGNAL(valueChanged(double)),this,SLOT(fChangeSlot()));
    connect(ui->FMSpinBox,SIGNAL(valueChanged(double)),this,SLOT(fmChangeSlot()));
    connect(ui->runPushButton,SIGNAL(clicked(bool)),this,SLOT(executeSlot()));
    connect(ui->stopPushButton,SIGNAL(clicked(bool)),this,SLOT(stopSlot()));
    connect(ui->gCodeTextEdit,SIGNAL(updateViewSignal()),this,SLOT(updateViewSlot()));
    connect(Gconverter,SIGNAL(messageSignal(QString)),this,SLOT(messageSlot(QString)));

    distribForms();
}
////////////////////////////////////////////////////////////////////////////////////
MainWindow::~MainWindow()
{
    delete portName;
    delete conIndicator;
    delete Gconverter;
    delete ui;
}
////////////////////////////////////////////////////////////////////////////////////
void MainWindow::connectSlot(){
    if(USBPort->isConnected()){
        USBPort->disconnectPort();
        ui->conntcnAction->setText("Подключиться");
    }
    else{
        if(USBPort->connectPort()){
            ui->consoleWidget->addString("Подключение");
            ui->conntcnAction->setText("Отключиться");
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::statusSlot(plotterStatus status){
    conIndicator->setState(true);
    ui->DMWidget->setPlotterStatus(status);
    portName->setText(USBPort->getCurrentPortName());
    ui->DMWidget->setSwitchStatus(status.swX,status.swY,status.swZ);
    ui->consoleWidget->addString("Статусное сообщение");
}
///////////////////////////////////////////////////////////////////////////////////////
void MainWindow::disconnectedSlot(){
    conIndicator->setState(false);
    ui->consoleWidget->addString("Отключение");
}
///////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::sendGCode(QString gCode){
    USBPort->sendGCode(gCode);
    ui->consoleWidget->addString("Код " + gCode + " отправлен в контроллер.");
}
////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::sendProgramm(QStringList *program){
    USBPort->runProgram(program);
    ui->consoleWidget->addString("Программа отправлена в контроллер");
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::openSlot(){
    QFileDialog dialog(this);
    dialog.setDirectory("D:\\");
    dialog.setNameFilter(tr("Поддерживаемые форматы (*.gbr)"));
    QString fileName=dialog.getOpenFileName();
    if(!fileName.isEmpty()){
        QFile file(fileName);
        if(!file.open(QIODevice::ReadOnly)){
            errorSlot(file.errorString(),tr("Ошибка открытия файла ")+file.fileName());
            return;
        }
        ui->consoleWidget->addString("Файл " + fileName + " открыт.");
        ui->gCodeTextEdit->reset();
        ui->vWidget->clearAll();
        ui->gCodeTextEdit->reset();
        Gconverter->allClear();

        if(!Gconverter->convertGerberCode(&file)){
            errorSlot(Gconverter->getLastError(),tr("Ошибка преобразования")+file.fileName());
            return;
        }
        file.close();
        ui->gCodeTextEdit->setPlainText(Gconverter->getGCode());
        if(ui->gCodeTextEdit->parseGCode()){
            ui->consoleWidget->addString("Парсинг " + fileName + " успешно завершен.");
        }
        else{
            errorSlot(tr("Ошибка синтаксиса G-кода ")+file.fileName());
            return;
        }
        ui->vWidget->drawProgramm(ui->gCodeTextEdit->getPainterProgramm());
        ui->DMWidget->setWorkRect(Gconverter->getWorkRect());
    }   
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::penChangeSlot(){
    Gconverter->setPenDiameter(ui->penDSpinBox->value());
    ui->gCodeTextEdit->setPenDiameter(ui->penDSpinBox->value());
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::zChangeSlot(){
    Gconverter->setZOffset(ui->ZSpinBox->value());
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::fChangeSlot(){
    Gconverter->setForce(ui->FSpinBox->value());
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::fmChangeSlot(){
    Gconverter->setMoveSpeed(ui->FMSpinBox->value());
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::executeSlot(){
    sendProgramm(Gconverter->getProgramm());
}
///////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::updateViewSlot(){
    ui->vWidget->update();
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::errorSlot(QString error, QString title){
    QMessageBox box(this);
    box.setIcon(QMessageBox::Critical);
    if(title != NULL){
        box.setWindowTitle(title);
    }
    else{
        box.setWindowTitle("Ошибка");
    }
    box.setStandardButtons(QMessageBox::Ok);
    box.setText(error);
    box.exec();
    ui->consoleWidget->addString(error);
}
//////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::stopSlot(){
    USBPort->stopProgram();
    ui->consoleWidget->addString("Останов программы");
}
/////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::pauseSlot(){
    USBPort->pauseProgram();
    ui->consoleWidget->addString("Пауза");
}
/////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::messageSlot(QString message){
    ui->consoleWidget->addString(message);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::distribForms(){
    ui->penDSpinBox->setValue(1);
    ui->ZSpinBox->setValue(-10);
    ui->FSpinBox->setValue(0.5);
    ui->FMSpinBox->setValue(2);
}
