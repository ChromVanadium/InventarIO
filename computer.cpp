#include "computer.h"

CVComputer::CVComputer()
{

}

QString CVComputer::ip(){
    return value1();
}

void CVComputer::setIp(QString _ip){
    setValue1(_ip);
}

QString CVComputer::user(){
    return value2();
}

void CVComputer::setUser(QString _user){
    setValue2(_user);
}

QString CVComputer::configuration()
{
    return value3();
}

void CVComputer::setConfiguration(QString _config)
{
    setValue3(_config);
}
