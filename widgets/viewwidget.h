#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QPen>
#include <QWheelEvent>
#include <QVector>
#include "gerberconverter.h"
#include "drawcommand.h"

namespace Ui {
class viewWidget;
}

class viewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit viewWidget(QWidget *parent = nullptr);
    void drawRawData(gerberConverter *converter);
    void drawProgramm(QVector<drawCommand> *programm);
    void clearAll();
    ~viewWidget();

protected:
    void paintEvent(QPaintEvent *);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);

    gerberConverter *converterPointer;

    float scaleX;
    float scaleY;
    float translateX;
    float translateY;
    int mousePosX;
    int mousePosY;
    QVector<drawCommand> *graphProgramm;

private:
    Ui::viewWidget *ui;
};

#endif // VIEWWIDGET_H
