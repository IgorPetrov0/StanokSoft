#include "directmanagewidget.h"
#include "ui_directmanagewidget.h"

directManageWidget::directManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::directManageWidget)
{
    ui->setupUi(this);

    ui->Xm01Button->setGCode("G00 X-0.1 F2\n");
    ui->Xm1Button->setGCode("G00 X-1 F2\n");
    ui->Xm10Button->setGCode("G00 X-10 F2\n");
    ui->Xp01Button->setGCode("G00 X0.1 F2\n");
    ui->Xp1Button->setGCode("G00 X1 F2\n");
    ui->Xp10Button->setGCode("G00 X10 F2\n");

    ui->Ym01Button->setGCode("G00 Y-0.1 F2\n");
    ui->Ym1Button->setGCode("G00 Y-1 F2\n");
    ui->Ym10Button->setGCode("G00 Y-10 F2\n");
    ui->Yp01Button->setGCode("G00 Y0.1 F2\n");
    ui->Yp1Button->setGCode("G00 Y1 F2\n");
    ui->Yp10Button->setGCode("G00 Y10 F2\n");

    ui->Zm01Button->setGCode("G00 Z-0.1 F2\n");
    ui->Zm1Button->setGCode("G00 Z-1 F2\n");
    ui->Zm10Button->setGCode("G00 Z-10 F2\n");
    ui->Zp01Button->setGCode("G00 Z0.1 F2\n");
    ui->Zp1Button->setGCode("G00 Z1 F2\n");
    ui->Zp10Button->setGCode("G00 Z10 F2\n");


    connect(ui->zeroPosButton,SIGNAL(clicked(bool)),this,SLOT(moveZeroSlot()));
    connect(ui->zeroButton,SIGNAL(clicked(bool)),this,SLOT(setZeroSlot()));
    connect(ui->disSteppersButton,SIGNAL(clicked(bool)),this,SLOT(diableSteppersSlot()));
    connect(ui->sendGCode,SIGNAL(clicked(bool)),this,SLOT(sendGCodeSlot()));

    connect(ui->Xm01Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Xm1Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Xm10Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Xp01Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Xp1Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Xp10Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));

    connect(ui->Ym01Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Ym1Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Ym10Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Yp01Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Yp1Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Yp10Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));

    connect(ui->Zm01Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Zm1Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Zm10Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Zp01Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Zp1Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));
    connect(ui->Zp10Button,SIGNAL(clickedSignal(GPushButton*)),this,SLOT(directMoveButtonSlot(GPushButton*)));

    error = false;
}
///////////////////////////////////////////////////////////////////////////
directManageWidget::~directManageWidget(){
    delete ui;
}
/////////////////////////////////////////////////////////////////////////////
void directManageWidget::setPlotterStatus(plotterStatus status){
    ui->posXLable->setText(QString::number(status.posX));
    ui->posYLable->setText(QString::number(status.posY));
    ui->posZLable->setText(QString::number(status.posZ));
}
////////////////////////////////////////////////////////////////////////////////////
directManageWidget::setSwitchStatus(bool X, bool Y, bool Z){
    if(X){
        ui->swXLable->setText("Замкнут");
    }
    else{
        ui->swXLable->setText("Разомкнут");
    }

    if(Y){
        ui->swYLable->setText("Замкнут");
    }
    else{
        ui->swYLable->setText("Разомкнут");
    }

    if(Z){
        ui->swZLable->setText("Замкнут");
    }
    else{
        ui->swZLable->setText("Разомкнут");
    }
}
///////////////////////////////////////////////////////////////////////////////////
bool directManageWidget::checkG01FromGLine(){
    bool ok = checkXYZFromGLine();
    if(!ok){
        emit errorSignal("Для команд G00 и G01 дожен быть хотябы один параметр X, Y или Z.");
    }
    return ok;
}
/////////////////////////////////////////////////////////////////////////////
bool directManageWidget::checkG0203FromGLine(){
    bool ok = false;
    int IJOk = 0;
    bool ROk = false;

    ok = checkXYZFromGLine();//X Y Z может не быть, но проверить на правильность все равно надо
    if(checkParameterFromGLine("I")){
        IJOk++;
    }
    if(checkParameterFromGLine("J")){
        IJOk++;
    }
    ROk = checkParameterFromGLine("R");

    if((IJOk > 0) || ROk){//если присутствуют или IJ или R
        return true;
    }
    error = true;
    errorSignal("В командах G02 и G03 должны быть или значения IJ или R.");
    return false;
}
//////////////////////////////////////////////////////////////////////////////
bool directManageWidget::checkXYZFromGLine(){
    int tmp=0;

    if(checkParameterFromGLine("X")){
        tmp++;
    }
    if(checkParameterFromGLine("Y")){
        tmp++;
    }
    if(checkParameterFromGLine("Z")){
        tmp++;
    }

    if(tmp > 0){
        return true;
    }
    return false;//поиск и проверка хотябы одной координаты должны выставить ok в значение true
}
//////////////////////////////////////////////////////////////////////////////////
bool directManageWidget::checkParameterFromGLine(QString parameter){
    bool ok = false;
    int bIndex = 0;
    int eIndex = 0;
    bIndex = ui->GlineEdit->text().indexOf(parameter,0);//ищем координату X
    if(bIndex != -1){//если нашли
        eIndex = ui->GlineEdit->text().indexOf(" ",bIndex);//то ищем пробел
        if(eIndex == -1){//если пробела не нашли
            eIndex = ui->GlineEdit->text().size()-1;//то ищем до конца строки
        }
        QString coord = ui->GlineEdit->text().mid(bIndex+1,eIndex-bIndex);
        coord.toFloat(&ok);
        if(!ok){
            emit errorSignal("Ошибка в значении " + parameter + ". Символы не являются числом.");
            error = true;
        }
        return ok;
    }
    return false;
}
/////////////////////////////////////////////////////////////////////////////
void directManageWidget::moveZeroSlot(){
    emit sendGCode("G28");
}
/////////////////////////////////////////////////////////////////////////////
void directManageWidget::setZeroSlot(){
    emit sendGCode("G92");
}
///////////////////////////////////////////////////////////////////////////////////
void directManageWidget::diableSteppersSlot(){
    emit sendGCode("M18");
}
//////////////////////////////////////////////////////////////////////////////////////
void directManageWidget::sendGCodeSlot(){
    error = false;
    //проверка на допустимые символы
    int pos = ui->GlineEdit->text().indexOf(QRegularExpression("[^GMXYZ0-9.IJREF -]"),0);
    if(pos != -1){
        errorSignal("Символ " + ui->GlineEdit->text().mid(pos,1) + " недопустим.");
        return;
    }

    if(ui->GlineEdit->text().at(0) == 'G'){
        QString tmp = ui->GlineEdit->text().mid(1,2);
        bool ok = false;
        int command = tmp.toInt(&ok);
        if(ok){
            ok = false;
            switch(command){
                case(0):{
                    ok = checkG01FromGLine();
                    break;
                }
                case(1):{
                    ok = checkG01FromGLine();
                    break;
                }
                case(2):{
                    ok = checkG0203FromGLine();
                    break;
                }
                case(3):{
                    ok = checkG0203FromGLine();
                    break;
                }
                default:{
                    errorSignal("Команда G" + ui->GlineEdit->text().mid(1,2) + " не поддерживается!");
                }
            }
            if(ok){
                if(!error){
                    if(!checkParameterFromGLine("F")){//если в строке нет параметра F
                        ui->GlineEdit->setText(ui->GlineEdit->text()+" F"+QString::number(ui->speedSpinBox->value()));//то добавляем
                    }
                    sendGCode(ui->GlineEdit->text()+"\n");
                }
            }
        }
        else{
            errorSignal("Символы " + ui->GlineEdit->text().mid(1,2) + " не являются числом!");
        }
    }
    else if(ui->GlineEdit->text().at(0) == 'M'){
        QString tmp = ui->GlineEdit->text().mid(1,2);
        bool ok = false;
        int command = tmp.toInt(&ok);
        if(command == 18){
            sendGCode(ui->GlineEdit->text());
        }
    }
    else{
        errorSignal("Символ " + ui->GlineEdit->text().mid(0,1) + " не является командой!");
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void directManageWidget::directMoveButtonSlot(GPushButton *pointer){
    QStringList *code = pointer->getGCode();
    emit sendProgram(code);
}
///////////////////////////////////////////////////////////////////////////////


