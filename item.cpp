#include "item.h"

CVItem::CVItem()
{
    f_parent = 0;
    f_id = 0;
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

    f_uuid = QUuid::createUuid().toString();

    events.clear();

    hash0 = makeHash();
    //childItems.clear();
}

CVItem::CVItem(int _id, int _sid, QString _uuid, int _parent, int _level, QString _qr, QString _name, QString _description, CVSpecs _type, QString _value1, QString _value2, QString _value3, int _lastUpdate)
{
    f_id = _id;
    f_sid = _sid;
    f_qr = _qr;
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
    f_uuid = _uuid;
    events.clear();
    f_modified = 0;
    //childItems.clear();
    hash0 = makeHash();
}

int CVItem::id(){
    return f_id;
}

void CVItem::setId(int _id){
    f_id = _id;
}

int CVItem::sid()
{
    return f_sid;
}

void CVItem::setSid(int _sid)
{
    f_sid = _sid;
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

CVSpecs CVItem::type(){
    return f_type;
}

void CVItem::setType(CVSpecs _type){
    f_type = _type;
}

QString CVItem::uuid()
{
    return f_uuid;
}

void CVItem::setUuid(QString _uuid)
{
    f_uuid = _uuid;
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

}

int CVItem::parent()
{
    return f_parent;
}

void CVItem::setParent(int _parent)
{
    f_parent = _parent;
}

void CVItem::toDB()
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

void CVItem::markToDelete()
{
    f_d = 1-f_d;
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
    CVEvent e = CVEvent(f_id, _eventText);
    e.toDB();
    events.append(e);
}

void CVItem::addEvent(CVEvent _event)
{
     events.append(_event);
}

//void CVItem::addChildItem(CVItem _child)
//{
//    _child.setLevel(f_level+1);
//    _child.setParent(f_id);
//    childItems.append(_child);
//}

//CVItem CVItem::takeChildItem(int _id)
//{
//    CVItem m;
//    for(int i=0;i<childItems.count();i++)
//        if(childItems[i].id()==_id)
//            m = childItems[i];
//    return m;
//}

QJsonObject CVItem::toJson()
{
    QJsonObject json;

    json["id"] = f_id;
    json["sid"] = f_sid;
    json["name"] = f_name;
    json["description"] = f_description;
    json["type"] = f_type.index;
    json["value1"] = f_value1;
    json["value2"] = f_value2;
    json["value3"] = f_value3;
    json["d"] = f_d;
    json["qr"] = f_qr;
    json["parent"] = f_parent;
    json["level"] = f_level;
    json["u"] = f_lastUpdate;
    json["uuid"] = f_uuid;

    return json;
}

QString CVItem::makeHash()
{
    QString h = QString("%1_%2_%3_%4_%5_%6_%7_%8_%9_%10_%11")
            .arg(f_id)
            .arg(f_name)
            .arg(f_description)
            .arg(f_type.index)
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

void CVItem::insertToDB()
{
    QSqlQuery q;
    QString qs;

    QString bredpitt = QDateTime::currentDateTime().toString("yyyyMMddHHmmsszzz");

    qs = QString("INSERT INTO items(value3) VALUES('%1')").arg(bredpitt);
    execSQL(qs);

    qs = QString("SELECT id FROM items WHERE value3 LIKE '%1'").arg(bredpitt);
    execSQL(&q, qs);

    q.next();
    f_id = q.record().value("id").toInt();

    updateToDB();
}

void CVItem::updateToDB()
{
    QSqlQuery q;
    QString qs;

    f_lastUpdate = QDateTime::currentDateTime().toTime_t();

    if(f_uuid.isEmpty())
        f_uuid = QUuid::createUuid().toString();

    qs = QString("UPDATE items SET "
                 "name='%2', description='%3', "
                 "type=%4, "
                 "value1='%5', value2='%6', value3='%7', d=%8, qr='%9', parent=%10, lvl=%11, u=%12, uuid='%13', sid=%14, modified=1 "
                 "WHERE id=%1")
            .arg(f_id)
            .arg(f_name.remove("'"))
            .arg(f_description.remove("'"))
            .arg(f_type.index)
            .arg(f_value1.remove("'"))
            .arg(f_value2.remove("'"))
            .arg(f_value3.remove("'"))
            .arg(f_d)
            .arg(f_qr.remove("'"))
            .arg(f_parent)
            .arg(f_level)
            .arg(f_lastUpdate)
            .arg(f_uuid)
            .arg(f_sid);
    execSQL(qs);
    qDebug() << "updated";
}

