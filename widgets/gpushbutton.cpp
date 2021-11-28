#include "gpushbutton.h"

GPushButton::GPushButton(QWidget *parent):
    QPushButton(parent)
{
    connect(this,SIGNAL(clicked(bool)),this,SLOT(clickedSlot(bool)));
    gCode.append("G91");
    gCode.append("G90");
}
/////////////////////////////////////////////////////////////////////////////////
void GPushButton::setGCode(QString code){
    gCode.insert(1,code);
}
//////////////////////////////////////////////////////////////////////////////
QStringList *GPushButton::getGCode(){
    return &gCode;
}
//////////////////////////////////////////////////////////////////////////
void GPushButton::clickedSlot(bool){
    emit clickedSignal(this);
}
