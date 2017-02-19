#ifndef CVITEM_H
#define CVITEM_H

#include <QString>
#include <QDebug>
#include "sql.h"
#include <QDateTime>
#include "specs.h"
#include "event.h"

class CVItem: public CVSql
{
public:
    CVItem();
    CVItem(int _id, int _parent, QString _qr, QString _name, QString _description, CVSpecs _type, QString _value1, QString _value2, QString _value3);

    int id();
    void setId(int _id);
    QString name();
    void setName(QString _name);
    QString QR();
    void setQR(QString _qr);
    QString description();
    void setDescription(QString _description);

    CVSpecs type();
    void setType(CVSpecs _type);

    int level();
    void setLevel(int _level);
    QString value1();
    void setValue1(QString _value1);
    QString value2();
    void setValue2(QString _value2);
    QString value3();
    void setValue3(QString _value3);

    void getEvents();
    void setEvents(QList<CVEvent> _events);
    int parent();
    void setParent(int _parent);

    void toDB();
    void markToDelete();

    QList<CVEvent> events;

private:
    CVSpecs f_type;
    int f_id, f_parent;
    int f_level;
    QString f_value1, f_value2, f_value3;
    QString f_name, f_description;
    QString f_qr;
    int f_d;

    void insertToDB();
    void updateToDB();
};

#endif // CVITEM_H
