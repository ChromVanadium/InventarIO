#include "sql.h"

//#include <QSql>
#include <QDebug>

CVSql::CVSql()
{
}

bool CVSql::execSQL(QSqlQuery *queryContainer, QString queryText){
    bool res = true;
    queryContainer->exec(queryText);

    if(queryContainer->lastError().number() != -1){
/* leave */ qDebug() << queryText;
/* leave */ qDebug() << queryContainer->lastError().number();
/* leave */ qDebug() << queryContainer->lastError().text();
       //errorSQL(queryContainer);
       res = false;
    };

    return res;
}

bool CVSql::execSQL(QString queryText){
    bool res = true;
    QSqlQuery q;
    q.exec(queryText);

    if(q.lastError().number() != -1){
/* leave */ qDebug() << queryText;
/* leave */ qDebug() << q.lastError().number();
/* leave */ qDebug() << q.lastError().text();
      // errorSQL(&q);
       res = false;
    };
    return res;
}

QSqlQuery CVSql::execSQLq(QString queryText)
{
    QSqlQuery q;
    q.exec(queryText);

    if(q.lastError().number() != -1){
/* leave */ qDebug() << queryText;
/* leave */ qDebug() << q.lastError().number();
/* leave */ qDebug() << q.lastError().text();
      // errorSQL(&q);
    };
    return q;
}

QString CVSql::sanitize(QString text)
{
    QString res;

    res = text.remove("'");

    return res;
}

QString CVSql::rowHash(QTableWidget *tw, int row, int fromColumn, int toColumn)
{
    int f = 0, t = tw->columnCount()-1;

    if(fromColumn > f && fromColumn < tw->columnCount()) f = fromColumn;
    if(toColumn < tw->columnCount() && toColumn > f) t = toColumn;

//qDebug() << "from" << f << ", to" << t;

    QString hashString = "";

    for(int i=f;i<=t;i++)
        hashString.append(tw->item(row,i)->text() + "_");
//qDebug() << hashString;
    return hashString;
}
