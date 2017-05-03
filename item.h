#ifndef CVITEM_H
#define CVITEM_H

#include <QString>
#include <QDebug>
#include "sql.h"
#include <QDateTime>
#include "specs.h"
#include "event.h"
#include <QJsonObject>
#include <QUuid>
#include <QCryptographicHash>

class CVItem: public CVSql
{
public:
    CVItem();
    CVItem(int _id, int _sid, QString _uuid, int _parent, int _level, QString _qr, QString _name, QString _description, CVSpecs _type,
           QString _value1, QString _value2, QString _value3, int _lastUpdate);

    int id();
    void setId(int _id);
    int sid();
    void setSid(int _sid);
    QString name();
    void setName(QString _name);
    QString QR();
    void setQR(QString _qr);
    QString description();
    void setDescription(QString _description);

    CVSpecs type();
    void setType(CVSpecs _type);

    QString uuid();
    void setUuid(QString _uuid);

    int level();
    void setLevel(int _level);
    QString value1();
    void setValue1(QString _value1);
    QString value2();
    void setValue2(QString _value2);
    QString value3();
    void setValue3(QString _value3);

    int lastUpdateUnix();
    void setLastUpdate(QDateTime _dateTime);
    void setLastUpdate(int _unixDateTime);

    void getEvents();
    void setEvents(QList<CVEvent> _events);
    int parent();
    void setParent(int _parent);

    void toDB();
    void insertToDB();
    void updateToDB();
    void markToDelete();

    QList<CVEvent> events;
    void addEvent(QString _eventText);
    void addEvent(CVEvent _event);
//    void addChildItem(CVItem _child);
//    CVItem takeChildItem(int _id);

    //QList<CVItem> childItems;

    QJsonObject toJson();

private:
    CVSpecs f_type;
    int f_id, f_sid, f_parent;
    int f_level;
    QString f_value1, f_value2, f_value3;
    QString f_name, f_description;
    QString f_qr;
    QString f_uuid;
    int f_d;
    int f_lastUpdate;

    QString hash0;
    QString makeHash();


};

#endif // CVITEM_H
