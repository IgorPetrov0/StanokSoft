#include "stanokobject.h"

stanokObject::stanokObject(QWidget *parent)
{
    parentWidget=parent;
}
//////////////////////////////////////////////////////////////////////
QString stanokObject::getLastError(){
    QString tmp=lastError;
    lastError.clear();
    return tmp;
}
