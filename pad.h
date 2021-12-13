#ifndef PAD_H
#define PAD_H

#include <QString>
#include <QStringList>
#include "apperture.h"
#include "defines.h"

class pad
{
public:
    pad();

    void setX(float value);
    void setY(float value);
    void setApp(apperture *value);
    QStringList calcGCode(float penDiameter, float force); 
    float getX() const;
    float getY() const;

protected:
    float X;//позиция пада на плате
    float Y;
    apperture *app;



};

#endif // PAD_H
