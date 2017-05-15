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
    CVItem(int _unq, QString _id, QString _parent, int _level, QString _qr, QString _name, QString _description, QString _type,
           QString _value1, QString _value2, QString _value3, int _lastUpdate, int _modified);
    CVItem(int _unq, QString _id, QString _parent, int _level, QString _qr, QString _name, QString _description, QString _type,
           QString _value1, QString _value2, QString _value3, int _lastUpdate, int _modified, int _sid);

    int unq();
    void setUnq(int _unq);

    QString id();
    void setId(QString _id);
    QString parent();
    void setParent(QString _parent);

    void setSid(int _sid);
    int sid();

    QString name();
    void setName(QString _name);
    QString QR();
    void setQR(QString _qr);
    QString description();
    void setDescription(QString _description);

    QString type();
    void setType(QString _type);



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


    void toDB(bool force);
    void insertToDB();
    void updateToDB();
    void markToDelete();
    void setModified(bool isModified);

    QList<CVEvent> events;
    void addEvent(QString _eventText);
    void addEvent(CVEvent _event);
//    void addChildItem(CVItem _child);
//    CVItem takeChildItem(int _id);

    //QList<CVItem> childItems;

    QJsonObject toJson();

private:
    QString f_type;
    int f_unq, f_sid;
    int f_level;
    QString f_value1, f_value2, f_value3;
    QString f_name, f_description;
    QString f_qr;
    QString f_id, f_parent;
    int f_d;
    int f_lastUpdate;
    int f_modified;

    QString hash0;
    QString makeHash();


};

#endif // CVITEM_H
