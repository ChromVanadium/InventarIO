#include "algorythm.h"

CVAlgorythm::CVAlgorythm()
{

}

QString CVAlgorythm::dateToHuman(QDateTime _dateTime)
{
    QString res = _dateTime.toString("dd.MM.yyyy hh:mm");

    QDateTime dt = QDateTime::currentDateTime();

    if(_dateTime.date() == dt.date())
        res = QString("сегодня в %1").arg( _dateTime.toString("hh:mm") );

    if(_dateTime.daysTo(dt)==1)
        res = QString("вчера в %1").arg( _dateTime.toString("hh:mm") );

    return res;
}
