#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "build.h"
#include <QSettings>
#include <QDebug>
#include <QCheckBox>
#include "item_dialog.h"

#include "sql.h"
#include "event.h"
#include "data.h"
#include "event_dialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, CVSql
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void closeEvent(QCloseEvent *event);

private slots:
    void on_actAddItem_triggered();
    void onDeleteClicked();
    void onEventAdd();
    void showTableWidgetContextMenu(const QPoint &pos);
    void on_tableWidget_cellActivated(int row, int column);

    void onFilterCheckBoxChanged();
    void on_btRefresh_clicked();

    void on_btAdd_clicked();

    void onSetParent();
    void onSetChilds();

    void on_tableWidget_cellClicked(int row, int column);

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;
    bool openDB(QString dbFile);

    void addItem();

    void getItems();
    void fillItems();

    void getEvents();
    void getConnections();

    int typeOfItem(int itemId);
    int parentOfItem(int itemId);
    bool indexOfItem(int itemId);

    void fillFilter();

    CVData *data;
    int colId, colType, colDesc, colVal1, colVal2, colVal3, colQR;

    QList<CVItem> fItems;

    bool parentSelecting;
    int childId, parentId;

    QList<int> typesIndexes;
    QList<bool> typesChecked;
};

#endif // MAINWINDOW_H
