#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    parentSelecting = false;
    childId = 0;
    parentId = 0;

    setWindowTitle( QString("InventarIO. build %1 от %2 %3")
                    .arg(BUILD)
                    .arg(BUILDDATE)
                    .arg(BUILDTIME) );

    QSettings *sets = new QSettings("inventa.ini",  QSettings::IniFormat);
    restoreGeometry(sets->value("Main/geometry").toByteArray());

    data = new CVData();

    if(openDB(sets->value("main/lastDB").toString())){
        data->getFromDB();
        getItems();
        fillItems();
    }

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(showTableWidgetContextMenu(QPoint)));

    fillFilter();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings *sets = new QSettings("inventa.ini",  QSettings::IniFormat);
    sets->setValue("Main/geometry", saveGeometry());
    delete sets;
}

void MainWindow::on_actAddItem_triggered()
{
    addItem();
}

void MainWindow::onDeleteClicked()
{
    QAction *act = qobject_cast<QAction *>(sender());
    int row = act->objectName().toInt();
    int id = ui->tableWidget->item(row,0)->text().toInt();

    for(int i=0;i<fItems.count();i++)
        if(fItems[i].id()==id){
            fItems[i].markToDelete();
            fItems[i].toDB();
        }

    ui->tableWidget->hideRow(row);
}

void MainWindow::onEventAdd()
{
    QAction *act = qobject_cast<QAction *>(sender());
    int row = act->objectName().toInt();
    int id = ui->tableWidget->item(row,0)->text().toInt();

    CVEventDialog *dlg = new CVEventDialog();
    CVEvent e;
    e.setItemId(id);
    dlg->setEvent(e);
    if(dlg->exec() == QDialog::Accepted){
        e = dlg->event();
        e.toDB();

        for(int i=0;i<fItems.count();i++)
            if(fItems[i].id()==id){
                fItems[i].events.append(e);
            }
    }
}

void MainWindow::showTableWidgetContextMenu(const QPoint &pos)
{
    QMenu *mnu = new QMenu("MENU");

    QMenu *submnu = new QMenu("Соотношения");
    mnu->addMenu(submnu);

    QAction *actSetParent = new QAction(this);
    actSetParent->setText("Выбрать родителя");
    actSetParent->setObjectName( QString::number(ui->tableWidget->currentRow()) );
    connect(actSetParent,SIGNAL(triggered()),this,SLOT(onSetParent()));
    submnu->addAction(actSetParent);

    QAction *actSetChilds = new QAction(this);
    actSetChilds->setText("Выбрать подчиненных");
    actSetChilds->setObjectName( QString::number(ui->tableWidget->currentRow()) );
    connect(actSetChilds,SIGNAL(triggered()),this,SLOT(onSetChilds()));
    submnu->addAction(actSetChilds);

    QAction *actEvent = new QAction(this);
    actEvent->setText("Добавить событие");
    actEvent->setObjectName( QString::number(ui->tableWidget->currentRow()) );
    connect(actEvent,SIGNAL(triggered()),this,SLOT(onEventAdd()));
    mnu->addAction(actEvent);

    mnu->addSeparator();
    QAction *actDel = new QAction(this);
    actDel->setText("удалить");
    actDel->setObjectName( QString::number(ui->tableWidget->currentRow()) );
    connect(actDel,SIGNAL(triggered()),this,SLOT(onDeleteClicked()));
    mnu->addAction(actDel);

    mnu->exec(QCursor::pos());
}

bool MainWindow::openDB(QString dbFile){

    bool res = false;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("inventa.db");

    if (!db.open())
    {
        qDebug() << "Base not connected";
        qDebug() << db.lastError().text();
        ui->lbLog->setText("Base not connected: " + db.lastError().text());
    }
    else
    {
        qDebug() << "connection ok";
        res = true;
    };

    return res;
}

void MainWindow::addItem()
{
    CVItemDialog *dlg = new CVItemDialog();
    dlg->setGlobalData(data);

    if(dlg->exec() == QDialog::Accepted){
        CVItem item = dlg->item();
        item.toDB();

        CVEvent e;
        e = CVEvent(item.id(),"Добавлено",0);
        e.toDB();
        item.events.append(e);

        fItems.append(item);
        fillItems();
    };
}

void MainWindow::getItems()
{
    QSqlQuery q;
    QString qs;

    qs = QString("SELECT id, parent, qr, name, description, type, value1, value2, value3, d "
                 "FROM items "
                 "WHERE d=0 "
                 "ORDER BY id ");
    execSQL(&q, qs);
    //CVEvent(0,"Запуск").toDB();

    fItems.clear();
    while(q.next()){
        CVItem itm;
        CVSpecs type;
        type = CVSpecs(data->type(q.record().value("type").toInt()),q.record().value("type").toInt());
        itm = CVItem( q.record().value("id").toInt(),
                      q.record().value("parent").toInt(),
                      q.record().value("qr").toString(),
                      q.record().value("name").toString(),
                      q.record().value("description").toString(),
                      type,
                      q.record().value("value1").toString(),
                      q.record().value("value2").toString(),
                      q.record().value("value3").toString() );

        fItems.append(itm);
    }

    getEvents();
}

void MainWindow::getEvents()
{
    QSqlQuery q;
    QString qs;

    qs = QString("SELECT * FROM events WHERE d=0 ");
    execSQL(&q, qs);
    while(q.next()){
        CVEvent e = CVEvent(q.record().value("id").toInt(),
                            q.record().value("itemid").toInt(),
                            q.record().value("description").toString(),
                            q.record().value("type").toInt(),
                            q.record().value("unix_time").toInt());

        for(int i=0;i<fItems.count();i++){
            if(fItems[i].id()==e.id())
                fItems[i].events.append(e);
        }
    }
}

void MainWindow::getConnections()
{
    QSqlQuery q;
    QString qs;

//    qs = QString("SELECT * FROM connections WHERE d=0 ");
//    execSQL(&q, qs);
//    while(q.next()){
//        CVEvent e = CVEvent(q.record().value("id").toInt(),
//                            q.record().value("itemid").toInt(),
//                            q.record().value("description").toString(),
//                            q.record().value("type").toInt(),
//                            q.record().value("unix_time").toInt());

//        for(int i=0;i<fItems.count();i++){
//            if(fItems[i].id()==e.id())
//                fItems[i].events.append(e);
//        }
    //    }
}

int MainWindow::typeOfItem(int itemId)
{
    int t = 0;

    for(int i=0;i<fItems.count();i++)
        if(fItems[i].id()==itemId)
            t = fItems[i].type().index;

    return t;
}

int MainWindow::parentOfItem(int itemId)
{
    int p = 0;

    for(int i=0;i<fItems.count();i++)
        if(fItems[i].id()==itemId)
            p = fItems[i].parent();

    return p;
}

bool MainWindow::indexOfItem(int itemId)
{
    int t = 0;
    qDebug() << itemId;
    for(int i=0;i<fItems.count();i++){
        qDebug() << "  " << i << fItems[i].id();
        if(fItems[i].id()==itemId)
            t = i;
    }
    qDebug() << "  =" << t << fItems[t].id();
    return t;
}


void MainWindow::fillFilter()
{
    typesIndexes.clear();
    typesChecked.clear();

    for(int i=0;i<data->types.count();i++){
        if(data->types[i].index>0){
            QCheckBox *ck = new QCheckBox();
            ck->setChecked(true);
            typesIndexes.append(data->types[i].index);
            typesChecked.append(true);
            ck->setText(data->types[i].name);
            ck->setObjectName(QString::number(data->types[i].index));
            ui->gridLayout->addWidget(ck,i,0);
            connect(ck,SIGNAL(toggled(bool)),this,SLOT(onFilterCheckBoxChanged()));
        };
    }

    connect(ui->ckShowChilds,SIGNAL(toggled(bool)),this,SLOT(onFilterCheckBoxChanged()));
}

void MainWindow::onFilterCheckBoxChanged()
{
    QCheckBox *ck = qobject_cast<QCheckBox *>(sender());
    int r = ck->objectName().toInt();
    bool c = ck->isChecked();

    for(int i=0;i<typesIndexes.count();i++)
        if(typesIndexes[i] == r)
            typesChecked[i] = ck->isChecked();

    for(int i=0;i<ui->tableWidget->rowCount();i++){
        int id = ui->tableWidget->item(i,0)->text().toInt();
        int t = typeOfItem(id);

        if(t==r)
            ui->tableWidget->setRowHidden(i,!c);

        int p = parentOfItem(id);

        if(p>0)
            ui->tableWidget->setRowHidden(i,ui->ckShowChilds->isChecked());


    }
}

void MainWindow::fillItems()
{
    ui->tableWidget->clear();
    QStringList labels;
    labels << "id" << "тип" << "QR" << "Описание" << "1" << "2" << "3" << "-";
    ui->tableWidget->setColumnCount(labels.count());
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    ui->tableWidget->setRowCount(fItems.count());

    colId = 0;
    colType = 1;
    colDesc = 3;
    colVal1 = 4;
    colVal2 = 5;
    colVal3 = 6;
    colQR = 2;

    ui->tableWidget->setColumnWidth(colId,24);
    ui->tableWidget->setColumnWidth(colType,120);
    ui->tableWidget->setColumnWidth(colQR,80);
    ui->tableWidget->setColumnWidth(colDesc,240);
    ui->tableWidget->setColumnWidth(colVal1,120);
    ui->tableWidget->setColumnWidth(colVal2,120);
    ui->tableWidget->setColumnWidth(colVal3,120);

    for(int i=0;i<fItems.count();i++){
        QTableWidgetItem *wId = new QTableWidgetItem();
        wId->setText( QString("%1").arg(fItems[i].id()) );
        ui->tableWidget->setItem(i,colId,wId);

        QTableWidgetItem *wQR = new QTableWidgetItem();
        wQR->setText( QString("%1").arg(fItems[i].QR()) );
        ui->tableWidget->setItem(i,colQR,wQR);

//        QTableWidgetItem *wName = new QTableWidgetItem();
//        wName->setText( QString("%1").arg(fItems[i].name()) );
//        ui->tableWidget->setItem(i,2,wName);

        QTableWidgetItem *wDescription = new QTableWidgetItem();
        wDescription->setText( QString("%1").arg(fItems[i].description()) );
        ui->tableWidget->setItem(i,colDesc,wDescription);

        QTableWidgetItem *wType = new QTableWidgetItem();
        wType->setText( QString("%1").arg(fItems[i].type().name) );
        ui->tableWidget->setItem(i,colType,wType);

        QTableWidgetItem *wV1 = new QTableWidgetItem();
        wV1->setText( QString("%1").arg(fItems[i].value1()) );
        ui->tableWidget->setItem(i,colVal1,wV1);
        QTableWidgetItem *wV2 = new QTableWidgetItem();
        wV2->setText( QString("%1").arg(fItems[i].value2()) );
        ui->tableWidget->setItem(i,colVal2,wV2);
        QTableWidgetItem *wV3 = new QTableWidgetItem();
        wV3->setText( QString("%1").arg(fItems[i].value3()) );
        ui->tableWidget->setItem(i,colVal3,wV3);

        if(fItems[i].parent()>0)
            ui->tableWidget->setRowHidden(i,ui->ckShowChilds->isChecked());
    }

    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->hideColumn(colId);

}



void MainWindow::on_tableWidget_cellActivated(int row, int column)
{
    CVItemDialog *dlg = new CVItemDialog();
    dlg->setGlobalData(data);
    dlg->setItem(fItems[row]);

    if(dlg->exec() == QDialog::Accepted){
        CVItem item = dlg->item();
        item.toDB();

        fItems[row] = item;
//        fItems.append(item);
        fillItems();
    };
}



void MainWindow::on_btRefresh_clicked()
{
    getItems();
    fillItems();
}

void MainWindow::on_btAdd_clicked()
{
    addItem();
}

void MainWindow::onSetParent()
{
    parentSelecting = true;
    int r = ui->tableWidget->currentRow();
    childId = ui->tableWidget->item(r,0)->text().toInt();
    qDebug() << "parentSelecting" << parentSelecting << "childId" << childId;
}

void MainWindow::onSetChilds()
{

}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
    if(parentSelecting){
        int r = row;
        parentId = ui->tableWidget->item(r,0)->text().toInt();
        for(int i=0;i<fItems.count();i++){
            if(fItems[i].id()==childId){
                fItems[i].setParent(parentId);
                fItems[i].toDB();
            }
        }
        qDebug() << "parentSelecting" << parentSelecting << "childId" << childId << "parentId" << parentId;
        parentSelecting = false;
        childId = 0;
        parentId = 0;
    }
}
