#ifndef GCODEEDITWIDGET_H
#define GCODEEDITWIDGET_H

#include <QTextEdit>
#include <QTextBlock>
#include "parsergcode.h"

class gCodeEditWidget : public QTextEdit
{
    Q_OBJECT
public:
    gCodeEditWidget(QWidget *parent);
    void setCurrentString(int number);
    bool parseGCode();
    QVector<drawCommand> *getPainterProgramm();
    void setPenDiameter(float diameter);
    void reset();

protected:
    int currentString;
    parserGCode parser;

protected slots:
    void cursorPosChangedSlot();
    void textChangedSlot();


signals:
    void updateViewSignal();


};

#endif // GCODEEDITWIDGET_H
