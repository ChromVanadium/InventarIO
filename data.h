#ifndef CVDATA_H
#define CVDATA_H

#include "sql.h"
#include "specs.h"

class CVData : public CVSql
{
public:
    CVData();

    void getFromDB();

    QList<CVSpecs> types;
    QString type(int index);
};

#endif // CVDATA_H
