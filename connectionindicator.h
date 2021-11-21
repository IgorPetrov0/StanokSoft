#ifndef CONNECTIONINDICATOR_H
#define CONNECTIONINDICATOR_H

#include <QWidget>
#include <QPainter>
#include <QString>

class connectionIndicator : public QWidget
{
public:
    connectionIndicator(QString name);
    void setState(bool state);

protected:
    void paintEvent(QPaintEvent *event);
    QColor color;
    QString name;


};

#endif // CONNECTIONINDICATOR_H
