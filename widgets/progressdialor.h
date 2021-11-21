#ifndef PROGRESSDIALOR_H
#define PROGRESSDIALOR_H

#include <QDialog>

namespace Ui {
class progressDialor;
}

class progressDialor : public QDialog
{
    Q_OBJECT

public:
    explicit progressDialor(QWidget *parent);
    ~progressDialor();
    void setValue(int value);


protected slots:
    void cancelSlot();


signals:
    void cancelSignal();


private:
    Ui::progressDialor *ui;
};

#endif // PROGRESSDIALOR_H
