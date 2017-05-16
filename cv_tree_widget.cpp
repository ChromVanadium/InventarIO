#include "cv_tree_widget.h"
#include "ui_cv_tree_widget.h"

CVTreeWidget::CVTreeWidget(QWidget *parent) :
    QTreeWidget(parent),
    ui(new Ui::CVTreeWidget)
{
    ui->setupUi(this);

    setAcceptDrops(true);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::InternalMove);
    setDropIndicatorShown(true);

    dragSource = "-1";
    dragDestination = "-1";
}

CVTreeWidget::~CVTreeWidget()
{
    delete ui;
}

void CVTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget::dropEvent(event);

    qDebug() << event->type();

    QModelIndex dropIndex = indexAt(event->pos());
    QTreeWidgetItem* destination_item = itemFromIndex(dropIndex);

    const QMimeData* qMimeData = event->mimeData();

    QByteArray encoded = qMimeData->data("application/x-qabstractitemmodeldatalist");
    QString mySt(encoded);

    QDataStream stream(&encoded, QIODevice::ReadOnly);

    QString into = "-1";
    QString from = "-1";
    dragSource = "-1";

    while (!stream.atEnd())
    {
        int row, col;
        QMap<int,  QVariant> roleDataMap;

        stream >> row >> col >> roleDataMap;
        QString dropped = roleDataMap[0].toString();
        if(col==3) {
                from = roleDataMap[0].toString();
                dragSource = roleDataMap[0].toString();

                if(this->itemAt(event->pos())!=NULL){
                    into = this->itemAt(event->pos())->text(3);
                }
                else {
                    into = "-1";
                }
            }

    }

    //emit dragAndDropped(from.toInt(), into.toInt());
    emit dragAndDropped(dragSource.toInt(), dragDestination.toInt());
    emit somethingDropped();
}

void CVTreeWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QModelIndex dropIndex = indexAt(event->pos());
    QTreeWidgetItem* destination_item = itemFromIndex(dropIndex);
    if(destination_item != NULL)
        dragDestination = destination_item->text(3);
    else
        dragDestination = "-1";
    qDebug() << dragDestination;

    QTreeWidget::dropEvent(event);
}

