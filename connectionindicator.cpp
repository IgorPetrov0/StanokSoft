#include "connectionindicator.h"

connectionIndicator::connectionIndicator(QString name){
    this->name=name;
    color=Qt::red;
    setToolTip(name+tr(" disconnected"));
}
////////////////////////////////////////////////////////////////
void connectionIndicator::paintEvent(QPaintEvent *event){
    QRect g=this->geometry();
    QPainter painter(this);
    painter.setPen(color);
    painter.setBrush(QBrush(color,Qt::SolidPattern));
    painter.drawEllipse(QPoint(g.width()-15,g.height()/2),7,7);
}
//////////////////////////////////////////////////////////////////
void connectionIndicator::setState(bool state){
    if(state==true){
        color=Qt::green;
        setToolTip(name+tr(" connected"));
    }
    else{
        color=Qt::red;
        setToolTip(name+tr(" disconnected"));
    }
    update();
}
//////////////////////////////////////////////////////////////////
