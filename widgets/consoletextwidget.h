#ifndef CONSOLETEXTWIDGET_H
#define CONSOLETEXTWIDGET_H

#include <QPlainTextEdit>
#include <QScrollBar>

class consoleTextWidget : public QPlainTextEdit
{
public:
    consoleTextWidget(QWidget *parent);
    void addString(QString string);
    void updateContent();

protected:
    QStringList list;
    int maxSize;
};

#endif // CONSOLETEXTWIDGET_H
