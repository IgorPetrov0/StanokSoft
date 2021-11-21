#include "progressdialor.h"
#include "ui_progressdialor.h"

progressDialor::progressDialor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::progressDialor)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked(bool)),this,SLOT(cancelSlot()));
}
/////////////////////////////////////////////////////////////////
progressDialor::~progressDialor()
{
    delete ui;
}
/////////////////////////////////////////////////////////////////////
void progressDialor::setValue(int value){
    ui->progressBar->setValue(value);
}
/////////////////////////////////////////////////////////////////////
void progressDialor::cancelSlot(){
    setWindowTitle(tr("Ожидается завершение текущей операции."));
    emit cancelSignal();
}
