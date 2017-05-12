#include "event.h"

CVEvent::CVEvent()
{
    f_text = "";
    f_dateTime = QDateTime::currentDateTime();
    f_id = 0;
    f_sid = 0;
    f_itemId = 0;
    f_type = 0; // 0 - авто, 1 - вручную (добавлено через диалог)
    f_lastUpdate = QDateTime::currentDateTime().toTime_t();
    f_uuid = QUuid::createUuid().toString();

    hash0 = makeHash();
}

CVEvent::CVEvent(int _itemId, QString _text)
{
    f_id = 0;
    f_sid = 0;
    f_itemId = _itemId;
    f_text = _text;
    f_dateTime = QDateTime::currentDateTime();
    f_type = 0;
    f_lastUpdate = QDateTime::currentDateTime().toTime_t();
    f_uuid = QUuid::createUuid().toString();

    hash0 = makeHash();
}

CVEvent::CVEvent(int _itemId, QString _text, int _type)
{
    f_itemId = _itemId;
    f_sid = 0;
    f_text = _text;
    f_dateTime = QDateTime::currentDateTime();
    f_type = _type;
    f_id = 0;
    f_lastUpdate = QDateTime::currentDateTime().toTime_t();
    f_uuid = QUuid::createUuid().toString();

    hash0 = makeHash();
}

CVEvent::CVEvent(int _unq, int _itemId, QString _description, int _type, int _unix_time){
    f_itemId = _itemId;
    f_text = _description;
    f_dateTime = QDateTime::fromTime_t(_unix_time);
    f_type = _type;
    f_id = _unq;
    f_sid = 0;
    f_lastUpdate = QDateTime::currentDateTime().toTime_t();
    f_uuid = QUuid::createUuid().toString();

    hash0 = makeHash();
}

CVEvent::CVEvent(int _unq, int _sid, QString _uuid, int _itemId, QString _description, int _type, int _unix_time, int _lastUpdate)
{
    f_itemId = _itemId;
    f_sid = _sid;
    f_text = _description;
    f_dateTime = QDateTime::fromTime_t(_unix_time);
    f_type = _type;
    f_id = _unq;
    f_lastUpdate = _lastUpdate;
    f_uuid = _uuid;
    if(f_uuid.isEmpty()){
        f_uuid = QUuid::createUuid().toString();
        setModified(true);
        toDB();
    }

    hash0 = makeHash();
}

QString CVEvent::text(){
    return f_text;
}

void CVEvent::setText(QString _text){
    f_text = _text;
}

int CVEvent::id(){
    return f_id;
}

void CVEvent::setId(int _id){
    f_id = _id;
}

int CVEvent::itemId(){
    return f_itemId;
}

void CVEvent::setItemId(int _itemId){
    f_itemId = _itemId;
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

void CVEvent::toDB()
{
    QString hash1 = makeHash();
    bool a = hash0.compare(hash1,Qt::CaseInsensitive)==0;

    if(!a || f_modified==1){
        hash0 = hash1;

        if(f_id>0)
            updateToDB();
        else
            insertToDB();
    }
}

QJsonObject CVEvent::toJson()
{
    QJsonObject json;

    int dt = f_dateTime.toTime_t();
    json["id"] = f_id;
    json["sid"] = f_sid;
    json["itemid"] = f_itemId;
    json["type"] = f_type;
    json["text"] = f_text;
    json["datetime"] = dt;
    json["d"] = f_d;
    json["u"] = f_lastUpdate;
    json["uuid"] = f_uuid;

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
    QString h = QString("%1_%2_%3_%4_%5_%6_%7_%8_%9_%10_%11_%12")
            .arg(f_id)
            .arg(f_sid)
            .arg(f_itemId)
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

void CVEvent::insertToDB()
{
    QSqlQuery q;
    QString qs;

    QString bredpitt = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");

    qs = QString("INSERT INTO events(description) VALUES('%1')").arg(bredpitt);
    execSQL(qs);

    qs = QString("SELECT id FROM events WHERE description LIKE '%1'").arg(bredpitt);
    execSQL(&q, qs);

    q.next();
    f_id = q.record().value("id").toInt();

    updateToDB();
}

void CVEvent::updateToDB()
{
    QSqlQuery q;
    QString qs;

    f_lastUpdate = QDateTime::currentDateTime().toTime_t();

    qs = QString("UPDATE events SET "
                 "description='%2', type=%3, "
                 "itemid=%4, unix_time=%5, u=%6, uuid='%7', modified=1 "
                 "WHERE id=%1")
            .arg(f_id)
            .arg(f_text)
            .arg(f_type)
            .arg(f_itemId)
            .arg(f_dateTime.toTime_t())
            .arg(f_lastUpdate)
            .arg(f_uuid);
    execSQL(qs);
}
