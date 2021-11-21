#ifndef DIRECTMANAGEWIDGET_H
#define DIRECTMANAGEWIDGET_H

#include <QWidget>
#include "usbconnection.h"

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

signals:
    void sendGCode(QString gCode);

protected slots:
    void moveLineSlot();
    void moveZeroSlot();
    void setZeroSlot();
    void diableSteppersSlot();
    void cwRotateSlot();
    void ccwRotateSlot();
    void fastMoveSlot();

private:
    Ui::directManageWidget *ui;
};

#endif // DIRECTMANAGEWIDGET_H
