#ifndef CVDATA_H
#define CVDATA_H

#include "sql.h"
#include "specs.h"
#include <QUuid>
#include <QMap>

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

private:
    QString f_uuid, f_serverUrl;

    void createUuid();

};

#endif // CVDATA_H
