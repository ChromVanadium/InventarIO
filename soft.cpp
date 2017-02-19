#include "soft.h"

CVSoft::CVSoft()
{

}

QString CVSoft::serialNumber(){
    return value1();
}

void CVSoft::setSerialNumber(QString _serial){
    setValue1(_serial);
}
