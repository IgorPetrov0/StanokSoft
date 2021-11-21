#include "pad.h"

pad::pad()
{

}
////////////////////////////////////////////////////////////
void pad::setX(float value){
    X = value;
}
///////////////////////////////////////////////////////////////////
void pad::setY(float value){
    Y = value;
}
///////////////////////////////////////////////////////////////////
void pad::setApp(apperture *value){
    app = value;
}
////////////////////////////////////////////////////////////////////
QStringList pad::calcGCode(float penDiameter, float force){
    QStringList tmpProg;
    switch(app->getType()){
        case(APP_CYCLE):{
            int lines=(app->getXSize()/2)/(penDiameter/2);//количество целых линий
            float lastLineOffset=(app->getXSize()/2)-(penDiameter/2*lines);//смещение последней линии
            //например, если размер Pad-а 2.2мм, а диаметр пера 1мм, то смещение последней линии будет 0.1мм
            tmpProg.append("G00 X"+QString::number(X)+" Y"+QString::number(Y));//смещаем инструмент в позицию
            tmpProg.append("G01 Z0 F"+QString::number(force));//опускаем в позицию Z0 делаем точку
            int n=1;
            float centerY=0;
            for(;n!=lines+1;n++){//рисуем концентрические окружности до заполнения PAD-а
                centerY=-n*(penDiameter/2);
                tmpProg.append("G01 Y"+QString::number(Y-centerY)+" F"+QString::number(force));//смещаем по Y на величину диаметра
                tmpProg.append("G02 X"+QString::number(X)+" Y"+QString::number(Y-centerY)+" I0 J"+QString::number(centerY)+" F2");
            }
            if(lastLineOffset!=0){
                tmpProg.append("G01 Y"+QString::number(Y-centerY+lastLineOffset)+" F"+QString::number(force));//смещаем по Y на величину диаметра
                tmpProg.append("G02 X"+QString::number(X)+" Y"+QString::number(Y-centerY+lastLineOffset)+" I0 J"+QString::number(centerY-lastLineOffset)+" F2");
            }
        }
        case(APP_OVAL):{
            int lines=0;
            float lastLineOffset=0;
            float center1=0;//смещения центров дуг верхней/нижней или правой/левой
            float center2=0;
            float centerOffset=0;
            float penRadius=penDiameter/2;
            //количество линий считаем по наименьшему размеру
            if(app->getXSize()>app->getYSize()){//горизонтальный овал
                lines=(app->getYSize()/2)/(penDiameter/2);//количество целых линий
                lastLineOffset=(app->getYSize()/2)-(penDiameter/2*lines);//смещение последней линии
                //например, если размер Pad-а 2.2мм, а диаметр пера 1мм, то смещение последней линии будет 0.1мм
                float halfSize=app->getXSize()/2;
                float step=0;
                centerOffset=halfSize-((lines*penRadius)+lastLineOffset);
                center1=X+centerOffset;
                center2=X-centerOffset;
                tmpProg.append("G00 X"+QString::number(center1)+" Y"+QString::number(Y));//смещаем инструмент в позицию
                tmpProg.append("G01 Z0 F"+QString::number(force));//опускаем в позицию Z0 делаем точку
                tmpProg.append("G01 X"+QString::number(center2)+QString::number(force));//рисуем центральную линию
                for(float n=1;n!=lines+1;n++){//рисуем овалы по количеству линий
                    step=penRadius*n;
                    tmpProg.append("G01 Y"+QString::number(Y+step)+" F"+QString::number(force));//смещение на радиус пера по Y
                    tmpProg.append("G03 Y"+QString::number(Y-step)+" J"+QString::number(-step)+" F"+QString::number(force));//дуга слева
                    tmpProg.append("G01 X"+QString::number(center1)+" F"+QString::number(force));//верхняя линия
                    tmpProg.append("G02 Y"+QString::number(Y+step)+" J"+QString::number(step)+" F"+QString::number(force));//дуга справа
                    tmpProg.append("G01 X"+QString::number(center2)+" F"+QString::number(force));//нижняя линия
                }
                if(lastLineOffset!=0){
                    tmpProg.append("G01 Y"+QString::number(Y+step+lastLineOffset)+" F"+QString::number(force));//смещение на радиус пера по Y
                    tmpProg.append("G03 Y"+QString::number(Y-step-lastLineOffset)+" J"+QString::number(-step-lastLineOffset)+" F"+QString::number(force));//дуга слева
                    tmpProg.append("G01 X"+QString::number(center1)+" F"+QString::number(force));//верхняя линия
                    tmpProg.append("G02 Y"+QString::number(Y+step+lastLineOffset)+" J"+QString::number(step+lastLineOffset)+" F"+QString::number(force));//дуга справа
                    tmpProg.append("G01 X"+QString::number(center2)+" F"+QString::number(force));//нижняя линия
                }
            }
            else{//вертикальный овал
                lines=(app->getXSize()/2)/(penDiameter/2);//количество целых линий
                lastLineOffset=(app->getXSize()/2)-(penDiameter/2*lines);//смещение последней линии
                //например, если размер Pad-а 2.2мм, а диаметр пера 1мм, то смещение последней линии будет 0.1мм
                float halfSize=app->getYSize()/2;
                float step=0;
                centerOffset=halfSize-((lines*penRadius)+lastLineOffset);
                center1=Y+centerOffset;
                center2=Y-centerOffset;
                tmpProg.append("G00 X"+QString::number(X)+" Y"+QString::number(center1));//смещаем инструмент в позицию
                tmpProg.append("G01 Z0 F"+QString::number(force));//опускаем в позицию Z0 делаем точку
                tmpProg.append("G01 Y"+QString::number(center2)+" F"+QString::number(force));//рисуем центральную линию
                for(float n=1;n!=lines+1;n++){//рисуем овалы по количеству линий
                    step=penRadius*n;
                    tmpProg.append("G01 X"+QString::number(X+step)+" F"+QString::number(force));//смещение на радиус пера по X
                    tmpProg.append("G03 X"+QString::number(X-step)+" I"+QString::number(-step)+" F"+QString::number(force));//дуга сверху
                    tmpProg.append("G01 Y"+QString::number(center1)+" F"+QString::number(force));//верхняя линия
                    tmpProg.append("G02 X"+QString::number(X+step)+" I"+QString::number(step)+" F"+QString::number(force));//дуга снизу
                    tmpProg.append("G01 Y"+QString::number(center2)+" F"+QString::number(force));//нижняя линия
                }
                if(lastLineOffset!=0){
                    tmpProg.append("G01 X"+QString::number(X+step+lastLineOffset)+" F"+QString::number(force));//смещение на радиус пера по X
                    tmpProg.append("G03 X"+QString::number(X-step-lastLineOffset)+" I"+QString::number(-step-lastLineOffset)+" F"+QString::number(force));//дуга слева
                    tmpProg.append("G01 Y"+QString::number(center1)+" F"+QString::number(force));//верхняя линия
                    tmpProg.append("G02 X"+QString::number(X+step+lastLineOffset)+" I"+QString::number(step+lastLineOffset)+" F"+QString::number(force));//дуга справа
                    tmpProg.append("G01 Y"+QString::number(center2)+" F"+QString::number(force));//нижняя линия
                }
            }
            break;
        }
        case(APP_RECT):{
            int lines=0;
            float lastLineOffset=0;
            float center1=0;//смещения центров дуг верхней/нижней или правой/левой
            float center2=0;
            float centerOffset=0;
            float penRadius=penDiameter/2;

            if(app->getXSize()>=app->getYSize()){//горизонтальный прямоугольник
                lines=(app->getYSize()/2)/(penDiameter/2);//количество целых линий
                lastLineOffset=(app->getYSize()/2)-(penDiameter/2*lines);//смещение последней линии
                //например, если размер Pad-а 2.2мм, а диаметр пера 1мм, то смещение последней линии будет 0.1мм
                float halfSize=app->getXSize()/2;
                float step=0;
                centerOffset=halfSize-((lines*penRadius)+lastLineOffset);
                center1=X+centerOffset;
                center2=X-centerOffset;
                tmpProg.append("G00 X"+QString::number(center1)+" Y"+QString::number(Y));//смещаем инструмент в позицию
                tmpProg.append("G01 Z0 F"+QString::number(force));//опускаем в позицию Z0 делаем точку
                tmpProg.append("G01 X"+QString::number(center2)+QString::number(force));//рисуем центральную линию
                for(float n=1;n!=lines+1;n++){//рисуем по количеству линий
                    step=penRadius*n;
                    tmpProg.append("G01 Y"+QString::number(Y+step)+" F"+QString::number(force));//смещение на радиус пера по Y
                    tmpProg.append("G01 X"+QString::number(center1+step)+" F"+QString::number(force));//нижняя линия
                    tmpProg.append("G01 Y"+QString::number(Y-step)+" F"+QString::number(force));//вертикальная линия справа
                    tmpProg.append("G01 X"+QString::number(center2-step)+" F"+QString::number(force));//верхняя линия
                }
                tmpProg.append("G01 Y"+QString::number(Y+step)+" F"+QString::number(force));//смещение на радиус пера по Y
                if(lastLineOffset!=0){
                    tmpProg.append("G01 Y"+QString::number(Y+step+lastLineOffset)+" F"+QString::number(force));//смещение на радиус пера по Y
                    tmpProg.append("G01 X"+QString::number(center1+step)+" F"+QString::number(force));//верхняя линия
                    tmpProg.append("G01 Y"+QString::number(Y-step-lastLineOffset)+" F"+QString::number(force));//дуга справа
                    tmpProg.append("G01 X"+QString::number(center2-step)+" F"+QString::number(force));//нижняя линия
                    tmpProg.append("G01 Y"+QString::number(Y+step)+" F"+QString::number(force));//смещение на радиус пера по Y
                }
            }
            else{//вертикальный прямоугольник
                lines=(app->getXSize()/2)/(penDiameter/2);//количество целых линий
                lastLineOffset=(app->getXSize()/2)-(penDiameter/2*lines);//смещение последней линии
                //например, если размер Pad-а 2.2мм, а диаметр пера 1мм, то смещение последней линии будет 0.1мм
                float halfSize=app->getYSize()/2;
                float step=0;
                centerOffset=halfSize-((lines*penRadius)+lastLineOffset);
                center1=Y+centerOffset;
                center2=Y-centerOffset;
                tmpProg.append("G00 X"+QString::number(X)+" Y"+QString::number(center1));//смещаем инструмент в позицию
                tmpProg.append("G01 Z0 F"+QString::number(force));//опускаем в позицию Z0 делаем точку
                tmpProg.append("G01 Y"+QString::number(center2)+" F"+QString::number(force));//рисуем центральную линию
                for(float n=1;n!=lines+1;n++){//рисуем овалы по количеству линий
                    step=penRadius*n;
                    tmpProg.append("G01 X"+QString::number(X+step)+" F"+QString::number(force));//смещение на радиус пера по X
                    tmpProg.append("G01 Y"+QString::number(center1+step)+" F"+QString::number(force));//верхняя линия
                    tmpProg.append("G01 X"+QString::number(X-step)+" F"+QString::number(force));//дуга снизу
                    tmpProg.append("G01 Y"+QString::number(center2-step)+" F"+QString::number(force));//нижняя линия
                }
                tmpProg.append("G01 X"+QString::number(X+step)+" F"+QString::number(force));//смещение на радиус пера по Y
                if(lastLineOffset!=0){
                    tmpProg.append("G01 X"+QString::number(X+step+lastLineOffset)+" F"+QString::number(force));//смещение на радиус пера по X
                    tmpProg.append("G01 Y"+QString::number(center1+step)+" F"+QString::number(force));//верхняя линия
                    tmpProg.append("G01 X"+QString::number(X-step-lastLineOffset)+" F"+QString::number(force));//дуга справа
                    tmpProg.append("G01 Y"+QString::number(center2-step)+" F"+QString::number(force));//нижняя линия
                    tmpProg.append("G01 X"+QString::number(X+step)+" F"+QString::number(force));//смещение на радиус пера по Y
                }
            }
            break;
        }
        case(APP_UNDEFINED):{
            break;
        }
    }
    return tmpProg;
}
