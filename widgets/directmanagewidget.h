#ifndef DIRECTMANAGEWIDGET_H
#define DIRECTMANAGEWIDGET_H

#include <QWidget>
#include "usbconnection.h"
#include "widgets/gpushbutton.h"

namespace Ui {
class directManageWidget;
}

class directManageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit directManageWidget(QWidget *parent = nullptr);
    ~directManageWidget();
    void setPlotterStatus(plotterStatus status);
    void setSwitchStatus(bool X,bool Y,bool Z);
    void setWorkRect(QRectF rect);

protected:
    bool error;
    bool checkG01FromGLine();
    bool checkG0203FromGLine();
    bool checkXYZFromGLine();
    bool checkParameterFromGLine(QString parameter);

signals:
    void sendGCode(QString gCode);
    void sendProgram(QStringList *program);
    void errorSignal(QString error);

protected slots:
    void moveZeroSlot();
    void setZeroSlot();
    void diableSteppersSlot();
    void sendGCodeSlot();
    void directMoveButtonSlot(GPushButton *pointer);
    void rectMoveButtonSlot(GPushButton *pointer);



private:
    Ui::directManageWidget *ui;
};

#endif // DIRECTMANAGEWIDGET_H
