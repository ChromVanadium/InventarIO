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

    ui->btJson->setVisible(false);

    QSettings *sets = new QSettings("inventa.ini",  QSettings::IniFormat);
    restoreGeometry(sets->value("Main/geometry").toByteArray());

    data = new CVData();

    ui->treeWidget->setVisible(false);
    ui->tableWidget->setVisible(false);
    tree = new CVTreeWidget();
    tree->setExpandsOnDoubleClick(false);
    ui->vl3->addWidget(tree);
    connect(tree,SIGNAL(dragAndDropped(int,int)),this,SLOT(onDragAndDropped(int,int)));
    connect(tree,SIGNAL(somethingDropped()),this,SLOT(onSomethingDropped()));
    connect(tree,SIGNAL(itemActivated(QTreeWidgetItem*,int)),this,SLOT(onItemActivated(QTreeWidgetItem*,int)));

    if(openDB(sets->value("main/lastDB").toString())){
        data->getFromDB();
        getItems();
        //fillItems();
        fillTree();
    }

    setWindowTitle( QString("InventarIO. %4. build %1 от %2 %3")
                    .arg(BUILD)
                    .arg(BUILDDATE)
                    .arg(BUILDTIME)
                    .arg(databaseName));

    ui->tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidget,SIGNAL(customContextMenuRequested(QPoint)),
            this,SLOT(showTableWidgetContextMenu(QPoint)));

    fillFilter();

    qDebug() << QDateTime::currentDateTime();
    qDebug() << QDateTime::currentDateTimeUtc();
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

void MainWindow::dropEvent(QDropEvent *event)
{
    qDebug() << "dropped" << event->source()->objectName();

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
    if(dbFile.isEmpty())
        dbFile = "inventa.db";

    databaseName = dbFile;

    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
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

void MainWindow::editItem(CVItem _item)
{
    CVItemDialog *dlg = new CVItemDialog();
    dlg->setGlobalData(data);
    dlg->setItem(_item);

    if(dlg->exec() == QDialog::Accepted){
        CVItem item = dlg->item();
        item.toDB();

        for(int i=0;i<fItems.count();i++)
            if(fItems[i].id()==item.id())
                fItems[i] = item;
//        CVEvent e;
//        e = CVEvent(item.id(),"Добавлено",0);
//        e.toDB();
//        item.events.append(e);

//        fItems.append(item);
//        fillItems();
    };
}

void MainWindow::editItem(QTreeWidgetItem *itm)
{
    bool expanded = itm->isExpanded();
    int id = itm->text(colId).toInt();
    int j = -1;
    for(int i=0;i<fItems.count();i++)
        if(fItems[i].id()==id)
            j = i;

    if(j>-1){

        CVItemDialog *dlg = new CVItemDialog();
        dlg->setGlobalData(data);
        dlg->setItem(fItems[j]);

        if(dlg->exec() == QDialog::Accepted){
            CVItem item = dlg->item();
            item.toDB();
            fItems[j]=item;
            fillTreeItem(itm,item);
        };
        itm->setExpanded(expanded);
    };
}

void MainWindow::addEvent(int pos)
{
    CVEvent e;
    e.setItemId(fItems[pos].id());
    CVEventDialog *dlg = new CVEventDialog();
    dlg->setEvent(e);

    if(dlg->exec() == QDialog::Accepted){
        e = dlg->event();
        e.toDB();
        fItems[pos].addEvent(e);
    }
    //fItems[pos].addEvent("");
}

void MainWindow::getItems()
{
    QSqlQuery q;
    QString qs;

    qs = QString("SELECT id, sid, parent, uuid, qr, name, description, type, value1, value2, value3, d, lvl, u "
                 "FROM items "
                 "WHERE d=0 "
                 "ORDER BY lvl, id ");
    execSQL(&q, qs);
    //CVEvent(0,"Запуск").toDB();

    fItems.clear();
    while(q.next()){
        CVItem itm;
        CVSpecs type;
        type = CVSpecs(data->type(q.record().value("type").toInt()),q.record().value("type").toInt());
        itm = CVItem( q.record().value("id").toInt(),
                      q.record().value("sid").toInt(),
                      q.record().value("uuid").toString(),
                      q.record().value("parent").toInt(),
                      q.record().value("lvl").toInt(),
                      q.record().value("qr").toString(),
                      q.record().value("name").toString(),
                      q.record().value("description").toString(),
                      type,
                      q.record().value("value1").toString(),
                      q.record().value("value2").toString(),
                      q.record().value("value3").toString(),
                      q.record().value("u").toInt() );

        fItems.append(itm);
    }

    getEvents();

    organizeItems();
}

void MainWindow::getEvents()
{
    QSqlQuery q;
    QString qs;

    qs = QString("SELECT * FROM events WHERE d=0 ");
    execSQL(&q, qs);
    while(q.next()){
        CVEvent e = CVEvent(q.record().value("id").toInt(),
                            q.record().value("sid").toInt(),
                            q.record().value("uuid").toString(),
                            q.record().value("itemid").toInt(),
                            q.record().value("description").toString(),
                            q.record().value("type").toInt(),
                            q.record().value("unix_time").toInt(),
                            q.record().value("u").toInt());

        for(int i=0;i<fItems.count();i++){
            if(fItems[i].id()==e.itemId())
                fItems[i].addEvent(e);
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
    for(int i=0;i<fItems.count();i++){
        if(fItems[i].id()==itemId)
            t = i;
    }
    return t;
}


void MainWindow::fillFilter()
{
    ui->frame->setVisible(false);
//    typesIndexes.clear();
//    typesChecked.clear();

//    for(int i=0;i<data->types.count();i++){
//        if(data->types[i].index>0){
//            QCheckBox *ck = new QCheckBox();
//            ck->setChecked(true);
//            typesIndexes.append(data->types[i].index);
//            typesChecked.append(true);
//            ck->setText(data->types[i].name);
//            ck->setObjectName(QString::number(data->types[i].index));
//            ui->gridLayout->addWidget(ck,i,0);
//            connect(ck,SIGNAL(toggled(bool)),this,SLOT(onFilterCheckBoxChanged()));
//        };
//    }

    //    connect(ui->ckShowChilds,SIGNAL(toggled(bool)),this,SLOT(onFilterCheckBoxChanged()));
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
    ui->tableWidget->setVisible(false);
    fillTree();
    return;

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
    ui->tableWidget->setColumnWidth(colVal1,240);
    ui->tableWidget->setColumnWidth(colVal2,240);
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
            ui->tableWidget->setRowHidden(i,!ui->ckShowChilds->isChecked());

    }


    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->hideColumn(colId);
}

void MainWindow::fillTree()
{
    tree->clear();
    QStringList labels;
    labels << "Название" << "id" << "Инв.№" << "тип" << "1" << "2" << "3" << "-";
    tree->setColumnCount(labels.count());
    tree->setHeaderLabels(labels);

    colId = 1;
    colType = 3;
    colName = 0;
    colVal1 = 4;
    colVal2 = 5;
    colVal3 = 6;
    colQR = 2;

    tree->setColumnCount(labels.count());
    tree->setHeaderLabels(labels);

    QList<CVItem> tempItems = fItems;

    // сначала пробегаем по безродительским итемам
    for(int i=0;i<tempItems.count();i++){
        if(tempItems[i].parent()==0){
            QTreeWidgetItem *tw = new QTreeWidgetItem();
            tree->addTopLevelItem(tw);
            fillTreeItem(tw, tempItems[i]);
            tempItems.takeAt(i); i--;
        }
    }

    // далее идем по оставшимся "детям"
    int looper = 0;
    while(tempItems.count()>0){
        for(int i=0;i<tempItems.count();i++){
            QTreeWidgetItemIterator it(tree);
                while (*it) {
                    QTreeWidgetItem *wp = (*it);
                    QString cid = wp->text(colId);
                    if (cid.toInt() == tempItems[i].parent()){
                        QTreeWidgetItem *tw = new QTreeWidgetItem();
                        wp->addChild(tw);
                        fillTreeItem(tw, tempItems[i]);
                        tempItems.takeAt(i); i--;
                    }
                    ++it;
                }
        }
        looper++;
        if(looper==1000)
            break;
    }

    tree->setColumnWidth(colName,260);
    tree->setColumnWidth(colType,80);
    tree->setColumnWidth(colVal1,120);
    tree->setColumnWidth(colVal2,120);
    tree->setColumnWidth(colVal3,120);
    tree->setColumnWidth(colQR,50);


    tree->hideColumn(colId);
    tree->expandAll();
}

void MainWindow::fillTreeItem(QTreeWidgetItem *_wi, CVItem _item)
{
    _wi->setText(colId,   QString("%1").arg(_item.id()) );
    _wi->setText(colQR,   QString("%1").arg(_item.QR()) );
    _wi->setText(colName, QString("%1").arg(_item.name()) );
    _wi->setText(colType, QString("%1").arg(_item.type().name) );
    _wi->setText(colVal1, QString("%1").arg(_item.value1()) );
    _wi->setText(colVal2, QString("%1").arg(_item.value2()) );
    _wi->setText(colVal3, QString("%1").arg(_item.value3()) );

    _wi->setTextAlignment(colQR,Qt::AlignVCenter|Qt::AlignRight);
}

void MainWindow::organizeItems()
{

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
    //fillItems();
    fillTree();
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

void MainWindow::onItemActivated(QTreeWidgetItem *it, int col)
{
    editItem(it);
    return;

//    int id = it->text(colId).toInt();
//    int j = -1;
//    for(int i=0;i<fItems.count();i++)
//        if(fItems[i].id()==id)
//            j = i;

//    if(j>-1)
//        editItem(fItems[j]);
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

void MainWindow::onDragAndDropped(int from, int into)
{
    qDebug() << "item" << from << "dropped into" << into;
    int f=0, t=0;
    for(int i=0;i<fItems.count();i++){
        if(fItems[i].QR().toInt() == from) f = i;
        if(fItems[i].QR().toInt() == into) t = i;
    }

    if(fItems[f].id()!=fItems[t].id()){
        fItems[f].setParent(fItems[t].id());
        fItems[f].toDB();
        fItems[f].addEvent( QString("Перенесен в подчинение %1").arg(into) );
        fItems[t].addEvent( QString("Добавлен подчиненный %1").arg(from) );
    }
    tree->expandAll();
}

void MainWindow::onSomethingDropped()
{
    qDebug() << "something dropped";
}

void MainWindow::on_btJson_clicked()
{
    QJsonArray itemsJson;
    for(int i=0;i<fItems.count();i++){
        QJsonObject j = fItems[i].toJson();
        itemsJson.append(j);

    }

    QJsonArray eventsJson;
    for(int i=0;i<fItems.count();i++){
        for(int j=0;j<fItems[i].events.count();j++){
            QJsonObject j0 = fItems[i].events[j].toJson();
            eventsJson.append(j0);
        }
    }

    int dt = QDateTime::currentDateTime().toTime_t();
    syncJson["items"] = itemsJson;
    syncJson["events"] = eventsJson;
    syncJson["source_id"] = data->uuid();
    syncJson["comp_time"] = dt;

    QJsonDocument doc(syncJson);
    QByteArray strJson = doc.toJson();
    //QString strJson(doc.toJson(QJsonDocument::Compact));

    sendNewToServer(strJson);
}


void MainWindow::sendNewToServer(QByteArray json)
{
    QString urlo = QString("http://193.150.105.40:16980/inventario/sendNew.php");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url(urlo);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    manager->post(request, "json=" + json);

    qDebug() << urlo;

    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(replyfinished(QNetworkReply *)));
    connect(manager, SIGNAL(finished(QNetworkReply *)), manager, SLOT(deleteLater()));

    //manager->get(request);
}



void MainWindow::replyfinished(QNetworkReply *reply){
    QByteArray content = reply->readAll();
    qDebug() << content;

    QJsonDocument json(QJsonDocument::fromJson(content));
    qDebug() << "\n\n";
    qDebug() << json;

    QJsonObject j1 = json.object();
    qDebug() << "\n\n";
    int done = j1["done"].toInt();
    QJsonArray newSids = j1["newSids"].toArray();

    for(int j=0;j<newSids.count();j++){
        QJsonObject obj = newSids[j].toObject();
        qDebug() << obj["uuid"].toString() << " =>" << obj["sid"].toInt();
        for(int i=0;i<fItems.count();i++){
            if(fItems[i].uuid().compare(obj["uuid"].toString(),Qt::CaseInsensitive)==0){
                fItems[i].setSid(obj["sid"].toInt());
                fItems[i].updateToDB();
            }

        }
    }
    ui->lbUpdated->setText( QString("last sync %1").arg(QDateTime::currentDateTime().toString("hh:mm.ss")) );
    //qDebug() << done << ar;
}


void MainWindow::on_btAddEvent_clicked()
{
    int selId = tree->currentItem()->text(colId).toInt();
    int pos = -1;
    for(int i=0;i<fItems.count();i++){
        if(fItems[i].id()==selId)
            pos = i;
    }
    if(pos>-1)
        addEvent(pos);
}

void MainWindow::on_actCreate_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, "Новая база",
                                               "Название базы данных", QLineEdit::Normal,
                                               "", &ok);

    if (ok && !text.isEmpty()){
        QString path = text + ".db";
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");//not dbConnection
        db.setDatabaseName(path);
        db.open();
        QSqlQuery query;
        query.exec("CREATE TABLE events ( "
                   "id	INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "sid	INTEGER DEFAULT 0, "
                   "itemid	INTEGER, "
                   "description	TEXT, "
                   "unix_time	INTEGER, "
                   "type	INTEGER, "
                   "d	INTEGER DEFAULT 0, "
                   "u	INTEGER, "
                   "uuid	TEXT, "
                   "modified	INTEGER DEFAULT 1 "
                   ")");
        query.exec("CREATE TABLE fields_labels ( "
                   "type	INTEGER, "
                   "label1	TEXT, "
                   "label2	TEXT, "
                   "label3	TEXT, "
                   "u	INTEGER "
                   ")");
        query.exec("CREATE TABLE items ( "
                   "id	INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "sid	INTEGER DEFAULT 0, "
                   "parent	INTEGER DEFAULT 0, "
                   "name	TEXT, "
                   "description	TEXT, "
                   "type	INTEGER DEFAULT 0, "
                   "value1	TEXT, "
                   "value2	TEXT, "
                   "value3	TEXT, "
                   "lvl	INTEGER DEFAULT 0, "
                   "d	INTEGER DEFAULT 0, "
                   "qr	TEXT, "
                   "u	INTEGER, "
                   "uuid	TEXT, "
                   "modified	INTEGER DEFAULT 1 "
                   ")");
        query.exec("CREATE TABLE localsets ( "
                   "uuid	TEXT, "
                   "timedelta	INTEGER, "
                   "lastservertime	INTEGER "
                   ")");
        query.exec("CREATE TABLE types ( "
                   "id	INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "description	TEXT, "
                   "u	INTEGER, "
                   ")");

        if(openDB(text)){
            data->getFromDB();
            getItems();
            fillTree();
        }

        setWindowTitle( QString("InventarIO. %4. build %1 от %2 %3")
                        .arg(BUILD)
                        .arg(BUILDDATE)
                        .arg(BUILDTIME)
                        .arg(databaseName));
    }
}

void MainWindow::on_actOpenDB_triggered()
{
    QFileDialog *f = new QFileDialog();
    f->setDirectory( QDir::currentPath() );

    if (f->exec() == QDialog::Accepted){
        QString text = f->selectedFiles().at(0);

        QFileInfo dr;
        dr.setFile( text );
        QString fN = dr.fileName();

        db.close();

        if(openDB(fN)){
            data->getFromDB();
            getItems();
            fillTree();

            QSettings *sets = new QSettings("inventa.ini",  QSettings::IniFormat);
            sets->setValue("main/lastDB",fN);

            setWindowTitle( QString("InventarIO. %4. build %1 от %2 %3")
                            .arg(BUILD)
                            .arg(BUILDDATE)
                            .arg(BUILDTIME)
                            .arg(databaseName));
        }
    }
}

void MainWindow::on_btSync_clicked()
{
    sync();
}

void MainWindow::sync()
{
    /*
     По каждой таблице в базе:
     1. Сначала выбрать все новые (без sid)
     2. Выбрать все измененнные локально
     2.1. Добавить в базу метку об изменениии
     3. Отправить все изменения на сервак
     4. Из полученного ответа:
     4.1. Присвоить sid всем новым, изменить время на серверное
     4.2. Внести изменения, полученные с сервера, в локальную базу
     */

    st0 = QDateTime::currentDateTime();
    st1 = st0;
    syncLog.append( QString("sync started at %1").arg(st0.toString("hh:mm:ss.zzz")) );

    QSqlQuery q;
    QString qs;

    qs = QString("SELECT MAX(sid) AS sid FROM items");
    execSQL(&q,qs);
    q.next();
    int maxItemsSid = q.record().value("sid").toInt();

    qs = QString("SELECT MAX(sid) AS sid FROM events");
    execSQL(&q,qs);
    q.next();
    int maxEventsSid = q.record().value("sid").toInt();

    qs = QString("SELECT lastservertime FROM localsets");
    execSQL(&q,qs);
    q.next();
    int lastServerTime = q.record().value("lastservertime").toInt();


    // таблица items
    // выборка вновь внесенных данных (п.1)
    // выборка измененных локально данных (п.2)
    qs = QString("SELECT id, sid, parent, uuid, qr, name, description, type, value1, value2, value3, d, lvl, u "
                 "FROM items "
                 "WHERE modified=1");
    execSQL(&q, qs);

    QJsonArray itemsJson;

    while(q.next()){
        QJsonObject json;
        json["id"] = q.record().value("id").toInt();
        json["sid"] = q.record().value("sid").toInt();
        json["name"] = q.record().value("name").toString();
        json["description"] = q.record().value("description").toString();
        json["type"] = q.record().value("type").toInt();
        json["value1"] = q.record().value("value1").toString();
        json["value2"] = q.record().value("value2").toString();
        json["value3"] = q.record().value("value3").toString();
        json["d"] = q.record().value("d").toInt();
        json["qr"] = q.record().value("qr").toString();
        json["parent"] = q.record().value("parent").toInt();
        json["level"] = q.record().value("lvl").toInt();
        json["u"] = q.record().value("u").toInt();
        json["uuid"] = q.record().value("uuid").toString();
        itemsJson.append(json);
    }

    QJsonArray eventsJson;
    qs = QString("SELECT * FROM events "
                 "WHERE modified=1");
    execSQL(&q, qs);
    while(q.next()){
        QJsonObject json;
        json["id"] = q.record().value("id").toInt();
        json["sid"] = q.record().value("sid").toInt();
        json["itemid"] = q.record().value("itemin").toInt();
        json["type"] = q.record().value("type").toInt();
        json["text"] = q.record().value("description").toString();
        json["datetime"] = q.record().value("unix_time").toInt();
        json["d"] = q.record().value("d").toInt();
        json["u"] = q.record().value("u").toInt();
        json["uuid"] = q.record().value("uuid").toString();
        eventsJson.append(json);
    }

    int dt = QDateTime::currentDateTime().toTime_t();
    syncJson["items"] = itemsJson;
    syncJson["events"] = eventsJson;
    syncJson["source_id"] = data->uuid();
    syncJson["comp_time"] = dt;
    syncJson["last_sync"] = lastServerTime;
    syncJson["maxItemsSid"] = maxItemsSid;
    syncJson["maxEventsSid"] = maxEventsSid;
    //syncJson["last_update"] = dt;

    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("created json to send to the server in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    QJsonDocument doc(syncJson);
    QByteArray strJson = doc.toJson();
qDebug() << doc << "\n" << strJson;

    QString urlo = QString("http://193.150.105.40:16980/inventario/sendNew.php");

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url(urlo);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    manager->post(request, "json=" + strJson);



    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(syncReplyFinished(QNetworkReply *)));
    connect(manager, SIGNAL(finished(QNetworkReply *)), manager, SLOT(deleteLater()));
}

void MainWindow::syncReplyFinished(QNetworkReply *reply)
{
    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("received response from server in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    QByteArray content = reply->readAll();
//    qDebug() << "\nreceiving:";
//    qDebug() << content;

    QJsonDocument json(QJsonDocument::fromJson(content));
//    qDebug() << "\n";
    qDebug() << json;

    QJsonObject j1 = json.object();
    int done = j1["done"].toInt();

    int serverTime = j1["servertime"].toInt();

    /* получаем sid от отправленных данных в таблицу items */
    QJsonArray newItemsSids = j1["newItemsSids"].toArray();
    for(int j=0;j<newItemsSids.count();j++){
        QJsonObject obj = newItemsSids[j].toObject();
        QString qs = QString("UPDATE items SET "
                             "sid=%1, u=%2, modified=0 WHERE uuid LIKE '%3'")
                .arg(obj["sid"].toInt())
                .arg(obj["u"].toInt())
                .arg(obj["uuid"].toString());
        execSQL(qs);
    }

    /* получаем sid от отправленных данных в таблицу events */
    QJsonArray newEventsSids = j1["newEventsSids"].toArray();
    for(int j=0;j<newEventsSids.count();j++){
        QJsonObject obj = newEventsSids[j].toObject();
        QString qs = QString("UPDATE events SET "
                             "sid=%1, u=%2, modified=0 WHERE uuid LIKE '%3'")
                .arg(obj["sid"].toInt())
                .arg(obj["u"].toInt())
                .arg(obj["uuid"].toString());
        execSQL(qs);
    }

    /* получаем новые данные из items внесенные извне */
    QJsonArray newItemsFromAnotherPlace = j1["newItemsFromAnotherPlace"].toArray();
    for(int j=0;j<newItemsFromAnotherPlace.count();j++){
        QJsonObject obj = newItemsFromAnotherPlace[j].toObject();

        CVItem itm;
        CVSpecs type;
        type = CVSpecs(data->type(obj["type"].toInt()),obj["type"].toInt());
        itm = CVItem( 0,
                      obj["sid"].toInt(),
                      obj["uuid"].toString(),
                      obj["parent"].toInt(),
                      obj["lvl"].toInt(),
                      obj["qr"].toString(),
                      obj["name"].toString(),
                      obj["description"].toString(),
                      type,
                      obj["value1"].toString(),
                      obj["value2"].toString(),
                      obj["value3"].toString(),
                      obj["u"].toInt() );
        itm.setModified(true);
        itm.toDB();
        fItems.append(itm);
    }

    QString qs = QString("UPDATE localsets SET lastservertime=%1").arg(serverTime);
    execSQL(qs);

    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("inserted reveived data in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    syncLog.append( QString("sync time %1 msecs").arg(st0.msecsTo(st2)) );

    ui->lbUpdated->setText( QString("last sync %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss")) );

    qDebug() << syncLog;

    getItems();
    getEvents();
    fillTree();
}


