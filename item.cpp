#include "item.h"

CVItem::CVItem()
{
    f_parent = "";
    f_id = QUuid::createUuid().toString();
    f_sid = 0;
    f_name = "";
    f_description = "";
    f_qr = "";
    f_d = 0;
    f_level = 0;
    f_value1 = "";
    f_value2 = "";
    f_value3 = "";
    f_modified = 1;
    f_unq = 0;

    events.clear();

    hash0 = makeHash();
}

CVItem::CVItem(int _unq, QString _id, QString _parent, int _level, QString _qr, QString _name, QString _description, QString _type, QString _value1, QString _value2, QString _value3, int _lastUpdate, int _modified)
{
    f_unq = _unq;
    f_qr = _qr;
    f_sid = 0;
    f_parent = _parent;
    f_name = _name;
    f_description = _description;
    f_type = _type;
    f_value1 = _value1;
    f_value2 = _value2;
    f_value3 = _value3;
    f_d = 0;
    f_level = _level;
    f_lastUpdate = _lastUpdate;
    f_id = _id;
    events.clear();

    hash0 = makeHash();
    f_modified = _modified;
}

CVItem::CVItem(int _unq, QString _id, QString _parent, int _level, QString _qr, QString _name, QString _description, QString _type, QString _value1, QString _value2, QString _value3, int _lastUpdate, int _modified, int _sid)
{
    f_unq = _unq;
    f_qr = _qr;
    f_sid = _sid;
    f_parent = _parent;
    f_name = _name;
    f_description = _description;
    f_type = _type;
    f_value1 = _value1;
    f_value2 = _value2;
    f_value3 = _value3;
    f_d = 0;
    f_level = _level;
    f_lastUpdate = _lastUpdate;
    f_id = _id;
    events.clear();

    hash0 = makeHash();
    f_modified = _modified;
}

int CVItem::unq(){
    return f_unq;
}

void CVItem::setUnq(int _unq){
    f_unq = _unq;
}

QString CVItem::name(){
    return f_name;
}

void CVItem::setName(QString _name){
    f_name = _name;
}

QString CVItem::QR()
{
    return f_qr;
}

void CVItem::setQR(QString _qr)
{
    f_qr = _qr;
}

QString CVItem::description(){
    return f_description;
}

void CVItem::setDescription(QString _description){
    f_description = _description;
}

QString CVItem::type(){
    return f_type;
}

void CVItem::setType(QString _type){
    f_type = _type;
}

QString CVItem::id()
{
    return f_id;
}

void CVItem::setId(QString _id)
{
    f_id = _id;
}

int CVItem::level(){
    return f_level;
}

void CVItem::setLevel(int _level){
    f_level =_level;
//    for(int i=0;i<childItems.count();i++)
//        childItems[i].setLevel(f_level+1);
}

QString CVItem::value1(){
    return f_value1;
}

void CVItem::setValue1(QString _value1){
    f_value1 = _value1;
}

QString CVItem::value2(){
    return f_value2;
}

void CVItem::setValue2(QString _value2){
    f_value2 = _value2;
}

QString CVItem::value3(){
    return f_value3;
}

void CVItem::setValue3(QString _value3){
    f_value3 = _value3;
}

int CVItem::lastUpdateUnix()
{
    return f_lastUpdate;
}

void CVItem::setLastUpdate(QDateTime _dateTime)
{
    f_lastUpdate = _dateTime.toTime_t();
}

void CVItem::setLastUpdate(int _unixDateTime)
{
    f_lastUpdate = _unixDateTime;
}

void CVItem::getEvents()
{
    // TODO: удалить?
}

QString CVItem::parent()
{
    return f_parent;
}

void CVItem::setParent(QString _parent)
{
    f_parent = _parent;
}

void CVItem::setSid(int _sid)
{
    f_sid = _sid;
}

int CVItem::sid()
{
    return f_sid;
}



void CVItem::setModified(bool isModified)
{
    if(isModified)
        f_modified = 1;
    else
        f_modified = 0;
}

void CVItem::addEvent(QString _eventText)
{
    CVEvent e = CVEvent(f_id, _eventText, 0);
    e.toDB(true);
    events.append(e);
}

void CVItem::addEvent(CVEvent _event)
{
     events.append(_event);
}

QJsonObject CVItem::toJson()
{
    QJsonObject json;

    json["id"] = f_id;
    json["name"] = f_name;
    json["description"] = f_description;
    json["type"] = f_type;
    json["value1"] = f_value1;
    json["value2"] = f_value2;
    json["value3"] = f_value3;
    json["d"] = f_d;
    json["qr"] = f_qr;
    json["parent"] = f_parent;
    json["level"] = f_level;
    json["u"] = f_lastUpdate;

    return json;
}

QString CVItem::makeHash()
{
    QString h = QString("%1_%2_%3_%4_%5_%6_%7_%8_%9_%10_%11")
            .arg(f_id)
            .arg(f_name)
            .arg(f_description)
            .arg(f_type)
            .arg(f_value1)
            .arg(f_value2)
            .arg(f_value3)
            .arg(f_d)
            .arg(f_qr)
            .arg(f_parent)
            .arg(f_level);

    QCryptographicHash md5(QCryptographicHash::Md5);
    QVariant v = h;
    md5.addData( v.toByteArray() );
    QString hash = md5.result().toHex();

    return hash;
}

void CVItem::toDB(bool force)
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

void CVItem::markToDelete()
{
    f_d = 1-f_d;
}

void CVItem::insertToDB()
{
    QSqlQuery q;
    QString qs;

    if(f_id.isEmpty())
        f_id = QUuid::createUuid().toString();

    qs = QString("INSERT INTO items(id) VALUES('%1')").arg(f_id);
    execSQL(qs);

    qs = QString("SELECT unq FROM items WHERE id LIKE '%1'").arg(f_id);
    execSQL(&q, qs);

    q.next();
    f_unq = q.record().value("unq").toInt();

    updateToDB();
}

void CVItem::updateToDB()
{
    QSqlQuery q;
    QString qs;

    f_lastUpdate = QDateTime::currentDateTime().toTime_t();

    if(f_id.isEmpty())
        f_id = QUuid::createUuid().toString();

    qs = QString("UPDATE items SET "
                 "name='%2', description='%3', "
                 "type='%4', "
                 "value1='%5', value2='%6', value3='%7', d=%8, qr='%9', parent='%10', lvl=%11, u=%12, modified=%14, sid=%13 "
                 "WHERE unq=%1")
            .arg(f_unq)
            .arg(f_name.remove("'"))
            .arg(f_description.remove("'"))
            .arg(f_type)
            .arg(f_value1.remove("'"))
            .arg(f_value2.remove("'"))
            .arg(f_value3.remove("'"))
            .arg(f_d)
            .arg(f_qr.remove("'"))
            .arg(f_parent)
            .arg(f_level)
            .arg(f_lastUpdate)
            .arg(f_sid)
            .arg(f_modified);
    execSQL(qs);
}

