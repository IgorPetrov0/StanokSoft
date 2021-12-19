#ifndef GERBERCONVERTER_H
#define GERBERCONVERTER_H

#include <QFile>
#include <QString>
#include <QStringList>
#include <QTextStream>
#include <QCoreApplication>
#include <QMessageBox>
#include <QVector>
#include <QRectF>
#include <cmath>
#include "stanokobject.h"
#include "widgets/progressdialor.h"
#include "apperture.h"
#include "pad.h"
#include "gpath.h"


enum zeroOmmitt{
    ZERO_LEADING,
    ZERO_FOLLOW,
    ZERO_NO_OMMITT
};




class gerberConverter : public stanokObject
{
    Q_OBJECT

public:
    gerberConverter(QWidget *parent);
    ~gerberConverter();
    bool convertGerberCode(QFile *file);
    QVector<pad *> *getPadsArray() const;//TODO потом закрыть эту возможность
    QVector<GPath *> *getPathsArray() const;//TODO потом закрыть эту возможность
    void setPenDiameter(float diameter);
    void setForce(float force);
    void setZOffset(float zOffset);
    void setMoveSpeed(float speed);
    QString getGCode();
    QStringList *getProgramm();
    void allClear();
    QRectF getWorkRect();

protected:
    QString KiCadVersion;
    QStringList gerberCode;//сырой gepber код
    bool stopFlag;
    progressDialor *progress;
    zeroOmmitt zOmmitt;
    int pointPos;
    int decDigits;
    float toolDiameter;
    float Zoffset;
    float averageSpeed;
    float moveSpeed;
    QVector<apperture*> *apperturesArray;
    QVector<pad*> *padsArray;
    QVector<GPath*> *pathsArray;
    QStringList gProgramm;//готовая программа в G-кодах
    int currentPosInGProg;
    QRectF workRect;


    bool readFile(QFile *file);
    bool parseAsKiCad();
    bool parseAsSomethingElse();
    bool parseGerberFormat();
    bool readAppertures();
    void deleteAppertures();
    void deletePads();
    void deletePaths();
    bool makePadsArray();
    bool makePathsArray();
    bool readXCoordinate(QString string, float *rez);
    bool readYCoordinate(QString string, float *rez);
    apperture *findApperture(int number);
    void initGProgram();
    void addGCommand(QString command);
    bool createPadsGCode();
    bool createPathsGCode();
    void connectPaths();
    void convertCoordinates();
    void findWorkRect();

protected slots:
    void processStopSlot();

signals:
    void messageSignal(QString message);




};

#endif // GERBERCONVERTER_H
