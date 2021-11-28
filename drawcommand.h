#ifndef DRAWCOMMAND_H
#define DRAWCOMMAND_H

#include <QPainter>
#include <math.h>


enum commandType{
    COMMAND_NO_TYPE,
    COMMAND_LINE,
    COMMAND_ARC_FCW,
    COMMAND_ARC_RCW,
    COMMAND_MOVE
};


class drawCommand
{
public:
    drawCommand();

    commandType getType() const;
    void setType(const commandType &value);
    void setCurrentX(float value);
    void setCurrentY(float value);
    void setCurrentZ(float value);
    void setNumber(int value);
    void setMoveX(float value);
    void setMoveY(float value);
    void setMoveZ(float value);
    void setForce(float value);
    int getNumber() const;
    float getForce() const;
    void setI(float value);
    void setJ(float value);
    void setK(float value);
    void setR(float value);
    void draw(QPainter *painter);
    void setPenDiameter(float value);
    void setShine(bool s);

protected:
    commandType type;
    int currentX;
    int currentY;
    int currentZ;
    int moveX;
    int moveY;
    int moveZ;
    int force;
    float I;
    float J;
    float K;
    float R;
    int penDiameter;
    float multiplier;
    int number;
    bool isMX;
    bool isMY;
    bool isMZ;
    bool isNumber;
    bool isI;
    bool isJ;
    bool isK;
    bool isR;
    bool shine;

    QRect calcArcRect();
    int calcAngle(float X, float Y, float centerX, float centerY);
    void checkCommand();//проверяем явно указанные параметры

};

#endif // DRAWCOMMAND_H
