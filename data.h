#ifndef CVDATA_H
#define CVDATA_H

#include "sql.h"
#include "specs.h"
#include <QUuid>

class CVData : public CVSql
{
public:
    CVData();

    void getFromDB();

    QString uuid();
    void setUuid(QString _uuid);

    QList<CVSpecs> types;
    QString type(int index);

private:
    QString f_uuid;

    void createUuid();

};

#endif // CVDATA_H
