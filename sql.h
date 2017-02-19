#ifndef SQL_H
#define SQL_H

#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QVariant>
#include <QTableWidget>

class CVSql
{
public:
    CVSql();

    bool execSQL(QSqlQuery *queryContainer, QString queryText);
    bool execSQL(QString queryText);
    QSqlQuery execSQLq(QString queryText);

    QString sanitize(QString text);

    QString rowHash(QTableWidget *tw, int row, int fromColumn, int toColumn);


};

#endif // CVSQL_H


