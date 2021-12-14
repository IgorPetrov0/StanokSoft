#include "usbconnection.h"

usbConnection::usbConnection(QWidget *parent){
    parentWidget = parent;
    currentPort = nullptr;
    programPointer = nullptr;
    runing = false;
    programCounter = 0;

    connect(&statusTimer,SIGNAL(timeout()),this,SLOT(requestTime()));
    connect(&waitTimer,SIGNAL(timeout()),this,SLOT(waitTimeSlot()));

}
/////////////////////////////////////////////////////////////////////////
usbConnection::~usbConnection(){
    deleteCurrentPort();
}
////////////////////////////////////////////////////////////////////////
bool usbConnection::selectPort(){

    if(openPortFromFile()){
        return true;
    }
    else{
        serialPortsDialog SPDialog(parentWidget);

        QSerialPortInfo info;
        QList<QSerialPortInfo> portsList=info.availablePorts();
        if(portsList.size()!=0){
            SPDialog.setPortsList(&portsList);
            if(SPDialog.exec()==QDialog::Accepted){
                deleteCurrentPort();
                currentPort=SPDialog.getPort();
                connect(currentPort,SIGNAL(errorOccurred(QSerialPort::SerialPortError)),this,SLOT(portError(QSerialPort::SerialPortError)));
                connect(currentPort,SIGNAL(readyRead()),this,SLOT(dataReadyRead()));
                return true;
            }
            return false;
        }
        deleteCurrentPort();
        message(tr("В системе не обнаружено СОМ портов."));
        return false;
    }
}
////////////////////////////////////////////////////////////////////////////
bool usbConnection::connectPort(){
    if(currentPort==nullptr){
        if(selectPort()){
            if(currentPort->open(QIODevice::ReadWrite)){
                statusTimer.start(1000);
                return true;
            }
            else{
                message(tr("Невозможно открыть порт")+currentPort->portName()+"\n"+
                        currentPort->errorString());
            }
        }
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////////////
void usbConnection::disconnectPort(){
    deleteCurrentPort();
}
////////////////////////////////////////////////////////////////////////////////////////////////
QString usbConnection::getCurrentPortName(){
    if(currentPort!=nullptr){
        return currentPort->portName();
    }
    return tr("Нет активного порта");
}
////////////////////////////////////////////////////////////////////////////////////
void usbConnection::sendGCode(QString gCode){
    if(currentPort!=nullptr){
        if(currentPort->isOpen()){
            QByteArray outArray(4,0);
            outArray[1]=MESSAGE_G_CODE;
            outArray.append(gCode);
            writeArray(&outArray);
            return;
        }
    }
    emit disconnectedSignal();
}
/////////////////////////////////////////////////////////////////////////////////////////////
void usbConnection::writeArray(QByteArray *array){
    statusTimer.stop();
    waitTimer.stop();
    array->data()[0]=static_cast<char>(array->size()+2);//здесь считаем и устанавливаем размер пакета
    quint16 crc=Crc16(array->data(),array->size());
    array->append((char)0);//добавляем 2 лишних байта в конец штатным способом
    array->append((char)0);
    memcpy(array->data()+(array->size()-2),&crc,2);
    currentPort->write(*array);
    waitTimer.start(100);
    statusTimer.start(1000);
}
///////////////////////////////////////////////////////////////////////////////////////
bool usbConnection::isConnected(){
    if(currentPort!=nullptr){
        return true;
    }
    return false;
}
///////////////////////////////////////////////////////////////////////////////////////
void usbConnection::runProgram(QStringList *program){
    programPointer = program;
    runing = true;
    sendNextComand();
}
//////////////////////////////////////////////////////////////////////////////////
void usbConnection::stopProgram(){
    runing = false;
    programPointer = nullptr;
    programCounter = 0;
}
//////////////////////////////////////////////////////////////////////////////////////
void usbConnection::pauseProgram(){
    runing = false;
}
/////////////////////////////////////////////////////////////////////////////////
void usbConnection::deleteCurrentPort(){
    if(currentPort!=nullptr){
        if(currentPort->isOpen()){
            currentPort->close();
        }
        delete currentPort;
        currentPort=nullptr;
    }
}
//////////////////////////////////////////////////////////////////////////////////
void usbConnection::message(QString text){
    QMessageBox box;
    box.setWindowTitle(tr("Внимание!"));
    box.setIcon(QMessageBox::Critical);
    box.setText(text);
    box.setStandardButtons(QMessageBox::Ok);
    box.exec();
}
///////////////////////////////////////////////////////////////////////////////////////////
quint16 usbConnection::Crc16(char *pcBlock, unsigned short len)
{
    quint16 crc = 0xFFFF;
    unsigned char i;

    while (len--)
    {
        crc ^= *pcBlock++ << 8;

        for (i = 0; i < 8; i++)
            crc = crc & 0x8000 ? (crc << 1) ^ 0x1021 : crc << 1;
    }
    return crc;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void usbConnection::decodePacket(){
    switch(inputBuffer.at(1)){
        case(MESSAGE_STATUS):{
            plotterStatus status;

            int tmp=0;
            memcpy(&tmp,inputBuffer.data()+2,sizeof(int));
            status.posX = static_cast<float>(tmp)/10;//приводим Int к float
            memcpy(&tmp,inputBuffer.data()+2+sizeof(int),sizeof(int));
            status.posY = static_cast<float>(tmp)/10;//приводим Int к float
            memcpy(&tmp,inputBuffer.data()+2+sizeof(int)*2,sizeof(int));
            status.posZ = static_cast<float>(tmp)/10;//приводим Int к float
            if(inputBuffer.at(14) == 1){
                status.swX = true;
            }
            else{
                status.swX = false;
            }
            if(inputBuffer.at(15) == 1){
                status.swY = true;
            }
            else{
                status.swY = false;
            }
            if(inputBuffer.at(16) == 1){
                status.swZ = true;
            }
            else{
                status.swZ = false;
            }
            emit connectedSignal(status);
            break;
        }
        case(MESSAGE_ACKNOWLEDGE):{
            sendNextComand();
            break;
        }
        case(MESSAGE_ERROR):{

            break;
        }
        default:{

        }
    }
    inputBuffer.clear();
}
////////////////////////////////////////////////////////////////////////////////////////////
bool usbConnection::openPortFromFile(){
    QFile file(QApplication::applicationDirPath());
    file.setFileName("comPort.conf");
    if(file.open(QIODevice::ReadOnly)){

        return true;
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////////
bool usbConnection::writeCurrentPortInFile(){
    //todo
}
//////////////////////////////////////////////////////////////////////////////////
void usbConnection::sendNextComand(){
    if(runing){
        if(programCounter < programPointer->size()){
            sendGCode(programPointer->at(programCounter));
            programCounter++;
        }
        else{
            programCounter = 0;
            runing = false;
        }
    }
}
////////////////////////////////////////////////////////////////////////////////////
void usbConnection::portError(QSerialPort::SerialPortError error){
    if(error!=QSerialPort::NoError){
        message(tr("Ошибка порта ")+currentPort->portName()+"\n"+
                currentPort->errorString());
        emit disconnectedSignal();
    }
}
///////////////////////////////////////////////////////////////////////////////
void usbConnection::requestTime(){
    if(currentPort!=nullptr){
        if(currentPort->isOpen()){
            QByteArray outArray(4,0);
            outArray[0]=4;
            outArray[1]=MESSAGE_STATUS;
            //writeArray(&outArray);
            return;
        }
    }
    emit disconnectedSignal();
}
////////////////////////////////////////////////////////////////////////////////////
void usbConnection::dataReadyRead(){
    waitTimer.stop();
    char inputArray[50];
    qint64 inputCount=currentPort->read(inputArray,50);
    inputBuffer.append(inputArray,inputCount);
    if(static_cast<quint64>(inputArray[0]) < inputCount){
        return;
    }
    else{
        quint16 crc1=Crc16(inputBuffer.data(),static_cast<unsigned short>(inputCount-2));
        quint16 crc2=0;
        memcpy(&crc2,inputBuffer.data()+inputCount-2,sizeof(quint16));
        if(crc1==crc2){
            decodePacket();
        }
        else{
            inputBuffer.clear();
        }
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void usbConnection::waitTimeSlot(){
    waitTimer.stop();
    statusTimer.stop();
    programCounter = 0;
    runing = false;
    message(tr("Превышен интервал ожидания ответа от устройства. \n"
               "USB соединение в порядке, но устройство не отвечает."));
    emit disconnectedSignal();

    //deleteCurrentPort();//для отладки. потом раскоментировать
}
//////////////////////////////////////////////////////////////////////////////////////
