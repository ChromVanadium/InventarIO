#ifndef CVSOFT_H
#define CVSOFT_H

#include "item.h"

class CVSoft: public CVItem
{
public:
    CVSoft();

    QString serialNumber();
    void setSerialNumber(QString _serial);

private:


};

#endif // CVSOFT_H
