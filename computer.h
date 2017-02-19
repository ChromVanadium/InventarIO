#ifndef CVCOMPUTER_H
#define CVCOMPUTER_H

#include "item.h"

class CVComputer: public CVItem
{
public:
    CVComputer();

    QString ip();
    void setIp(QString _ip);
    QString user();
    void setUser(QString _user);
    QString configuration();
    void setConfiguration(QString _config);


};

#endif // CVCOMPUTER_H
