#include "data.h"

CVData::CVData()
{

}

void CVData::getFromDB()
{
    QSqlQuery q;
    QString qs;

    qs = QString("SELECT id, description FROM types ORDER BY description ");
    execSQL(&q,qs);

    types.clear();
    types.append( CVSpecs("",0) );

    while(q.next()){
        types.append( CVSpecs(q.record().value("description").toString(),
                              q.record().value("id").toInt()) );
    }

    qs = QString("SELECT uuid FROM localsets LIMIT 1 ");
    execSQL(&q,qs);
    q.next();
    f_uuid = q.record().value("uuid").toString();

    if(f_uuid.isEmpty())
        createUuid();
}

QString CVData::uuid()
{
    return f_uuid;
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

void CVData::createUuid()
{
    f_uuid = QUuid::createUuid().toString();

    execSQL( QString("INSERT INTO localsets(uuid) VALUES('%1')").arg(f_uuid) );
}
