#ifndef GFRAME_H
#define GFRAME_H

#include <QFrame>

class GFrame : public QFrame
{
    Q_OBJECT
public:
    GFrame(QWidget *parent);


protected:
    virtual void mousePressEvent(QMouseEvent *event);


};

#endif // GFRAME_H
