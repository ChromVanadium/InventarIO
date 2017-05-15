#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDropEvent>
#include <QFileDialog>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QMainWindow>
#include "build.h"
#include <QSettings>
#include <QDebug>
#include <QCheckBox>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QInputDialog>
#include "item_dialog.h"

#include "sql.h"
#include "event.h"
#include "data.h"
#include "event_dialog.h"
#include "cv_tree_widget.h"

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
    void dropEvent(QDropEvent *event);


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
    void onItemActivated(QTreeWidgetItem* it,int col);

    void on_tableWidget_cellClicked(int row, int column);
    void onDragAndDropped(int from, int into);
    void onSomethingDropped();

    void on_btJson_clicked();
    void on_btAddEvent_clicked();
    void replyfinished(QNetworkReply *reply);
    void on_actCreate_triggered();
    void on_actOpenDB_triggered();

    void syncReplyFinished(QNetworkReply *reply);

    void on_btSync_clicked();

    void on_btDelete_clicked();

private:
    Ui::MainWindow *ui;

    QSqlDatabase db;
    bool openDB(QString dbFile);
    QString databaseName;

    void addItem();
    void editItem(CVItem _item);
    void editItem(QTreeWidgetItem *itm);

    void addEvent(int pos);

    void getItems();

    void fillTree();
    void fillTree2();

    void fillTreeItem(QTreeWidgetItem *_wi, CVItem _item);

    void getEvents();
    void getConnections();

    int typeOfItem(int itemId);
    int parentOfItem(int itemId);
    bool indexOfItem(int itemId);

    void fillFilter();

    void sendNewToServer(QByteArray json);

    CVTreeWidget *tree;

    QJsonObject syncJson;

    CVData *data;
    int colUnq, colId, colRow, colType, colDesc, colVal1, colVal2, colVal3, colQR, colName;

    QStringList syncLog;
    QDateTime st0, st1, st2;

    QList<CVItem> fItems;

    bool parentSelecting;
    int childId, parentId;

    QList<int> typesIndexes;
    QList<bool> typesChecked;

    void sync();
};

#endif // MAINWINDOW_H
