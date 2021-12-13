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
    connect(USBPort,SIGNAL(connectedSignal(plotterStatus)),this,SLOT(connectedSlot(plotterStatus)));
    connect(USBPort,SIGNAL(disconnectedSignal()),this,SLOT(disconnectedSlot()));
    connect(ui->DMWidget,SIGNAL(sendGCode(QString)),this,SLOT(sendGCode(QString)));
    connect(ui->DMWidget,SIGNAL(errorSignal(QString)),this,SLOT(errorSlot(QString)));
    connect(ui->DMWidget,SIGNAL(sendProgram(QStringList*)),this,SLOT(sendProgramm(QStringList*)));
    connect(ui->openAction,SIGNAL(triggered(bool)),this,SLOT(openSlot()));
    connect(ui->ZSpinBox,SIGNAL(valueChanged(double)),this,SLOT(zChangeSlot()));
    connect(ui->FSpinBox,SIGNAL(valueChanged(double)),this,SLOT(fChangeSlot()));
    connect(ui->runPushButton,SIGNAL(clicked(bool)),this,SLOT(executeSlot()));
    connect(ui->gCodeTextEdit,SIGNAL(updateViewSignal()),this,SLOT(updateViewSlot()));
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
        USBPort->connectPort();
        ui->conntcnAction->setText("Отключиться");
    }
}
/////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::connectedSlot(plotterStatus status){
    conIndicator->setState(true);
    ui->DMWidget->setPlotterStatus(status);
    portName->setText(USBPort->getCurrentPortName());
    ui->DMWidget->setSwitchStatus(status.swX,status.swY,status.swZ);
}
///////////////////////////////////////////////////////////////////////////////////////
void MainWindow::disconnectedSlot(){
    conIndicator->setState(false);
}
///////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::sendGCode(QString gCode){
    USBPort->sendGCode(gCode);
}
////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::sendProgramm(QStringList *program){
    USBPort->runProgram(program);
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
        ui->gCodeTextEdit->clear();
        ui->vWidget->clearAll();
        if(!Gconverter->convertGerberCode(&file)){
            errorSlot(Gconverter->getLastError(),tr("Ошибка преобразования")+file.fileName());
            return;
        }
        file.close();
        ui->gCodeTextEdit->setPlainText(Gconverter->getGCode());
        if(ui->gCodeTextEdit->parseGCode()){

        }
        else{
            errorSlot(tr("Ошибка синтаксиса G-кода ")+file.fileName());
            return;
        }
        ui->vWidget->drawProgramm(ui->gCodeTextEdit->getPainterProgramm());
        //ui->vWidget->drawRawData(Gconverter);
    }   
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::penChangeSlot(){
    Gconverter->setPenDiameter(ui->penComboBox->currentData().toInt());
    ui->gCodeTextEdit->setPenDiameter(ui->penComboBox->currentData().toFloat());
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
}
///////////////////////////////////////////////////////////////////////////////////////////////
void MainWindow::distribForms(){
    int size=penDiametersArray.size();
    disconnect(ui->penComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(penChangeSlot()));
    for(int n=0;n!=size;n++){
        ui->penComboBox->addItem(QString::number(penDiametersArray.at(n)));
        ui->penComboBox->setItemData(n,penDiametersArray.at(n));
    }
    ui->penComboBox->addItem(tr("Управление."));
    connect(ui->penComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(penChangeSlot()));
    ui->penComboBox->setCurrentIndex(0);
    penChangeSlot();
    ui->ZSpinBox->setValue(1);
    ui->FSpinBox->setValue(2);
    Gconverter->setForce(2);
    Gconverter->setZOffset(1);
    Gconverter->setPenDiameter(1);
}
