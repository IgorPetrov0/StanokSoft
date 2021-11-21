#include "consoletextwidget.h"

consoleTextWidget::consoleTextWidget(QWidget *parent):
    QPlainTextEdit(parent)
{
    maxSize=100;
}
//////////////////////////////////////////////////////////////////////////////
void consoleTextWidget::addString(QString string){
    if(list.size()>=maxSize){//храним не более определенного количества строк
        list.removeFirst();
    }
    list.append(string);
    updateContent();
}
//////////////////////////////////////////////////////////////////////////////
void consoleTextWidget::updateContent(){
    this->clear();
    QString text;
    int size=list.size();
    clear();
    for(int n=0;n!=size;n++){
        text+=list.at(n)+"\n";
    }
    this->setPlainText(text);
    this->verticalScrollBar()->setValue(this->verticalScrollBar()->maximum());
}
//////////////////////////////////////////////////////////////////////////////
