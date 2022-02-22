#include "apperture.h"

apperture::apperture(){
    type=APP_UNDEFINED;
    xSize=0;
    ySize=0;
    number=0;
}
////////////////////////////////////////////////////////////////////
void apperture::setType(const appertureType &value){
    type = value;
}
////////////////////////////////////////////////////////////////
void apperture::setXSize(float value){
    xSize = value;
}
//////////////////////////////////////////////////////////////////
void apperture::setYSize(float value){
    ySize = value;
}
//////////////////////////////////////////////////////////////////////
void apperture::setNumber(float value){
    number = value;
}
/////////////////////////////////////////////////////////////////////////
appertureType apperture::getType(){
    return type;
}
/////////////////////////////////////////////////////////////////////////
float apperture::getXSize(){
    return xSize;
}
////////////////////////////////////////////////////////////////////////////
float apperture::getYSize(){
    return ySize;
}
///////////////////////////////////////////////////////////////////////////
bool apperture::readApperture(QString string){
    QString tmpStr;
    int tmp1=0;
    bool ok=false;

    tmp1=string.indexOf(",");
    tmpStr=string.mid(tmp1-1,1);
    type=getAppType(tmpStr);//читаем тип
    if(type==APP_UNDEFINED){
        return false;
    }
    tmpStr=string.left(tmp1-1);
    number=tmpStr.toInt(&ok);//читаем номер
    if(!ok){
        return false;
    }
    //читаем остальную информацию в зависимости от типа аппертуры
    return readData(string.remove(0,tmp1+1));
}
/////////////////////////////////////////////////////////////////////////////////////////////
int apperture::getNumber() const{
    return number;
}
////////////////////////////////////////////////////////////////////////////////////////////////
bool apperture::isInApperture(QPointF *point1, QPointF *point2){
    float difX=0;
    float difY=0;

    switch(type){
        case(APP_CYCLE):{
            difX=point1->x()-point2->x();
            difY=point1->y()-point2->y();
            float radius=xSize/2;
            if((abs(difX)<=radius) && (abs(difY)<=radius)){
                return true;
            }
            return false;
        }
        case(APP_OVAL):{//TODO потом усложнить проверку овала
            difX=point1->x()-point2->x();
            difY=point1->y()-point2->y();
            if((abs(difX)<=xSize) && (abs(difY)<=ySize)){
                return true;
            }
            return false;
        }
        case(APP_RECT):{
            difX=point1->x()-point2->x();
            difY=point1->y()-point2->y();
            if((abs(difX)<=xSize/2) && (abs(difY)<=ySize/2)){
                return true;
            }
            return false;
        }
        case(APP_UNDEFINED):{
            return false;
            break;
        }
        default:{
            return false;
        }
    }
}
///////////////////////////////////////////////////////////////////////////////////////////////
bool apperture::readData(QString string){
    bool ok=false;
    int ind1=0;

    switch(type){
        case(APP_CYCLE):{
            ind1=string.indexOf("*");
            xSize=string.left(ind1-1).toFloat(&ok);
            if(!ok){
                return false;
            }
            break;
        }
        case(APP_OVAL):{
            return readXY(string);
        }
        case(APP_RECT):{
            return readXY(string);
        }
        case(APP_UNDEFINED):{
            return false;
        }
    }
    return true;
}
//////////////////////////////////////////////////////////////////////
appertureType apperture::getAppType(QString symbol){
    if(symbol.size()!=1){
        return APP_UNDEFINED;
    }
    if(symbol=="O"){
        return APP_OVAL;
    }
    else if(symbol=="R"){
        return APP_RECT;
    }
    else if(symbol=="C"){
        return APP_CYCLE;
    }
    else{
        return APP_UNDEFINED;
    }
}
//////////////////////////////////////////////////////////////////////////////
bool apperture::readXY(QString string){
    bool ok=false;
    int ind1=0;
    int ind2=0;

    ind1=string.indexOf("X");
    xSize=string.left(ind1-1).toFloat(&ok);
    if(!ok){
        return false;
    }
    ind2=string.indexOf("*",ind1+1);
    ySize=string.mid(ind1+1,ind2-ind1-1).toFloat(&ok);
    if(!ok){
        return false;
    }
    return true;
}
