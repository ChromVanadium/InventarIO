#include "data.h"

CVData::CVData()
{

}

void CVData::getFromDB()
{
    QSqlQuery q;
    QString qs;

    qs = QString("SELECT unq, id, description FROM types ORDER BY description ");
    execSQL(&q,qs);

    types.clear();
    types.append( CVSpecs("",0) );

    while(q.next()){
        types.append( CVSpecs(q.record().value("description").toString(),
                              q.record().value("unq").toInt()) );
        types2.insert(q.record().value("id").toString(),q.record().value("description").toString());
    }

    qs = QString("SELECT uuid, serverurl, lastservertime, timedelta FROM localsets LIMIT 1 ");
    execSQL(&q,qs);
    q.next();
    f_uuid = q.record().value("uuid").toString();
    f_serverUrl = q.record().value("serverurl").toString();
    f_lastServerTime = QDateTime::fromTime_t(q.record().value("lastservertime").toInt());
    f_timeDelta = q.record().value("timedelta").toInt();

    if(f_uuid.isEmpty())
        createUuid();
}

QString CVData::uuid()
{
    return f_uuid;
}

QString CVData::serverUrl()
{
    return f_serverUrl;
}

void CVData::setUuid(QString _uuid)
{
    f_uuid = _uuid;
}

QString CVData::type(int index)
{
    QString res = "";
    for(int i=0;i<types.count();i++)
        if(types[i].index==index)
            res = types[i].name;
    return res;
}

QDateTime CVData::lastServerTime()
{
    return f_lastServerTime;
}

int CVData::timeDelta()
{
    return f_timeDelta;
}

void CVData::createUuid()
{
    f_uuid = QUuid::createUuid().toString();

    execSQL( QString("UPDATE localsets SET uuid='%1'").arg(f_uuid) );
}
