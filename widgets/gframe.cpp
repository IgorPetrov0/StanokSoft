#include "gframe.h"

GFrame::GFrame(QWidget *parent):
    QFrame(parent)
{
    setMouseTracking(true);
}
////////////////////////////////////////////////////////////////////////
void GFrame::mousePressEvent(QMouseEvent *event)
{
    setLineWidth(3);
}

