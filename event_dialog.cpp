#include "event_dialog.h"
#include "ui_event_dialog.h"

CVEventDialog::CVEventDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CVEventDialog)
{
    ui->setupUi(this);

    ui->dtDate->setDate(QDateTime::currentDateTime().date());
    ui->dtTime->setTime(QDateTime::currentDateTime().time());
}

CVEventDialog::~CVEventDialog()
{
    delete ui;
}

void CVEventDialog::setEvent(CVEvent _event)
{
    f_event = _event;
    ui->textEdit->setText(f_event.text());
    ui->dtDate->setDate(f_event.dateTime().date());
    ui->dtTime->setTime(f_event.dateTime().time());
}

CVEvent CVEventDialog::event()
{
    return f_event;
}

void CVEventDialog::on_buttonBox_accepted()
{
    f_event.setText(ui->textEdit->toPlainText());
    QDateTime dt;
    dt.setDate( ui->dtDate->date() );
    dt.setTime( ui->dtTime->time() );
    f_event.setDateTime(dt.toTime_t());
    f_event.setType(1);
    accept();
}
