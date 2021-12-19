#include "gcodeeditwidget.h"

gCodeEditWidget::gCodeEditWidget(QWidget *parent):
    QTextEdit(parent)
{
    currentString=0;
    connect(this,SIGNAL(cursorPositionChanged()),this,SLOT(cursorPosChangedSlot()));
}
//////////////////////////////////////////////////////////////////////////////////////////
void gCodeEditWidget::setCurrentString(int number){
    QTextCursor cursor=this->textCursor();
    QTextBlock block;
    int startPos=0;
    int endPos=0;
    QTextCharFormat fmt;
    if(number>=0){
        block=this->document()->findBlockByNumber(currentString);
        startPos=block.position();
        endPos=startPos+block.length()-1;
        fmt.setBackground(Qt::white);
        cursor.setPosition(startPos, QTextCursor::MoveAnchor);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);

        block=this->document()->findBlockByNumber(number);
        startPos=block.position();
        endPos=startPos+block.length()-1;
        fmt.setBackground(Qt::yellow);
        cursor.setPosition(startPos, QTextCursor::MoveAnchor);
        cursor.setPosition(endPos, QTextCursor::KeepAnchor);
        cursor.setCharFormat(fmt);
        currentString=number;
    }
}
///////////////////////////////////////////////////////////////////////////////////////
bool gCodeEditWidget::parseGCode(){
    parser.reset();
    return parser.readProgramm(this->document()->toPlainText());
}
///////////////////////////////////////////////////////////////////////////////////
QVector<drawCommand> *gCodeEditWidget::getPainterProgramm(){
    return parser.getPainterProgramm();
}
/////////////////////////////////////////////////////////////////////////////////////
void gCodeEditWidget::setPenDiameter(float diameter){
    parser.setPenDiameter(diameter);
}
///////////////////////////////////////////////////////////////////////////////////
void gCodeEditWidget::reset(){
    this->clear();
    parser.reset();
    currentString = 0;
}
//////////////////////////////////////////////////////////////////////////////////
void gCodeEditWidget::textChangedSlot(){
    this->cursor().setPos(0,0);
}
///////////////////////////////////////////////////////////////////////////////////
void gCodeEditWidget::cursorPosChangedSlot(){

    //может быть вообще не стоит проверять синтаксис на лету
    QTextCursor cursor=this->textCursor();
    QTextBlock block=cursor.block();
    int number=block.blockNumber();
    parser.shineDrawCommand(number);
    emit updateViewSignal();
//    QTextCharFormat fmt;
//    int startPos=0;
//    int endPos=0;

//    if(parser.isStringCorrect(block.text())){
//        fmt.setBackground(Qt::white);
//    }
//    else{
//        fmt.setBackground(Qt::red);
//    }
//    startPos=block.position();
//    endPos=startPos+block.length()-1;
//    cursor.setPosition(startPos, QTextCursor::MoveAnchor);
//    cursor.setPosition(endPos, QTextCursor::KeepAnchor);
//    cursor.setCharFormat(fmt);

}
