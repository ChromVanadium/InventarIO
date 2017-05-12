#ifndef CVEVENT_H
#define CVEVENT_H

#include <QDateTime>
#include <QString>
#include <QDebug>
#include <QJsonObject>
#include <QUuid>
#include <QCryptographicHash>

#include "sql.h"
#include "algorythm.h"

class CVEvent: public CVSql, CVAlgorythm
{
public:
    CVEvent();
    CVEvent(int _itemId, QString _text);
    CVEvent(int _itemId, QString _text, int _type);
    CVEvent(int _unq, int _itemId, QString _description, int _type, int _unix_time);
    CVEvent(int _unq, int _sid, QString _uuid, int _itemId, QString _description, int _type, int _unix_time, int _lastUpdate);

    QString text();
    void setText(QString _text);
    int id();
    void setId(int _id);
    int itemId();
    void setItemId(int _itemId);
    QDateTime dateTime();
    void setDateTime(int _dateTime_unix);
    int type();
    void setType(int _type);

    void toDB();
    QJsonObject toJson();
    void setModified(bool isModified);

private:
    QString f_text;
    QDateTime f_dateTime;
    int f_id, f_sid, f_itemId, f_type, f_d;
    int f_lastUpdate;
    QString f_uuid;
    int f_modified;

    QString hash0;
    QString makeHash();

    void insertToDB();
    void updateToDB();

};

#endif // CVEVENT_H
