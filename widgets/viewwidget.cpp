#include "viewwidget.h"
#include "ui_viewwidget.h"

viewWidget::viewWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::viewWidget)
{
    ui->setupUi(this);
    converterPointer=nullptr;
    scaleX=1;
    scaleY=1;
    translateX=0;
    translateY=0;
    mousePosX=0;
    mousePosY=0;
    setMouseTracking(true);
    graphProgramm=nullptr;
}
//////////////////////////////////////////////////////////////////////////////////
void viewWidget::drawRawData(gerberConverter *converter){
    converterPointer=converter;
    update();
}
//////////////////////////////////////////////////////////////////////////////////
void viewWidget::drawProgramm(QVector<drawCommand> *programm){
    graphProgramm=programm;
    update();
}
//////////////////////////////////////////////////////////////////////
viewWidget::~viewWidget()
{
    delete ui;
}
///////////////////////////////////////////////////////////////////////////
void viewWidget::paintEvent(QPaintEvent*){
    QPainter painter(this);
    QPen pen;
    //рисуем рамку
    pen.setColor(QColor(0,0,0));
    pen.setWidth(1);
    painter.setPen(pen);
    painter.drawLine(1,1,this->width()-1,1);
    painter.drawLine(this->width()-1,1,this->width()-1,this->height()-1);
    painter.drawLine(this->width()-1,this->height()-1,1,this->height()-1);
    painter.drawLine(1,this->height()-1,1,1);
    painter.drawText(100,10,"scaleX= "+QString::number(scaleX));
    painter.drawText(100,20,"scaleY= "+QString::number(scaleY));
    painter.drawText(100,30,"translateX= "+QString::number(translateX));
    painter.drawText(100,40,"translateY= "+QString::number(translateY));

    painter.scale(scaleX,scaleY);
    painter.translate(translateX,translateY);
    painter.setPen(pen);

    //для отрисовки сырых данных
//    if(converterPointer!=nullptr){
//        QVector<pad*> *padsArray = converterPointer->getPadsArray();
//        QVector<GPath*> *pathsArray=converterPointer->getPathsArray();

//        int pathsSize=pathsArray->size();

//        for(int n=0;n!=pathsSize;n++){
//            GPath *path=pathsArray->at(n);
//            QVector<QPointF> *pointsArray=path->getPointsArray();
//            int lines=path->getPointsCount()-1;
//            for(int m=0;m!=lines;m++){
//                painter.drawLine(pointsArray->at(m)*scaleX,pointsArray->at(m+1)*scaleX);
//            }
//        }
//    }
    if(graphProgramm!=nullptr){
        int size=graphProgramm->size();
        for(int n=0;n!=size;n++){
            drawCommand com=graphProgramm->at(n);
            com.draw(&painter);
        }
    }




}
///////////////////////////////////////////////////////////////////////////
void viewWidget::wheelEvent(QWheelEvent *event){
    QPoint s=event->angleDelta();
    if(s.y()>0){
        scaleX+=0.1;
        scaleY+=0.1;
    }
    else if(s.y()<0){
        scaleX-=0.1;
        scaleY-=0.1;
    }
    update();
}
////////////////////////////////////////////////////////////////////////////////
void viewWidget::mouseMoveEvent(QMouseEvent *event){
    if(event->buttons()==Qt::LeftButton){
        int x=event->x()-mousePosX;
        int y=event->y()-mousePosY;
        if(x>0){
            translateX+=5;
        }
        else{
            translateX-=5;
        }
        if(y>0){
            translateY+=5;
        }
        else{
            translateY-=5;
        }

        update();
    }
    mousePosX=event->x();
    mousePosY=event->y();
}
///////////////////////////////////////////////////////////////////////////////////
void viewWidget::mousePressEvent(QMouseEvent *event){

}
//////////////////////////////////////////////////////////////////////////////////
void viewWidget::clearAll(){
    converterPointer=nullptr;
    scaleX=1;
    scaleY=1;
    translateX=0;
    translateY=0;
}
