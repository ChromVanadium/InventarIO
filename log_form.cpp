#include "log_form.h"
#include "ui_log_form.h"

CVLogForm::CVLogForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CVLogForm)
{
    ui->setupUi(this);
}

CVLogForm::~CVLogForm()
{
    delete ui;
}

void CVLogForm::addText(QString text)
{
    if(ui->ckTimeStamp->isChecked())
        ui->edLog->append(QDateTime::currentDateTime().toString("yyyy.MM.dd hh:mm.ss.zzz: ") + text);
    else
        ui->edLog->append(text);
}

void CVLogForm::on_btClear_clicked()
{
    ui->edLog->clear();
}
