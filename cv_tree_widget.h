#ifndef CV_TREE_WIDGET_H
#define CV_TREE_WIDGET_H

#include <QTreeWidget>
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>
#include <QStandardItemModel>

namespace Ui {
class CVTreeWidget;
}

class CVTreeWidget : public QTreeWidget
{
    Q_OBJECT

public:
    explicit CVTreeWidget(QWidget *parent = 0);
    ~CVTreeWidget();

signals:
    void dragAndDropped(int from, int to);
    void somethingDropped();

private:
    Ui::CVTreeWidget *ui;

    void dropEvent(QDropEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);

    QString dragSource, dragDestination;

};

#endif // CV_TREE_WIDGET_H
