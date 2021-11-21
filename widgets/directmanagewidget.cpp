#include "directmanagewidget.h"
#include "ui_directmanagewidget.h"

directManageWidget::directManageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::directManageWidget)
{
    ui->setupUi(this);
    connect(ui->zeroPosButton,SIGNAL(clicked(bool)),this,SLOT(moveZeroSlot()));
    connect(ui->setPosButton,SIGNAL(clicked(bool)),this,SLOT(moveLineSlot()));
    connect(ui->setPosButton_4,SIGNAL(clicked(bool)),this,SLOT(fastMoveSlot()));
    connect(ui->setPosButton_2,SIGNAL(clicked(bool)),this,SLOT(cwRotateSlot()));
    connect(ui->setPosButton_3,SIGNAL(clicked(bool)),this,SLOT(ccwRotateSlot()));
    connect(ui->zeroButton,SIGNAL(clicked(bool)),this,SLOT(setZeroSlot()));
    connect(ui->disSteppersButton,SIGNAL(clicked(bool)),this,SLOT(diableSteppersSlot()));
}
///////////////////////////////////////////////////////////////////////////
directManageWidget::~directManageWidget(){
    delete ui;
}
/////////////////////////////////////////////////////////////////////////////
void directManageWidget::setPlotterStatus(plotterStatus status){
    ui->posXLable->setText(QString::number(status.posX));
    ui->posYLable->setText(QString::number(status.posY));
    ui->posZLable->setText(QString::number(status.posZ));
}
/////////////////////////////////////////////////////////////////////////////
void directManageWidget::moveZeroSlot(){
    QString code="G00 X0 Y0 Z0 F"+
            QString::number(ui->speedSpinBox->value())+
            "\n";
    emit sendGCode(code);
}
/////////////////////////////////////////////////////////////////////////////
void directManageWidget::setZeroSlot(){
    QString code="G92";
    emit sendGCode(code);
}
///////////////////////////////////////////////////////////////////////////////////
void directManageWidget::diableSteppersSlot(){
    QString code="M18";
    emit sendGCode(code);
}
///////////////////////////////////////////////////////////////////////////////////
void directManageWidget::cwRotateSlot(){
    QString code="G02 X"+
            QString::number(ui->XposSpinBox_2->value())+
            " Y"+
            QString::number(ui->YposSpinBox_2->value())+
            " Z"+
            QString::number(ui->ZposSpinBox_2->value())+
            " R"+
            QString::number(ui->RposSpinBox_2->value())+
            " F"+
            QString::number(ui->speedSpinBox->value())+
            "\n";
    emit sendGCode(code);
}
//////////////////////////////////////////////////////////////////////////////////
void directManageWidget::ccwRotateSlot(){
    QString code="G03 X"+
            QString::number(ui->XposSpinBox_3->value())+
            " Y"+
            QString::number(ui->YposSpinBox_3->value())+
            " Z"+
            QString::number(ui->ZposSpinBox_3->value())+
            " R"+
            QString::number(ui->YposSpinBox_3->value())+
            " F"+
            QString::number(ui->speedSpinBox->value())+
            "\n";
    emit sendGCode(code);
}
///////////////////////////////////////////////////////////////////////////////
void directManageWidget::fastMoveSlot(){
    QString code="G00 X"+
            QString::number(ui->XposSpinBox_4->value())+
            " Y"+
            QString::number(ui->YposSpinBox_4->value())+
            " Z"+
            QString::number(ui->ZposSpinBox_4->value())+
            " F"+
            QString::number(ui->speedSpinBox->value())+
            "\n";
    emit sendGCode(code);
}
/////////////////////////////////////////////////////////////////////////////
void directManageWidget::moveLineSlot(){
    QString code="G01 X"+
            QString::number(ui->XposSpinBox->value())+
            " Y"+
            QString::number(ui->YposSpinBox->value())+
            " Z"+
            QString::number(ui->ZposSpinBox->value())+
            " F"+
            QString::number(ui->speedSpinBox->value())+
            "\n";
    emit sendGCode(code);
}
