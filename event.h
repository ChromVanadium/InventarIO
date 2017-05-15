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
    CVEvent(QString _parent,  QString _text, int _type=0);
    CVEvent(int _unq, QString _id, QString _parent, QString _description, int _type, int _unix_time, int _lastUpdate, int _modified);
    CVEvent(int _unq, QString _id, QString _parent, QString _description, int _type, int _unix_time, int _lastUpdate, int _modified, int _sid);

    QString text();
    void setText(QString _text);
    QString id();
    void setId(QString _id);

    int sid();
    void setSid(int _sid);

    QString parent();
    void setParent(QString _parent);
    QDateTime dateTime();
    void setDateTime(int _dateTime_unix);
    int type();
    void setType(int _type);

    void toDB(bool force);
    QJsonObject toJson();
    void setModified(bool isModified);

private:
    QString f_text;
    QDateTime f_dateTime;
    QString f_id, f_parent;
    int f_unq, f_type, f_d, f_sid;
    int f_lastUpdate;
    int f_modified;

    QString hash0;
    QString makeHash();

    void insertToDB();
    void updateToDB();

};

#endif // CVEVENT_H
