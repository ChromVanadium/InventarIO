#ifndef ITEM_DIALOG_H
#define ITEM_DIALOG_H

#include <QDialog>
#include "item.h"
#include "data.h"
#include "sql.h"
#include "algorythm.h"
#include "event_dialog.h"

namespace Ui {
class CVItemDialog;
}

class CVItemDialog : public QDialog, CVSql, CVAlgorythm
{
    Q_OBJECT

public:
    explicit CVItemDialog(QWidget *parent = 0);
    ~CVItemDialog();

    void setItem(CVItem _item);
    void setGlobalData(CVData *_data);
    CVItem item();

private slots:
    void on_buttonBox_accepted();
    void on_buttonBox_rejected();

    void onCKtoggled();

    void on_tableWidget_cellActivated(int row, int column);

    void on_btAddEvent_clicked();

private:
    Ui::CVItemDialog *ui;

    CVItem f_item;
    CVData *globalData;

    QStringList f_types2;
    QList<CVSpecs> f_types;

    void fillTypes();
    void fillEvents();
};

#endif // ITEM_DIALOG_H
