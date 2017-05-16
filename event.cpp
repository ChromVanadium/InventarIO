#include "event.h"

CVEvent::CVEvent()
{
    f_parent.clear();
    f_text.clear();
    f_dateTime = QDateTime::currentDateTime();
    f_type = 0;
    f_unq = 0;
    f_lastUpdate = QDateTime::currentDateTime().toTime_t();
    f_modified = 1;
    f_sid = 0;

    f_id = QUuid::createUuid().toString();

    hash0 = makeHash();
}

CVEvent::CVEvent(QString _parent, QString _text, int _type)
{
    f_parent = _parent;
    f_text = _text;
    f_sid = 0;
    f_dateTime = QDateTime::currentDateTime();
    f_type = _type;
    f_unq = 0;
    f_lastUpdate = QDateTime::currentDateTime().toTime_t();
    f_modified = 1;

    f_id = QUuid::createUuid().toString();

    hash0 = makeHash();
}

CVEvent::CVEvent(int _unq, QString _id, QString _parent, QString _description, int _type, int _unix_time, int _lastUpdate, int _modified){
    f_id = _id;
    f_sid = 0;
    f_parent = _parent;
    f_text = _description;
    f_dateTime = QDateTime::fromTime_t(_unix_time);
    f_type = _type;
    f_unq = _unq;
    f_lastUpdate = _lastUpdate;
    f_modified = _modified;

    f_id = _id;
    if(f_id.isEmpty()){
        f_id = QUuid::createUuid().toString();
        setModified(true);
        toDB(false);
    }

    hash0 = makeHash();
}

CVEvent::CVEvent(int _unq, QString _id, QString _parent, QString _description, int _type, int _unix_time, int _lastUpdate, int _modified, int _sid)
{
    f_id = _id;
    f_sid = _sid;
    f_parent = _parent;
    f_text = _description;
    f_dateTime = QDateTime::fromTime_t(_unix_time);
    f_type = _type;
    f_unq = _unq;
    f_lastUpdate = _lastUpdate;
    f_modified = _modified;

    f_id = _id;
    if(f_id.isEmpty()){
        f_id = QUuid::createUuid().toString();
        setModified(true);
        toDB(false);
    }

    hash0 = makeHash();
}

QString CVEvent::text(){
    return f_text;
}

void CVEvent::setText(QString _text){
    f_text = _text;
}

QString CVEvent::id(){
    return f_id;
}

void CVEvent::setId(QString _id){
    f_id = _id;
}

int CVEvent::sid()
{
    return f_sid;
}

void CVEvent::setSid(int _sid)
{
    f_sid = _sid;
}

QString CVEvent::parent(){
    return f_parent;
}

void CVEvent::setParent(QString _parent){
    f_parent = _parent;
}

QDateTime CVEvent::dateTime(){
    return f_dateTime;
}

void CVEvent::setDateTime(int _dateTime_unix){
    f_dateTime = QDateTime::fromTime_t(_dateTime_unix);
}

int CVEvent::type(){
    return f_type;
}

void CVEvent::setType(int _type){
    f_type = _type;
}



QJsonObject CVEvent::toJson()
{
    QJsonObject json;

    int dt = f_dateTime.toTime_t();
    json["id"] = f_id;
    json["parent"] = f_parent;
    json["type"] = f_type;
    json["text"] = f_text;
    json["datetime"] = dt;
    json["d"] = f_d;
    json["u"] = f_lastUpdate;

    return json;
}

void CVEvent::setModified(bool isModified)
{
    if(isModified)
        f_modified = 1;
    else
        f_modified = 0;
}

QString CVEvent::makeHash()
{
    QString h = QString("%1_%2_%3_%4_%5_%6")
            .arg(f_id)
            .arg(f_parent)
            .arg(f_type)
            .arg(f_text)
            .arg(f_dateTime.toTime_t())
            .arg(f_d);

    QCryptographicHash md5(QCryptographicHash::Md5);
    QVariant v = h;
    md5.addData( v.toByteArray() );
    QString hash = md5.result().toHex();

    return hash;
}

void CVEvent::toDB(bool force)
{
    QString hash1 = makeHash();
    bool a = hash0.compare(hash1,Qt::CaseInsensitive)==0;

    if(!a || f_modified==1 || force){
        hash0 = hash1;

        if(f_unq>0)
            updateToDB();
        else
            insertToDB();
    }
}

void CVEvent::insertToDB()
{
    QSqlQuery q;
    QString qs;

    if(f_id.isEmpty())
        f_id = QUuid::createUuid().toString();

    qs = QString("INSERT INTO events(id) VALUES('%1')").arg(f_id);
    execSQL(qs);

    qs = QString("SELECT unq FROM events WHERE id LIKE '%1'").arg(f_id);
    execSQL(&q, qs);

    q.next();
    f_unq = q.record().value("unq").toInt();

    updateToDB();
}

void CVEvent::updateToDB()
{
    QSqlQuery q;
    QString qs;

    f_lastUpdate = QDateTime::currentDateTime().toTime_t();

    qs = QString("UPDATE events SET "
                 "description='%2', type=%3, "
                 "parent='%4', unix_time=%5, u=%6, modified=%8, sid=%7 "
                 "WHERE unq=%1")
            .arg(f_unq)
            .arg(f_text)
            .arg(f_type)
            .arg(f_parent)
            .arg(f_dateTime.toTime_t())
            .arg(f_lastUpdate)
            .arg(f_sid)
            .arg(f_modified);
    execSQL(qs);
}
