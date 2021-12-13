#include "gerberconverter.h"

gerberConverter::gerberConverter(QWidget *parent):
    stanokObject(parent)
{
    stopFlag=false;
    progress = new progressDialor(parent);
    progress->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    progress->setModal(true);
    apperturesArray=nullptr;
    padsArray=nullptr;
    pathsArray=nullptr;
    connect(progress,SIGNAL(cancelSignal()),this,SLOT(processStopSlot()));
    toolDiameter=-1;
    Zoffset=0;
    zOmmitt=ZERO_NO_OMMITT;
    currentPosInGProg=0;
}
///////////////////////////////////////////////////////////////////////////////////////
gerberConverter::~gerberConverter(){
    delete progress;
    allClear();
}
////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::convertGerberCode(QFile *file){

    if(!readFile(file)){
        return false;
    }

    int size=gerberCode.size();

    //определяем принадлежность к KiCad
    for(int n=0;n!=size;n++){
        QString str=gerberCode.at(n);
        int index=str.indexOf("TF.");
        if(index!=-1){
            if(str.mid(index,35)=="TF.GenerationSoftware,KiCad,Pcbnew,"){
                KiCadVersion=str.mid(index+35);//и сохраняем версию
                KiCadVersion.remove(KiCadVersion.size()-2,2);//удаляем символы комментария в конце строки
                return parseAsKiCad();
            }
        }
    }
    QMessageBox box;
    box.setStandardButtons((QMessageBox::Yes)|QMessageBox::No);
    box.setText(tr("Файл ")+file->fileName()+tr(" не является Gerber файлом KiCad. Попробовать его прочитать \n"
                                                 " как Gerber файл?"));
    box.setWindowTitle(tr("Ошибка чтения ")+file->fileName());
    box.setIcon(QMessageBox::Question);
    if(box.exec()==QMessageBox::Yes){
        return parseAsSomethingElse();
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::readFile(QFile *file){
    allClear();//очищаем перед чтением файла

    progress->setWindowTitle(tr("Чтение файла ")+file->fileName());
    progress->show();
    QTextStream in(file);
    int bytesPerPercent=file->size()/100;
    while((!in.atEnd())&&(!stopFlag)){
        gerberCode.append(in.readLine());
        progress->setValue(in.pos()/bytesPerPercent);
        QCoreApplication::processEvents();
    }
    if(gerberCode.isEmpty()){
        lastError=tr("Ошибка чтения файла ")+file->fileName();
        progress->close();
        return false;
    }
    if(stopFlag){//если остановлено пользователем
        gerberCode.clear();
    }
    progress->close();
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::parseAsKiCad(){
    if(!parseGerberFormat()){
        return false;
    }
    if(!readAppertures()){
        return false;
    }
    if(!makePadsArray()){
        return false;
    }
    if(!makePathsArray()){
        return false;
    }
    initGProgram();
    connectPaths();//оптимизация путей из набора коротких в длинные
    convertCoordinates();//пересчитываем все координаты в систему координат станка
    if(!createPadsGCode()){//рассчитываем площадки и заполняем программу
        return false;
    }
    if(!createPathsGCode()){//рассчитываем дорожки и заполняем программу
        return false;
    }

    return true;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::parseAsSomethingElse(){

}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::parseGerberFormat(){
    int count=gerberCode.size();
    int n=0;

    for(;n!=count;n++){
        QString string=gerberCode.at(n);
        if(string.left(3)=="G04"){
            if(string.left(15)=="G04 Gerber Fmt "){
                bool ok=false;
                int tmp1=string.indexOf(".",14);
                QString tmpStr=string.mid(15,tmp1-15);
                pointPos=tmpStr.toInt(&ok);
                if(!ok){
                    lastError=tr("Невозможно получить позицию десятичной точки. Строка ")+QString::number(n);
                    return false;
                }
                int tmp2=string.indexOf(",",tmp1);
                tmpStr=string.mid(tmp1+1,tmp2-tmp1-1);
                decDigits=tmpStr.toInt(&ok);
                if(!ok){
                    lastError=tr("Невозможно получить количество знаков после запятой. Строка ")+QString::number(n);
                    return false;
                }
                tmpStr=string.mid(tmp2+2,20);
                if(tmpStr=="Leading zero omitted"){
                    zOmmitt=ZERO_LEADING;
                }
                else{
                    lastError=tr("Невозможно получить информацию о подавлении нулей. Строка ")+QString::number(n);
                    return false;
                }

                tmp1=string.indexOf(",",tmp2+1);
                tmpStr=string.mid(tmp1+2,21);
                if(tmpStr!="Abs format (unit mm)*"){
                    lastError=tr("Невозможно получить информацию о единицах измерения. Строка ")+QString::number(n);
                    return false;
                }
                break;
            }
        }
    }
    if(n==count){
        lastError=tr("Невозможно получить данные о формате Gerber");
        return false;
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::readAppertures(){
    int count=gerberCode.size();
    int n=0;
    deleteAppertures();
    apperturesArray = new QVector<apperture*>;
    progress->setWindowTitle(tr("Чтение аппертур"));
    progress->show();
    float stringsInPercent=count/100;

    for(;n!=count;n++){
        QString string=gerberCode.at(n);
        if(string.left(4)=="%ADD"){
            apperture *tmpApp = new apperture;
            if(!tmpApp->readApperture(string.remove(0,4))){
                lastError=tr("Ошибка чтения аппертуры. Строка ")+QString::number(n);
                delete tmpApp;
                return false;
            }
            apperturesArray->append(tmpApp);
        }
        if(string.left(3)=="G04"){
            if(string.left(22)=="G04 APERTURE END LIST*"){
                break;
            }
        }
        progress->setValue(n/stringsInPercent);
        QCoreApplication::processEvents();
    }
    progress->close();

    if(n==count){
        lastError="Данные аппертур в файле не найдены.";
        deleteAppertures();
        return false;
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::deleteAppertures(){
    if(apperturesArray!=nullptr){
        int size=apperturesArray->size();
        for(int n=0;n!=size;n++){
            delete apperturesArray->at(n);
        }
        delete apperturesArray;
        apperturesArray=nullptr;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::deletePads(){
    if(padsArray!=nullptr){
        int size=padsArray->size();
        for(int n=0;n!=size;n++){
            delete padsArray->at(n);
        }
        delete padsArray;
        padsArray=nullptr;
    }
}
//////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::deletePaths(){
    if(pathsArray!=nullptr){
        int size=pathsArray->size();
        for(int n=0;n!=size;n++){
            delete pathsArray->at(n);
        }
        delete pathsArray;
        pathsArray=nullptr;
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::makePadsArray(){
    int n=0;
    int size=gerberCode.size();
    int currApperture=0;
    bool ok=false;
    int tmp1=0;
    int tmp2=0;
    float tmpFloat=0;
    QString tmpStr;

    padsArray = new QVector<pad*>;

    for(;n!=size;n++){
        QString string=gerberCode.at(n);
        if(string.left(1)=="D"){
            tmp1=string.indexOf("*");
            tmpStr=string.mid(1,tmp1-1);
            tmp1=tmpStr.toInt(&ok);
            if(!ok){
                lastError="Значение аппертуры не читается. Строка "+QString::number(n);
                return false;
            }
            if(tmp1>9){
                currApperture=tmp1;
            }
        }
        if(string.left(1)=="X"){
            tmpStr=string.mid(string.size()-4,3);
            if(tmpStr=="D03"){//значит нашли пад
                pad *tmpPad = new pad;
                tmpPad->setApp(findApperture(currApperture));
                if(!readXCoordinate(string,&tmpFloat)){
                    lastError==tr("Ошибка чтения координаты X. Строка ")+QString::number(n);
                    delete tmpPad;
                    return false;
                }
                tmpPad->setX(tmpFloat);
                if(!readYCoordinate(string,&tmpFloat)){
                    lastError==tr("Ошибка чтения координаты Y. Строка ")+QString::number(n);
                    delete tmpPad;
                    return false;
                }
                tmpPad->setY(tmpFloat);
                padsArray->append(tmpPad);
            }
        }
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::makePathsArray(){
    pathsArray = new QVector<GPath*>;

    int n=0;
    int size=gerberCode.size();
    int currApperture=0;
    apperture *pcApperture=nullptr;
    bool ok=false;
    int tmp1=0;
    float fTmp=0;
    QString tmpStr;
    QPointF *tmpPoint = nullptr;
    GPath *tmpPath = new GPath;//создаем путь

    for(;n!=size;n++){
        QString string=gerberCode.at(n);
        if(string.left(1)=="D"){//так сделано для того, чтобы в памяти сохранялась последняя аппертура по ходу чтения файла
            tmp1=string.indexOf("*");
            tmpStr=string.mid(1,tmp1-1);
            tmp1=tmpStr.toInt(&ok);
            if(!ok){
                lastError="Значение аппертуры не читается. Строка "+QString::number(n);
                delete tmpPath;
                delete pathsArray;
                pathsArray=nullptr;
                return false;
            }
            if(tmp1>9){//если это номер аппертуры
                currApperture=tmp1;
                pcApperture=findApperture(currApperture);
                //todo тут что-то не так. потом проверить на предмет оптимизации
                if(tmpPath->isApperture()){//если у пути уже прописана аппертура( аппертура прописывается при чтении D02)
                    pathsArray->append(tmpPath);//путь в массив и
                    tmpPath = new GPath;//создаем новый путь
                }
            }
        }
        tmpStr=string.mid(string.size()-4,3);
        if(tmpStr=="D02"){//значит нашли перемещение без засветки
            if(string.left(1)=="X"){
                tmpPoint = new QPointF;
                if(!readXCoordinate(string,&fTmp)){//читаем координату Х
                    lastError==tr("Ошибка чтения координаты X. Строка ")+QString::number(n);
                    delete tmpPoint;
                    delete tmpPath;
                    delete pathsArray;
                    pathsArray=nullptr;
                    return false;
                }
                tmpPoint->setX(fTmp);

                if(!readYCoordinate(string,&fTmp)){//читаем координату Y
                    lastError==tr("Ошибка чтения координаты Y. Строка ")+QString::number(n);
                    delete tmpPoint;
                    delete tmpPath;
                    delete pathsArray;
                    pathsArray=nullptr;
                    return false;
                }
                tmpPoint->setY(fTmp);

                tmpPath->setApp(pcApperture);

                //перемещение без засветки это или начало пути или продолжение
                //если его координаты в пределах аппертуры предыдущей координаты, то это продолжение
                //иначе начало
                QPointF *tmpPoint2 = tmpPath->endtPoint();
                if(tmpPath->getPointsCount()!=0){
                    if(pcApperture->isInApperture(tmpPoint,tmpPoint2)){//если в аппертуре
                        if(*tmpPoint != *tmpPoint2){//и если точки не равны
                            tmpPath->addPoint(tmpPoint);//то добавляем
                        }
                    }
                    else{//если не в аппертуре, то закрываем этот путь и начинаем следующий
                        pathsArray->append(tmpPath);
                        tmpPath = new GPath;
                        tmpPath->setApp(pcApperture);
                        tmpPath->addPoint(tmpPoint);//и добавляем в него точку
                    }
                }
                else{
                    tmpPath->addPoint(tmpPoint);//если в пути нет точек, то добавляем в любом случае
                }
            }
        }
        else if(tmpStr=="D01"){//значит нашли перемещение с засветкой
            tmpPoint = new QPointF;
            if(!readXCoordinate(string,&fTmp)){//читаем координату Х
                lastError==tr("Ошибка чтения координаты X. Строка ")+QString::number(n);
                delete tmpPoint;
                delete tmpPath;
                delete pathsArray;
                pathsArray=nullptr;
                return false;
            }
            tmpPoint->setX(fTmp);

            if(!readYCoordinate(string,&fTmp)){//читаем координату Y
                lastError==tr("Ошибка чтения координаты Y. Строка ")+QString::number(n);
                delete tmpPoint;
                delete tmpPath;
                delete pathsArray;
                pathsArray=nullptr;
                return false;
            }
            tmpPoint->setY(fTmp);
            tmpPath->addPoint(tmpPoint);//добавляем точку в любом случае
        }
    }
    if(tmpPath->isValide()){
        pathsArray->append(tmpPath);//последний путь в массив, если он валидный
    }
    else{
        delete tmpPath;
    }
    //последний путь может быть не валидным, тогда его просто отстреливаем

    if(pathsArray->isEmpty()){//если других путей нет, то это не ошибка. Их может и не быть
        delete pathsArray;
        pathsArray=nullptr;
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::allClear(){
    deleteAppertures();
    deletePads();
    deletePaths();
    gerberCode.clear();
}
////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::readYCoordinate(QString string, float *rez){
    int tmp1=0;
    int tmp2=0;
    QString tmpStr;
    bool ok;

    tmp1=string.indexOf("Y");//читаем координату
    tmp2=string.indexOf("D");
    tmpStr=string.mid(tmp1+1,tmp2-1-tmp1);
    switch(zOmmitt){
        case(ZERO_LEADING):{
            tmpStr.insert(tmpStr.size()-decDigits,".");
            break;
        }
        default:{
            return false;//TODO дописать остальные варианты подавления нулей
        }
    }
    *rez=tmpStr.toFloat(&ok);
    if(!ok){
        return false;
    }
    return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::readXCoordinate(QString string, float *rez){
    int tmp1=0;
    int tmp2=0;
    QString tmpStr;
    bool ok;

    tmp1=string.indexOf("X");//читаем координату
    tmp2=string.indexOf("Y");
    tmpStr=string.mid(tmp1+1,tmp2-1);
    switch(zOmmitt){
        case(ZERO_LEADING):{
            tmpStr.insert(tmpStr.size()-decDigits,".");
            break;
        }
        default:{
            return false;//TODO дописать остальные варианты подавления нулей
        }
    }
    *rez=tmpStr.toFloat(&ok);
    if(!ok){
        return false;
    }
    return true;
}
/////////////////////////////////////////////////////////////////////////////////////////////////
apperture *gerberConverter::findApperture(int number){
    int size=apperturesArray->size();
    for(int n=0;n!=size;n++){
        if(apperturesArray->at(n)->getNumber()==number){
            return apperturesArray->at(n);
        }
    }
    return nullptr;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::initGProgram(){
    gProgramm.append("G01 Z0 F2\n");
    gProgramm.append("G01 X0 Y0\n");
    currentPosInGProg=2;
    gProgramm.append("G01 Z0\n");
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::addGCommand(QString command){
    gProgramm.insert(currentPosInGProg, command);
    currentPosInGProg++;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::createPadsGCode(){

    if(apperturesArray!=nullptr){
        if(toolDiameter<=0){
            lastError=tr("Диаметр инструмента не задан. Рассчет невозможен.");
            return false;
        }
        if(averageSpeed<=0){
            lastError=tr("Скорость подачи не задана. Расчет невозможен.");
            return false;
        }
        QStringList subProgr;
        int size=padsArray->size();

        for(int n=0;n!=size;n++){
            subProgr=padsArray->at(n)->calcGCode(toolDiameter,averageSpeed);
            if(subProgr.isEmpty()){
                lastError=tr("Ошибка при формировании G-кода площадки.");
                return false;
            }
            int subProgSize=subProgr.size();
            for(int m=0;m!=subProgSize;m++){
                addGCommand(subProgr.at(m));
            }
        }
        return true;
    }
    lastError=tr("Массив аппертур не определен.");
    return false;
}
///////////////////////////////////////////////////////////////////////////////////////////////////
bool gerberConverter::createPathsGCode(){
    if(pathsArray!=nullptr){
        if(toolDiameter<=0){
            lastError=tr("Диаметр инструмента не задан. Рассчет невозможен.");
            return false;
        }
        if(averageSpeed<=0){
            lastError=tr("Скорость подачи не задана. Расчет невозможен.");
        }
        QStringList subProgr;
        int size=pathsArray->size();

        for(int n=0;n!=size;n++){
            subProgr=pathsArray->at(n)->calcGCode(toolDiameter,averageSpeed);
            if(subProgr.isEmpty()){
                lastError=tr("Ошибка при формировании G-кода дорожки.");
                return false;
            }
            int subProgSize=subProgr.size();
            for(int m=0;m!=subProgSize;m++){
                addGCommand(subProgr.at(m));
            }
        }
    }
    return true;//массив путей может быть и не определен. это не ошибка
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::connectPaths(){
    if(pathsArray==nullptr){
        return;
    }
    //процедура оптимизации. находит и соединяет короткие пути в более длинные
    for(int m=0;m!=pathsArray->size();m++){
        for(int n=1;n!=pathsArray->size();){//проходим по всем путям
            if(m!=n){//не проверяем один и тот же путь
                //ищем путь, начало которого совпадает с концом текущего
                GPath *currentPath=pathsArray->operator [](m);
                GPath *nextPath=pathsArray->operator [](n);
                float appSize=0;
                if(currentPath->appertureSize()==nextPath->appertureSize()){
                    appSize=abs(nextPath->appertureSize()/2);
                    QPointF diff = *currentPath->endtPoint() - *nextPath->startPoint();
                    if((abs(diff.x())<=appSize) && (abs(diff.y())<=appSize)){//если начало следующего пути в зоне текущего
                        currentPath->addPath(nextPath);//то добавляем путь к текущему
                        pathsArray->remove(n);//удаляем путь
                        m=0;//начинаем все с начала т.к. следующий путь встает на место удаленного
                        n=0;
                    }
                    else{
                        n++;//если пути не в зоне друг друга - переходим к следующему
                    }
                }
                else{
                    n++;
                }
            }
            else{
                n++;
            }
        }
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::convertCoordinates(){
    //пересчитываем координаты из системы координат редактора в систему координат станка.
    //за 0 принимаем координаты первой площадки в списке

    float coordX = padsArray->at(0)->getX();
    float coordY = padsArray->at(0)->getY();

    foreach(pad *nextPad, *padsArray){
        nextPad->setX(nextPad->getX() - coordX);
        nextPad->setY(nextPad->getY() - coordY);
    }

    foreach(GPath *nextPath, *pathsArray){
        nextPath->convertCoordinates(coordX, coordY);
    }

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
QVector<pad *> *gerberConverter::getPadsArray() const{
    return padsArray;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
QVector<GPath *> *gerberConverter::getPathsArray() const{
    return pathsArray;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::setPenDiameter(float diameter){
    toolDiameter=diameter;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::setForce(float force){
    averageSpeed=force;
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::setZOffset(float zOffset){
    Zoffset=zOffset;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
QString gerberConverter::getGCode(){
    QString tmpStr;
    int size=gProgramm.size();
    for(int n=0;n!=size;n++){
        tmpStr+=gProgramm.at(n);
    }
    return tmpStr;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
QStringList *gerberConverter::getProgramm(){
    return &gProgramm;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////
void gerberConverter::processStopSlot(){
    stopFlag=true;
}
