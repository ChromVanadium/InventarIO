#ifndef CVSPECS_H
#define CVSPECS_H

#include <QString>

class CVSpecs
{
public:
    CVSpecs();
    CVSpecs(QString _name, int _index);

    int index;
    QString name;
};

#endif // CVSPECS_H
