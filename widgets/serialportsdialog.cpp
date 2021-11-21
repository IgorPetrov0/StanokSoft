#include "serialportsdialog.h"
#include "ui_serialportsdialog.h"

    serialPortsDialog::serialPortsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::serialPortsDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("Настройка последовательных портов"));

    currentPort=nullptr;
    currentPortsList=nullptr;

    fillComboBoxes();
    connect(ui->canselButton,SIGNAL(clicked(bool)),this,SLOT(reject()));
    connect(ui->okButton,SIGNAL(clicked(bool)),this,SLOT(saveSlot()));
    connect(ui->portName,SIGNAL(currentIndexChanged(int)),this,SLOT(portChangeSlot(int)));
}
///////////////////////////////////////////////////////////////////
serialPortsDialog::~serialPortsDialog()
{
    delete ui;
}
///////////////////////////////////////////////////////////////////////////////////////
void serialPortsDialog::setPort(QSerialPortInfo port){
    if(currentPort!=nullptr){
        delete currentPort;
    }
    currentPort = new QSerialPort(port);
    int size=ui->baudRate->count();
    for(int n=0;n!=size;n++){
        if(ui->baudRate->itemData(n)==currentPort->baudRate()){
            ui->baudRate->setCurrentIndex(n);
            break;
        }
    }
    size=ui->parity->count();
    for(int n=0;n!=size;n++){
        if(ui->parity->itemData(n)==currentPort->parity()){
            ui->parity->setCurrentIndex(n);
            break;
        }
    }
    size=ui->dataBits->count();
    for(int n=0;n!=size;n++){
        if(ui->dataBits->itemData(n)==currentPort->dataBits()){
            ui->dataBits->setCurrentIndex(n);
            break;
        }
    }
    size=ui->stopBits_1->count();
    for(int n=0;n!=size;n++){
        if(ui->stopBits_1->itemData(n)==currentPort->dataBits()){
            ui->stopBits_1->setCurrentIndex(n);
            break;
        }
    }
    size=ui->flowControl->count();
    for(int n=0;n!=size;n++){
        if(ui->flowControl->itemData(n)==currentPort->flowControl()){
            ui->flowControl->setCurrentIndex(n);
            break;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////
void serialPortsDialog::setPortsList(QList<QSerialPortInfo> *list){
    currentPortsList=list;
    int size=list->size();
    for(int n=0;n!=size;n++){
        ui->portName->addItem(list->at(n).portName());
    }
    ui->portName->setCurrentIndex(0);
    setPort(list->at(0));
}
//////////////////////////////////////////////////////////////////////////////
QSerialPort *serialPortsDialog::getPort(){
    return currentPort;
}
////////////////////////////////////////////////////////////////////////////
void serialPortsDialog::fillComboBoxes(){
    ui->baudRate->addItem("1200",QSerialPort::Baud1200);
    ui->baudRate->addItem("2400",QSerialPort::Baud2400);
    ui->baudRate->addItem("4800",QSerialPort::Baud4800);
    ui->baudRate->addItem("9600",QSerialPort::Baud9600);
    ui->baudRate->addItem("19200",QSerialPort::Baud19200);
    ui->baudRate->addItem("38400",QSerialPort::Baud38400);
    ui->baudRate->addItem("57600",QSerialPort::Baud57600);
    ui->baudRate->addItem("115200",QSerialPort::Baud115200);

    ui->dataBits->addItem("5",QSerialPort::Data5);
    ui->dataBits->addItem("6",QSerialPort::Data6);
    ui->dataBits->addItem("7",QSerialPort::Data7);
    ui->dataBits->addItem("8",QSerialPort::Data8);

    ui->parity->addItem("No parity",QSerialPort::NoParity);
    ui->parity->addItem("Even parity",QSerialPort::EvenParity);
    ui->parity->addItem("Odd parity",QSerialPort::OddParity);
    ui->parity->addItem("Space parity",QSerialPort::SpaceParity);
    ui->parity->addItem("Mark parity",QSerialPort::MarkParity);

    ui->stopBits_1->addItem("1",QSerialPort::OneStop);
    ui->stopBits_1->addItem("1.5",QSerialPort::OneAndHalfStop);
    ui->stopBits_1->addItem("2",QSerialPort::TwoStop);

    ui->flowControl->addItem(tr("Нет"),QSerialPort::NoFlowControl);
    ui->flowControl->addItem(tr("Аппаратный"),QSerialPort::HardwareControl);
    ui->flowControl->addItem(tr("Программный"),QSerialPort::SoftwareControl);

}
////////////////////////////////////////////////////////////////////////////////
void serialPortsDialog::saveSlot(){
    if(currentPort!=nullptr){
        currentPort->setBaudRate((QSerialPort::BaudRate)ui->baudRate->currentData().toInt());
        currentPort->setDataBits((QSerialPort::DataBits)ui->dataBits->currentData().toInt());
        currentPort->setFlowControl((QSerialPort::FlowControl)ui->flowControl->currentData().toInt());
        currentPort->setParity((QSerialPort::Parity)ui->parity->currentData().toInt());
        currentPort->setStopBits((QSerialPort::StopBits)ui->stopBits_1->currentData().toInt());
    }
    accept();
}
///////////////////////////////////////////////////////////////////////////////////////////////
void serialPortsDialog::portChangeSlot(int index){
    if(currentPortsList!=nullptr){
        setPort(currentPortsList->at(index));
    }
}
