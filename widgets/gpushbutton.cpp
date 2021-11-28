#include "gpushbutton.h"

GPushButton::GPushButton(QWidget *parent):
    QPushButton(parent)
{
    connect(this,SIGNAL(clicked(bool)),this,SLOT(clickedSlot(bool)));
}
/////////////////////////////////////////////////////////////////////////////////
void GPushButton::setGCode(QString code){
    gCode = code;
}
//////////////////////////////////////////////////////////////////////////////
QString GPushButton::getGCode(){
    return gCode;
}
//////////////////////////////////////////////////////////////////////////
void GPushButton::clickedSlot(bool){
    emit clickedSignal(this);
}
