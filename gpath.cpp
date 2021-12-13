#include "gpath.h"

GPath::GPath()
{
    pointsArray=nullptr;
    currentApperture=nullptr;
    valide=false;

}
/////////////////////////////////////////////////////////////////////////
GPath::~GPath(){
    if(pointsArray!=nullptr){
        foreach (QPointF *nextPoint, *pointsArray){
           delete nextPoint;
        }
        delete pointsArray;
    }
}
//////////////////////////////////////////////////////////////////////////
void GPath::setApp(apperture *value){
    currentApperture = value;
}
/////////////////////////////////////////////////////////////////////////
void GPath::addPoint(QPointF *point){
    if(pointsArray==nullptr){
        pointsArray = new QVector<QPointF*>;
    }
    pointsArray->append(point);
    if(pointsArray->size()>1){//путь считается валидным, если состоит из более, чем одной точки
        valide=true;
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void GPath::addPath(GPath *path){
    int size=path->getPointsCount();
    int n=0;
    if(*path->startPoint() == *this->endtPoint()){//не записываем начальную точку, если она равна конечной
        n=1;
    }
    for(;n!=size;n++){
        this->addPoint(path->getPoint(n));
    }
}
///////////////////////////////////////////////////////////////////////////////
QPointF *GPath::startPoint(){
    if(pointsArray!=nullptr){
        if(pointsArray->size()!=0){
            return pointsArray->at(0);
        }
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////
QPointF *GPath::endtPoint(){
    if(pointsArray!=nullptr){
        if(pointsArray->size()!=0){
            return pointsArray->at(pointsArray->size()-1);
        }
    }
    return nullptr;
}
///////////////////////////////////////////////////////////////////////////
int GPath::getPointsCount(){
    if(pointsArray!=nullptr){
        return pointsArray->size();
    }
    return 0;
}
/////////////////////////////////////////////////////////////////////////////////
QStringList GPath::calcGCode(float penDiameter, float force){
    QStringList tmpProg;
    //исходим из того, что для рисования дорожек всегда используется круглая аппертура
    if(currentApperture->getType()!=APP_CYCLE){
        return tmpProg;
    }
    int size=pointsArray->size();
    tmpProg.append("G00 X"+QString::number(pointsArray->at(0)->x())+" Y"+QString::number(pointsArray->at(0)->y())+" F"+QString::number(force)+"\n");//перемещаемся в начало пути
    tmpProg.append("G00 Z"+QString::number(zMove)+"\n");//опускаем в позицию рисования, делаем точку
    for(int n=0;n!=size;n++){//строим центральную линию
        QPointF *point=pointsArray->at(n);
        float X=point->x();
        float Y=point->y();
        tmpProg.append("G01 X"+QString::number(X)+" Y"+QString::number(Y)+" F"+QString::number(force)+"\n");
    }

    float penRadius=penDiameter/2;
    int lines=(currentApperture->getXSize()/2)/penRadius;

    for(int n=1;n!=lines+1;n++){
        for(int m=size-1;m!=0;m--){//строим параллельные линии с положительной стороны от центральной
            //идем с конца т.к. при построении центральной линии перо оказывается в ее конце
            QPointF perPoint1=perOffset(pointsArray->at(m),pointsArray->at(m-1),penRadius*n);//начальная точка
            QPointF perPoint2=perOffset(pointsArray->at(m-1),pointsArray->at(m),-penRadius*n);//конечная точка
            tmpProg.append("G01 X"+QString::number(perPoint1.x())+" Y"+QString::number(perPoint1.y())+"\n");//смещаемся перпендикулярно заданному отрезку
            tmpProg.append("G01 X"+QString::number(perPoint2.x())+" Y"+QString::number(perPoint2.y())+"\n");//смещаемся в конечную точку
        }
        for(int m=1;m!=size;m++){//строим параллельные линии с отрицательной стороны от центральной
            //идем с конца т.к. при построении центральной линии перо оказывается в ее конце
            QPointF perPoint1=perOffset(pointsArray->at(m),pointsArray->at(m-1),-penRadius*n);//начальная точка
            QPointF perPoint2=perOffset(pointsArray->at(m-1),pointsArray->at(m),penRadius*n);//конечная точка
            tmpProg.append("G01 X"+QString::number(perPoint2.x())+" Y"+QString::number(perPoint2.y())+"\n");//смещаемся перпендикулярно заданному отрезку
            tmpProg.append("G01 X"+QString::number(perPoint1.x())+" Y"+QString::number(perPoint1.y())+"\n");//смещаемся в конечную точку
        }
    }
    tmpProg.append("G00 Z"+QString::number(0)+"\n");
    return tmpProg;
}
/////////////////////////////////////////////////////////////////////////////////////////////
bool GPath::isValide(){
    return valide;
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool GPath::isApperture(){
    if(currentApperture!=nullptr){
        return true;
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////
float GPath::appertureSize(){
    return currentApperture->getXSize();
}
///////////////////////////////////////////////////////////////////////////////////////////////////
QPointF *GPath::getPoint(int index){
    return pointsArray->at(index);
}
///////////////////////////////////////////////////////////////////////////////////////////////
void GPath::convertCoordinates(float zeroX, float zeroY){
    int size = pointsArray->size();
    foreach (QPointF *nextPoint, *pointsArray) {
        nextPoint->setX(nextPoint->x() - zeroX);
        nextPoint->setY(nextPoint->y() - zeroY);
    }

}
//////////////////////////////////////////////////////////////////////////////////////////////
QPointF GPath::perOffset(QPointF *point1, QPointF *point2, float distance){
    QPointF rezult = *point1;
    QVector2D vector(*point2 - *point1);
    vector.normalize();
    QVector2D perpendicular;
    float X2=0;
    float Y2=0;
    if(vector.x()!=0){
        if(vector.x()>0){
            Y2=1;
        }
        else{
            Y2=-1;
        }
        X2=-(vector.y()*Y2)/vector.x();
        perpendicular.setX(X2);
        perpendicular.setY(Y2);
    }
    else if(vector.y()!=0){
        if(vector.y()>0){
            X2=-1;
        }
        else{
            X2=1;
        }
        Y2=-(vector.x()*X2)/vector.y();
        perpendicular.setX(X2);
        perpendicular.setY(Y2);
    }
    else{//если обе координаты =0, то возвращаем начальную точку
        return rezult;
    }
    perpendicular.normalize();
    perpendicular *= distance;
    rezult = *point1 + perpendicular.toPointF();
    return rezult;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
QPointF GPath::parLengthOffset(QPointF startPoint, QPoint parPoint1, QPointF parPoint2){

}
