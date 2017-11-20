#ifndef CVDATA_H
#define CVDATA_H

#include "sql.h"
#include "specs.h"
#include <QUuid>
#include <QMap>
#include <QDateTime>

class CVData : public CVSql
{
public:
    CVData();

    void getFromDB();

    QString uuid();
    QString serverUrl();
    void setUuid(QString _uuid);

    QList<CVSpecs> types;
    QMap<QString, QString> types2;
    QString type(int index);

    QDateTime lastServerTime();
    int timeDelta();

private:
    QString f_uuid, f_serverUrl;

    QDateTime f_lastServerTime;
    int f_timeDelta;

    void createUuid();

};

#endif // CVDATA_H
