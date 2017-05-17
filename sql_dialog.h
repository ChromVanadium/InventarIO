#ifndef SQL_DIALOG_H
#define SQL_DIALOG_H

#include <QDialog>
#include <QDebug>
#include "sql.h"

namespace Ui {
class CVSqlDialog;
}

class CVSqlDialog : public QDialog, CVSql
{
    Q_OBJECT

public:
    explicit CVSqlDialog(QWidget *parent = 0);
    ~CVSqlDialog();

private slots:
    void on_btExecute_clicked();

private:
    Ui::CVSqlDialog *ui;
};

#endif // SQL_DIALOG_H
