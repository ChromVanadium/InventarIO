#ifndef EVENT_DIALOG_H
#define EVENT_DIALOG_H

#include <QDialog>
#include "event.h"

namespace Ui {
class CVEventDialog;
}

class CVEventDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CVEventDialog(QWidget *parent = 0);
    ~CVEventDialog();

    void setEvent(CVEvent _event);
    CVEvent event();

    void setTitle(QString _title);

private slots:
    void on_buttonBox_accepted();

private:
    Ui::CVEventDialog *ui;

    CVEvent f_event;
};

#endif // EVENT_DIALOG_H
