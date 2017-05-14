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
}

CVTreeWidget::~CVTreeWidget()
{
    delete ui;
}

void CVTreeWidget::dropEvent(QDropEvent *event)
{
    QTreeWidget::dropEvent(event);

    const QMimeData* qMimeData = event->mimeData();

    QByteArray encoded = qMimeData->data("application/x-qabstractitemmodeldatalist");
    QDataStream stream(&encoded, QIODevice::ReadOnly);

    QString into = "-1";
    QString from = "-1";

    while (!stream.atEnd())
    {
        int row, col;
        QMap<int,  QVariant> roleDataMap;

        stream >> row >> col >> roleDataMap;
        QString dropped = roleDataMap[0].toString();

        if(col==3) {
            qDebug() << event->pos();
                from = roleDataMap[0].toString();
                if(this->itemAt(event->pos())!=NULL)
                    into = this->itemAt(event->pos())->text(2);
                else
                    into = "-1";

            }

    }

    emit dragAndDropped(from.toInt(), into.toInt());
    emit somethingDropped();
}
