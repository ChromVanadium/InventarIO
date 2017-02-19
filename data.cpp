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
}

QString CVData::type(int index)
{
    QString res = "";
    for(int i=0;i<types.count();i++)
        if(types[i].index==index)
            res = types[i].name;
    return res;
}
