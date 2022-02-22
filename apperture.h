#ifndef APPERTURE_H
#define APPERTURE_H

#include <QString>
#include <QPointF>
#include <stdlib.h>

enum appertureType{
    APP_UNDEFINED,
    APP_OVAL,
    APP_RECT,
    APP_CYCLE
};

class apperture
{
public:
    apperture();


    void setType(const appertureType &value);
    void setXSize(float value);
    void setYSize(float value);
    void setNumber(float value);
    appertureType getType();
    float getXSize();
    float getYSize();
    bool readApperture(QString string);
    int getNumber() const;
    bool isInApperture(QPointF *point1, QPointF *point2);




protected:
    appertureType type;
    float xSize;
    float ySize;
    int number;

    bool readData(QString string);
    appertureType getAppType(QString symbol);
    bool readXY(QString string);


};

#endif // APPERTURE_H
