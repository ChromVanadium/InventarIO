#ifndef LOG_FORM_H
#define LOG_FORM_H

#include <QWidget>
#include <QDateTime>

namespace Ui {
class CVLogForm;
}

class CVLogForm : public QWidget
{
    Q_OBJECT

public:
    explicit CVLogForm(QWidget *parent = 0);
    ~CVLogForm();

    void addText(QString text);

private slots:
    void on_btClear_clicked();

private:
    Ui::CVLogForm *ui;
};

#endif // LOG_FORM_H
