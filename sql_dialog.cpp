#include "sql_dialog.h"
#include "ui_sql_dialog.h"

CVSqlDialog::CVSqlDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CVSqlDialog)
{
    ui->setupUi(this);
}

CVSqlDialog::~CVSqlDialog()
{
    delete ui;
}

void CVSqlDialog::on_btExecute_clicked()
{
    QSqlQuery q;
    QString qs;

    QString text = ui->edText->toPlainText();
    QStringList sqls = text.split("\n",QString::SkipEmptyParts);

    for(int i=0;i<sqls.count();i++)
        execSQL(sqls[i]);
    //qDebug() << sqls;
}
