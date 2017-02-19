#include "item.h"

CVItem::CVItem()
{
    f_parent = 0;
    f_id = 0;
    f_name = "";
    f_description = "";
    f_qr = "";
    f_d = 0;
    f_level = 0;
    f_value1 = "";
    f_value2 = "";
    f_value3 = "";
}

CVItem::CVItem(int _id, int _parent, QString _qr, QString _name, QString _description, CVSpecs _type, QString _value1, QString _value2, QString _value3)
{
    f_id = _id;
    f_qr = _qr;
    f_parent = _parent;
    f_name = _name;
    f_description = _description;
    f_type = _type;
    f_value1 = _value1;
    f_value2 = _value2;
    f_value3 = _value3;
    f_d = 0;
}

int CVItem::id(){
    return f_id;
}

void CVItem::setId(int _id){
    f_id = _id;
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

int CVItem::level(){
    return f_level;
}

void CVItem::setLevel(int _level){
    f_level =_level;
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
    if(f_id>0)
        updateToDB();
    else
        insertToDB();
}

void CVItem::markToDelete()
{
    f_d = 1-f_d;
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

    qs = QString("UPDATE items SET "
                 "name='%2', description='%3', "
                 "type=%4, "
                 "value1='%5', value2='%6', value3='%7', d=%8, qr='%9', parent=%10 "
                 "WHERE id=%1")
            .arg(f_id)
            .arg(f_name)
            .arg(f_description)
            .arg(f_type.index)
            .arg(f_value1)
            .arg(f_value2)
            .arg(f_value3)
            .arg(f_d)
            .arg(f_qr)
            .arg(f_parent);
    execSQL(qs);
}

