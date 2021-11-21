#ifndef STANOKOBJECT_H
#define STANOKOBJECT_H

#include <QObject>
#include <QWidget>

class stanokObject : public QObject
{

    Q_OBJECT

public:
    stanokObject(QWidget *parent);
    QString getLastError();


protected:
    QString lastError;
    QWidget *parentWidget;


};

#endif // STANOKOBJECT_H
