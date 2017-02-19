#ifndef CVEVENT_H
#define CVEVENT_H

#include <QDateTime>
#include <QString>

#include "sql.h"

class CVEvent: public CVSql
{
public:
    CVEvent();
    CVEvent(int _itemId, QString _text);
    CVEvent(int _itemId, QString _text, int _type);
    CVEvent(int _unq, int _itemId, QString _description, int _type, int _unix_time);

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

private:
    QString f_text;
    QDateTime f_dateTime;
    int f_id, f_itemId, f_type;

    void insertToDB();
    void updateToDB();

};

#endif // CVEVENT_H
