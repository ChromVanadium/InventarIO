#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    logForm = new CVLogForm();

    parentSelecting = false;
    childId = 0;
    parentId = 0;

    //ui->btJson->setVisible(false);

    QSettings *sets = new QSettings("inventa.ini",  QSettings::IniFormat);
    restoreGeometry(sets->value("Main/geometry").toByteArray());

    data = new CVData();

//    ui->treeWidget->setVisible(false);
//    ui->tableWidget->setVisible(false);
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
        fillTable();

        ui->lbLastServerTime->setText( QString("%1 (%2)")
                                       .arg(data->lastServerTime().toString("dd.MM.yyyy hh:mm:ss"))
                                       .arg(data->lastServerTime().toTime_t()));
        ui->lbDelta->setText( QString::number(data->timeDelta()) );
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
    ui->stackedWidget->setCurrentIndex(0);
    ui->toolBox->setCurrentIndex(0);

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

//    for(int i=0;i<fItems.count();i++)
//        if(fItems[i].id()==id){
//            fItems[i].markToDelete();
//            fItems[i].toDB();
//        }

    ui->tableWidget->hideRow(row);
}

void MainWindow::onEventAdd()
{
//    QAction *act = qobject_cast<QAction *>(sender());
//    int row = act->objectName().toInt();
//    int id = ui->tableWidget->item(row,0)->text().toInt();

//    CVEventDialog *dlg = new CVEventDialog();
//    CVEvent e;
//    e.setItemId(id);
//    dlg->setEvent(e);
//    if(dlg->exec() == QDialog::Accepted){
//        e = dlg->event();
//        e.toDB();

//        for(int i=0;i<fItems.count();i++)
//            if(fItems[i].id()==id){
//                fItems[i].events.append(e);
//            }
//    }
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

    CVItem itm;
    dlg->setItem(itm);

    if(dlg->exec() == QDialog::Accepted){
        CVItem item = dlg->item();
        item.setModified(true);
        item.toDB(true);

        CVEvent e;
        e = CVEvent(item.id(),"Добавлено",0);
        e.toDB(true);
        item.events.append(e);

        fItems.append(item);
        fillTree();
    };
}

void MainWindow::editItem(CVItem _item)
{
    CVItemDialog *dlg = new CVItemDialog();
    dlg->setGlobalData(data);
    dlg->setItem(_item);

    if(dlg->exec() == QDialog::Accepted){
        CVItem item = dlg->item();
        item.toDB(false);

        for(int i=0;i<fItems.count();i++)
            if(fItems[i].id().compare(item.id())==0)
                fItems[i] = item;
        CVEvent e;
        e = CVEvent(item.id(),"Добавлено",0);
        e.toDB(false);
        item.events.append(e);

        fItems.append(item);
        fillTree();
    };
}

void MainWindow::editItem(QTreeWidgetItem *itm)
{
    bool expanded = itm->isExpanded();
    QString id = itm->text(colId);
    int unq = itm->text(colUnq).toInt();
    int j = -1;

    for(int i=0;i<fItems.count();i++)
        if(fItems[i].unq()==unq)
            j = i;

    if(j>-1){

        CVItemDialog *dlg = new CVItemDialog();
        dlg->setGlobalData(data);
        dlg->setItem(fItems[j]);

        if(dlg->exec() == QDialog::Accepted){
            CVItem item = dlg->item();
            item.toDB(false);
            fItems[j]=item;
            fillTreeItem(itm,item);
        };
        itm->setExpanded(expanded);
    };
}

void MainWindow::addEvent(int pos)
{
    CVEvent e;
    e.setParent(fItems[pos].id());
    CVEventDialog *dlg = new CVEventDialog();
    dlg->setTitle("событие по " + fItems[pos].name());
    dlg->setEvent(e);

    if(dlg->exec() == QDialog::Accepted){
        e = dlg->event();
        e.toDB(true);
        fItems[pos].addEvent(e);
    }
}

void MainWindow::getItems()
{
    QSqlQuery q;
    QString qs;

    qs = QString("SELECT unq, id, parent, qr, name, description, type, value1, value2, value3, d, lvl, u, modified, sid "
                 "FROM items "
                 "WHERE d=0 "
                 "ORDER BY lvl, unq ");
    execSQL(&q, qs);
    //CVEvent(0,"Запуск").toDB();

    fItems.clear();
    while(q.next()){
        CVItem itm;
        itm = CVItem( q.record().value("unq").toInt(),
                      q.record().value("id").toString(),
                      q.record().value("parent").toString(),
                      q.record().value("lvl").toInt(),
                      q.record().value("qr").toString(),
                      q.record().value("name").toString(),
                      q.record().value("description").toString(),
                      q.record().value("type").toString(),
                      q.record().value("value1").toString(),
                      q.record().value("value2").toString(),
                      q.record().value("value3").toString(),
                      q.record().value("u").toInt(),
                      q.record().value("modified").toInt(),
                      q.record().value("sid").toInt());
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
        CVEvent e = CVEvent(q.record().value("unq").toInt(),
                            q.record().value("id").toString(),
                            q.record().value("parent").toString(),
                            q.record().value("description").toString(),
                            q.record().value("type").toInt(),
                            q.record().value("unix_time").toInt(),
                            q.record().value("u").toInt(),
                            q.record().value("modified").toInt(),
                            q.record().value("sid").toInt());

        for(int i=0;i<fItems.count();i++){
            if(fItems[i].id().compare(e.parent())==0){
                fItems[i].addEvent(e);
            }

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
        if(fItems[i].unq() == itemId)
            t = fItems[i].type().toInt();

    return t;
}

int MainWindow::parentOfItem(int itemId)
{
    int p = 0;

//    for(int i=0;i<fItems.count();i++)
//        if(fItems[i].id()==itemId)
//            p = fItems[i].parent();

    return p;
}

bool MainWindow::indexOfItem(int itemId)
{
    int t = 0;
//    for(int i=0;i<fItems.count();i++){
//        if(fItems[i].id()==itemId)
//            t = i;
//    }
    return t;
}


void MainWindow::fillFilter()
{
    //ui->frame->setVisible(false);
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

    //connect(ui->ckShowChilds,SIGNAL(toggled(bool)),this,SLOT(onFilterCheckBoxChanged()));
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

        //int p = parentOfItem(id);

//        if(p>0)
//            ui->tableWidget->setRowHidden(i,ui->ckShowChilds->isChecked());
    }
}


void MainWindow::fillTree()
{
    fillTree2();
}

void MainWindow::fillTree2()
{
qDebug() << "\nvoid MainWindow::fillTree2()...";
    tree->clear();
    QStringList labels;
    labels << "Название" << "unq" << "uuid" << "Инв.№" << "тип" << "1" << "2" << "3" << "ID" << "-";
    tree->setColumnCount(labels.count());
    tree->setHeaderLabels(labels);

    colUnq = 1;
    colId = 2;
    colType = 4;
    colName = 0;
    colVal1 = 5;
    colVal2 = 6;
    colVal3 = 7;
    colQR = 3;
    colSID = 8;

    tree->setColumnCount(labels.count());
    tree->setHeaderLabels(labels);

    QList<CVItem> tempItems = fItems;

    // сначала пробегаем по безродительским итемам
    for(int i=0;i<tempItems.count();i++){
        if(tempItems[i].parent().isEmpty()){
            QTreeWidgetItem *tw = new QTreeWidgetItem();
            tree->addTopLevelItem(tw);
            fillTreeItem(tw, tempItems[i]);
            tempItems.takeAt(i); i--;
        }
    }

    // далее идем по оставшимся "детям"
    int looper = 0;
    while(tempItems.count()>0){

        QTreeWidgetItemIterator it(tree);
        while (*it) {
            QTreeWidgetItem *wp = (*it);
            QString id = wp->text(colId);
//            int j = -1;
            QList<int> j;
            j.clear();

            for(int i=0;i<tempItems.count();i++){
                if (id.compare(tempItems[i].parent())==0){
                    QTreeWidgetItem *tw = new QTreeWidgetItem();
                    wp->addChild(tw);
                    fillTreeItem(tw, tempItems[i]);
                    j.append(i);
                }
            }
            ++it;
            for(int a=j.count()-1;a>=0;a--){
                qDebug() << "a" << a << j.count() << "tempItems.count()" << tempItems.count();
                            tempItems.takeAt(j[a]);
            }
        }

        looper++;                       /* за случай зацикливания */
        if(looper==1000) break;         /*                        */
    }

    tree->setColumnWidth(colName,260);
    tree->setColumnWidth(colType,100);
    tree->setColumnWidth(colVal1,120);
    tree->setColumnWidth(colVal2,120);
    tree->setColumnWidth(colVal3,120);
    tree->setColumnWidth(colQR,50);
    tree->setColumnWidth(colSID,350);

    tree->hideColumn(colId);
    tree->hideColumn(colUnq);
    tree->expandAll();
qDebug() << "...void MainWindow::fillTree2()";
}

void MainWindow::fillTable()
{
    QStringList labels;
    labels << "unq" << "id" << "QR" << "Имя" << "тип" << "1" << "2" << "3" << "ID" << "-";
    ui->tableWidget->setColumnCount(labels.count());
    ui->tableWidget->setHorizontalHeaderLabels(labels);

    ui->tableWidget->setColumnWidth(2,48);
    ui->tableWidget->setColumnWidth(3,100);
    ui->tableWidget->setColumnWidth(4,300);
    ui->tableWidget->setColumnWidth(5,200);
    ui->tableWidget->setColumnWidth(6,200);
    ui->tableWidget->setColumnWidth(7,200);


    int r=1;
    for(int i=0;i<fItems.count();i++){
        ui->tableWidget->setRowCount(r);
        fillTableItem(i,fItems[i]);
        r++;
    }

    ui->tableWidget->setColumnHidden(0,true);
    ui->tableWidget->setColumnHidden(1,true);
    ui->tableWidget->resizeRowsToContents();

    //onFilterCheckBoxChanged();
}

void MainWindow::fillTableItem(int row, CVItem _item)
{
    //labels << "unq" << "id" << "QR" << "Имя" << "тип" << "1" << "2" << "3" << "ID" << "-";

    QTableWidgetItem *w0 = new QTableWidgetItem();
    w0->setText(QString::number(_item.unq()));
    ui->tableWidget->setItem(row,0,w0);

    QTableWidgetItem *w1 = new QTableWidgetItem();
    w1->setText(_item.id());
    ui->tableWidget->setItem(row,1,w1);

    QTableWidgetItem *w2 = new QTableWidgetItem();
    w2->setText(_item.QR());
    ui->tableWidget->setItem(row,2,w2);
    w2->setTextAlignment(Qt::AlignRight|Qt::AlignVCenter);

    QTableWidgetItem *w3 = new QTableWidgetItem();
    w3->setText(_item.name());
    ui->tableWidget->setItem(row,4,w3);

    QTableWidgetItem *w4 = new QTableWidgetItem();
    w4->setText( data->type(_item.type().toInt()) );
    ui->tableWidget->setItem(row,3,w4);

    QTableWidgetItem *w5 = new QTableWidgetItem();
    w5->setText(_item.value1());
    ui->tableWidget->setItem(row,5,w5);

    QTableWidgetItem *w6 = new QTableWidgetItem();
    w6->setText(_item.value2());
    ui->tableWidget->setItem(row,6,w6);

    QTableWidgetItem *w7 = new QTableWidgetItem();
    w7->setText(_item.value3());
    ui->tableWidget->setItem(row,7,w7);

    QTableWidgetItem *w8 = new QTableWidgetItem();
    QString w8str = QString("%1: %2")
            .arg(_item.sid())
            .arg(_item.id());
    w8->setText(w8str);
    ui->tableWidget->setItem(row,8,w8);

//    QTableWidgetItem *w8 = new QTableWidgetItem();
//    w8->setText(QString::number(_item.unq()));
//    ui->tableWidget->setItem(row,8,w8);

//    QTableWidgetItem *w9 = new QTableWidgetItem();
//    w9->setText(QString::number(_item.unq()));
//    ui->tableWidget->setItem(row,9,w9);

}

void MainWindow::fillTreeItem(QTreeWidgetItem *_wi, CVItem _item)
{
    _wi->setText(colUnq,  QString("%1").arg(_item.unq()) );
    _wi->setText(colId,  QString("%1").arg(_item.id()) );
    _wi->setText(colQR,   QString("%1").arg(_item.QR()) );
    _wi->setText(colName, QString("%1").arg(_item.name()) );
    _wi->setText(colType, QString("%1").arg( data->type(_item.type().toInt()) ) );
    _wi->setText(colVal1, QString("%1").arg(_item.value1()) );
    _wi->setText(colVal2, QString("%1").arg(_item.value2()) );
    _wi->setText(colVal3, QString("%1").arg(_item.value3()) );

    QString w8str = QString("%1: %2")
            .arg(_item.sid())
            .arg(_item.id());
    if(_item.modified())
        w8str.append(" *");
     _wi->setText(colSID, w8str );

    _wi->setTextAlignment(colQR,Qt::AlignVCenter|Qt::AlignRight);
}



void MainWindow::on_tableWidget_cellActivated(int row, int column)
{
    CVItemDialog *dlg = new CVItemDialog();
    dlg->setGlobalData(data);
    dlg->setItem(fItems[row]);

    if(dlg->exec() == QDialog::Accepted){
        CVItem item = dlg->item();
        item.toDB(false);

        fItems[row] = item;
        fillTree();
    };
}

void MainWindow::on_btRefresh_clicked()
{
    getItems();
    fillTree();
    fillTable();
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
}

void MainWindow::on_tableWidget_cellClicked(int row, int column)
{
//    if(parentSelecting){
//        int r = row;
//        parentId = ui->tableWidget->item(r,0)->text().toInt();
//        for(int i=0;i<fItems.count();i++){
//            if(fItems[i].id()==childId){
//                fItems[i].setParent(parentId);
//                fItems[i].toDB();
//            }
//        }
//        qDebug() << "parentSelecting" << parentSelecting << "childId" << childId << "parentId" << parentId;
//        parentSelecting = false;
//        childId = 0;
//        parentId = 0;
//    }
}

void MainWindow::onDragAndDropped(int from, int into)
{
    qDebug() << "item" << from << "dropped into" << into;
    int f=0, t=0;
    for(int i=0;i<fItems.count();i++){
        if(fItems[i].QR().toInt() == from) f = i;
        if(fItems[i].QR().toInt() == into) t = i;
    }
    if(from==into)
        return;

    if(fItems[f].id().compare(fItems[t].id())!=0){
        fItems[f].setParent(fItems[t].id());
        fItems[f].toDB(false);
        fItems[f].addEvent( QString("Перенесен в подчинение %1").arg(into) );
        fItems[t].addEvent( QString("Добавлен подчиненный %1").arg(from) );
    }
    if(into == -1){
        fItems[f].setParent("");
        fItems[f].toDB(false);
    }
    tree->expandAll();
}

void MainWindow::onSomethingDropped()
{
    qDebug() << "something dropped";
}

void MainWindow::on_btJson_clicked()
{
    QJsonArray managers;

    QJsonObject manager1;
    manager1["title"] = "Ножкин";
    manager1["value"] = 250000;
    manager1["short"] = true;

    QJsonObject manager2;
    manager2["title"] = "Ложкин";
    manager2["value"] = 150000;
    manager2["short"] = true;

    QJsonObject manager3;
    manager3["title"] = "Кошкин";
    manager3["value"] = 199999;
    manager3["short"] = true;

    QJsonArray fields;
    fields.append(manager1);
    fields.append(manager2);
    fields.append(manager3);

    QJsonArray attachments;
    QJsonObject attachment;
    attachment["fallback"] = "Отчет по манагерам за месяц";
    attachment["pretext"] = "Отчет по манагерам за месяц";
    //attachment["text"] = "text ";
    attachment["color"] = "#36a64f";
    attachment["fields"] = fields;

    QJsonObject isp1;
    isp1["title"] = "Соколова";
    isp1["value"] = "22300 / 1200";
    isp1["short"] = true;

    QJsonObject isp2;
    isp2["title"] = "Молчунова";
    isp2["value"] = "15000 / 0";
    isp2["short"] = true;

    QJsonObject isp3;
    isp3["title"] = "Колбочкина";
    isp3["value"] = "0 / 1200";
    isp3["short"] = true;

    QJsonArray fields2;
    fields2.append(isp1);
    fields2.append(isp2);
    fields2.append(isp3);

    QJsonObject attachment2;
    attachment2["fallback"] = "Отчет по штатникам";
    attachment2["pretext"] = "Отчет по штатникам";
    //attachment["text"] = "text ";
    attachment2["color"] = "#4f36a6";
    attachment2["fields"] = fields2;

    attachments.append(attachment);
    attachments.append(attachment2);

    QJsonObject payload;
    payload["username"] = "ОТЧЕТ";
    //payload["text"] = "text0";
    payload["fallback"] = "fallback";
    payload["attachments"] = attachments;

    QJsonObject jsonToSend;
    //jsonToSend["payload"] = payload;

    QJsonDocument doc(payload);
    QByteArray strJson = doc.toJson();

    QString urlo = QString("https://hooks.slack.com/services/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");

    QNetworkAccessManager *m = new QNetworkAccessManager(this);
    QUrl url(urlo);

    QNetworkRequest request(url);
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2);
    request.setSslConfiguration(config);

    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    //request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    connect(m, SIGNAL(finished(QNetworkReply *)), this, SLOT(webhookReplyFinished(QNetworkReply *)));
    connect(m, SIGNAL(finished(QNetworkReply *)), m, SLOT(deleteLater()));

    //QByteArray jsn = "{\"text\":\"Hey! this is a test\"}";
    qDebug() << strJson;
    m->post(request, "payload=" + strJson);
    //m->post(request, strJson);

//    QJsonArray itemsJson;
//    for(int i=0;i<fItems.count();i++){
//        QJsonObject j = fItems[i].toJson();
//        itemsJson.append(j);

//    }

//    QJsonArray eventsJson;
//    for(int i=0;i<fItems.count();i++){
//        for(int j=0;j<fItems[i].events.count();j++){
//            QJsonObject j0 = fItems[i].events[j].toJson();
//            eventsJson.append(j0);
//        }
//    }

//    int dt = QDateTime::currentDateTime().toTime_t();
//    syncJson["items"] = itemsJson;
//    syncJson["events"] = eventsJson;
//    syncJson["source_id"] = data->uuid();
//    syncJson["comp_time"] = dt;

//    QJsonDocument doc(syncJson);
//    QByteArray strJson = doc.toJson();
//    //QString strJson(doc.toJson(QJsonDocument::Compact));

//    sendNewToServer(strJson);
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
//    QByteArray content = reply->readAll();
//    qDebug() << content;

//    QJsonDocument json(QJsonDocument::fromJson(content));
//    qDebug() << "\n\n";
//    qDebug() << json;

//    QJsonObject j1 = json.object();
//    qDebug() << "\n\n";
//    int done = j1["done"].toInt();
//    QJsonArray newSids = j1["newSids"].toArray();

//    for(int j=0;j<newSids.count();j++){
//        QJsonObject obj = newSids[j].toObject();
//        qDebug() << obj["uuid"].toString() << " =>" << obj["sid"].toInt();
//        for(int i=0;i<fItems.count();i++){
//            if(fItems[i].uuid().compare(obj["uuid"].toString(),Qt::CaseInsensitive)==0){
//                fItems[i].setSid(obj["sid"].toInt());
//                fItems[i].updateToDB();
//            }

//        }
//    }
//    ui->lbUpdated->setText( QString("last sync %1").arg(QDateTime::currentDateTime().toString("hh:mm.ss")) );
    //qDebug() << done << ar;
}


void MainWindow::on_btAddEvent_clicked()
{
    QString selId = tree->currentItem()->text(colId);
    int pos = -1;
    for(int i=0;i<fItems.count();i++){
        if(fItems[i].id().compare(selId)==0)
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
                   "unq	INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "id	TEXT, "                                         /* UUID */
                   "parent	TEXT, "                                     /* UUID */
                   "description	TEXT, "
                   "unix_time	INTEGER, "
                   "type	INTEGER DEFAULT 0, "                        /* 0 - ручное, 1 - авто */
                   "d	INTEGER DEFAULT 0, "
                   "u	INTEGER, "
                   "modified	INTEGER DEFAULT 1,"
                   "sid	INTEGER DEFAULT 0 "
                   ") ");
        query.exec("CREATE TABLE fields_labels ( "
                   "unq	INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "label1	TEXT, "
                   "label2	TEXT, "
                   "label3	TEXT, "
                   "u	INTEGER "
                   ") ");
        query.exec("CREATE TABLE items ( "
                   "unq	INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "id	TEXT, "                                         /* UUID */
                   "parent	TEXT, "                                     /* UUID */
                   "name	TEXT, "
                   "description	TEXT, "
                   "type	TEXT, "
                   "value1	TEXT, "
                   "value2	TEXT, "
                   "value3	TEXT, "
                   "lvl	INTEGER DEFAULT 0, "
                   "d	INTEGER DEFAULT 0, "
                   "qr	TEXT, "
                   "u	INTEGER, "
                   "modified	INTEGER DEFAULT 1, "
                   "sid	INTEGER DEFAULT 0 "
                   ") ");
//        `unq`	INTEGER PRIMARY KEY AUTOINCREMENT,
//            `id`	TEXT,
        query.exec("CREATE TABLE localsets ( "
                   "uuid	TEXT, "
                   "timedelta	INTEGER, "
                   "lastservertime	INTEGER, "
                   "serverurl   TEXT"
                   ") ");
        query.exec("CREATE TABLE types ( "
                   "unq	INTEGER PRIMARY KEY AUTOINCREMENT, "
                   "id	TEXT, "
                   "description	TEXT, "
                   "u	INTEGER "
                   ") ");

        query.exec( QString("INSERT INTO localsets (uuid, timedelta, lastservertime, serverurl) "
                    "VALUES('%1',0,0,'%2')")
                    .arg(data->uuid())
                    .arg("http://193.150.105.40:16980/inventario/sendNew.php") );

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
    sync1();
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

    ui->lbUpdated->setText("sending to serva");

    st0 = QDateTime::currentDateTime();
    st1 = st0;
    syncLog.append( QString("sync started at %1").arg(st0.toString("hh:mm:ss.zzz")) );

    QSqlQuery q;
    QString qs;

    qs = QString("SELECT MAX(sid) AS sid FROM items ");
    q = execSQLq(qs);
    q.next();
    int maxItemSid = q.record().value("sid").toInt();

    qs = QString("SELECT MAX(sid) AS sid FROM events ");
    q = execSQLq(qs);
    q.next();
    int maxEventSid = q.record().value("sid").toInt();

    qs = QString("SELECT lastservertime FROM localsets");
    execSQL(&q,qs);
    q.next();
    int lastServerTime = q.record().value("lastservertime").toInt();

    // таблица items
    // выборка вновь внесенных данных (п.1)
    // выборка измененных локально данных (п.2)
    qs = QString("SELECT * "
                 "FROM items "
                 "WHERE modified=1");
    execSQL(&q, qs);

    QJsonArray itemsJson;

    while(q.next()){
        QJsonObject json;
        json["id"] = q.record().value("id").toString();
        json["sid"] = q.record().value("sid").toInt();
        json["name"] = q.record().value("name").toString();
        json["description"] = q.record().value("description").toString();
        json["type"] = q.record().value("type").toString();
        json["value1"] = q.record().value("value1").toString();
        json["value2"] = q.record().value("value2").toString();
        json["value3"] = q.record().value("value3").toString();
        json["d"] = q.record().value("d").toInt();
        json["qr"] = q.record().value("qr").toString();
        json["parent"] = q.record().value("parent").toString();
        json["level"] = q.record().value("lvl").toInt();
        json["u"] = q.record().value("u").toInt();

        itemsJson.append(json);
    }

    QJsonArray eventsJson;
    qs = QString("SELECT * FROM events WHERE modified=1");
    execSQL(&q, qs);
    while(q.next()){
        QJsonObject json;
        json["id"] = q.record().value("id").toString();
        json["sid"] = q.record().value("sid").toInt();
        json["parent"] = q.record().value("parent").toString();
        json["type"] = q.record().value("type").toInt();
        json["text"] = q.record().value("description").toString();
        json["datetime"] = q.record().value("unix_time").toInt();
        json["d"] = q.record().value("d").toInt();
        json["u"] = q.record().value("u").toInt();
        eventsJson.append(json);
    }

    int dt = QDateTime::currentDateTime().toTime_t();
    syncJson["max_item_sid"] = maxItemSid;
    syncJson["max_event_sid"] = maxEventSid;
    syncJson["new_items"] = itemsJson;
    syncJson["new_events"] = eventsJson;
    syncJson["source_id"] = data->uuid();
    syncJson["comp_time"] = dt;
    syncJson["last_sync"] = lastServerTime;

    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("created json to send to the server in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    QJsonDocument doc(syncJson);
    QByteArray strJson = doc.toJson();
qDebug() << "sending:\n" << doc;
    QString urlo = QString(data->serverUrl());

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url(urlo);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    manager->post(request, "json=" + strJson);

    ui->lbUpdated->setText("receiving from server...");

    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(syncReplyFinished(QNetworkReply *)));
    connect(manager, SIGNAL(finished(QNetworkReply *)), manager, SLOT(deleteLater()));
}

void MainWindow::sync1()
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

    ui->lbUpdated->setText("sending items to serva");

    st0 = QDateTime::currentDateTime();
    st1 = st0;
    syncLog.append( QString("sync items started at %1").arg(st0.toString("hh:mm:ss.zzz")) );

    QSqlQuery q;
    QString qs;

    qs = QString("SELECT MAX(sid) AS sid FROM items ");
    q = execSQLq(qs);
    q.next();
    int maxItemSid = q.record().value("sid").toInt();

    logForm->addText(qs);

    qs = QString("SELECT lastservertime FROM localsets");
    execSQL(&q,qs);
    q.next();
    int lastServerTime = q.record().value("lastservertime").toInt();

    logForm->addText(qs);
    // таблица items
    // выборка вновь внесенных данных (п.1)
    // выборка измененных локально данных (п.2)
    qs = QString("SELECT * "
                 "FROM items "
                 "WHERE sid=0 OR modified=1");
    execSQL(&q, qs);

    logForm->addText(qs);

    QJsonArray newItemsJson;
    QJsonArray modifiedItemsJson;

    while(q.next()){
        QJsonObject json;
        json["id"] = q.record().value("id").toString();
        json["sid"] = q.record().value("sid").toInt();
        json["name"] = q.record().value("name").toString();
        json["description"] = q.record().value("description").toString();
        json["type"] = q.record().value("type").toString();
        json["value1"] = q.record().value("value1").toString();
        json["value2"] = q.record().value("value2").toString();
        json["value3"] = q.record().value("value3").toString();
        json["d"] = q.record().value("d").toInt();
        json["qr"] = q.record().value("qr").toString();
        json["parent"] = q.record().value("parent").toString();
        json["level"] = q.record().value("lvl").toInt();
        json["u"] = q.record().value("u").toInt();

        if(q.record().value("sid").toInt()==0)
            newItemsJson.append(json);
        else
            modifiedItemsJson.append(json);
    }

    int dt = QDateTime::currentDateTime().toTime_t();
    syncJson["max_item_sid"] = maxItemSid;
    syncJson["new_items"] = newItemsJson;
    syncJson["modified_items"] = modifiedItemsJson;
    syncJson["source_id"] = data->uuid();
    syncJson["comp_time"] = dt;
    syncJson["last_sync"] = lastServerTime;

    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("created items json to send to the server in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    QJsonDocument doc(syncJson);
    QByteArray strJson = doc.toJson();

    logForm->addText("-= ОТПРАВЛЕНО =-");
    logForm->addText(strJson);

qDebug() << "sending:\n" << doc;
    QString urlo = QString(data->serverUrl());
    urlo = "http://193.150.105.40:16980/inventario/sync1.php";

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    QUrl url(urlo);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    manager->post(request, "json=" + strJson);

    ui->lbUpdated->setText("receiving from server...");

    connect(manager, SIGNAL(finished(QNetworkReply *)), this, SLOT(syncItemsReplyFinished(QNetworkReply *)));
    connect(manager, SIGNAL(finished(QNetworkReply *)), manager, SLOT(deleteLater()));
}

void MainWindow::syncReplyFinished(QNetworkReply *reply)
{
    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("received response from server in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    ui->lbUpdated->setText("processing");

    QByteArray content = reply->readAll();
    qDebug() << "\nreceiving:";
qDebug() << content;
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
                             "u=%1, sid=%3, modified=0 WHERE id LIKE '%2'")
                .arg(obj["u"].toInt())
                .arg(obj["id"].toString())
                .arg(obj["sid"].toInt());
        execSQL(qs);
    }

    /* получаем sid от отправленных данных в таблицу events */
    QJsonArray newEventsSids = j1["newEventsSids"].toArray();
    for(int j=0;j<newEventsSids.count();j++){
        QJsonObject obj = newEventsSids[j].toObject();
        QString qs = QString("UPDATE events SET "
                             "u=%1, sid=%3, modified=0 WHERE id LIKE '%2'")
                .arg(obj["u"].toInt())
                .arg(obj["id"].toString())
                .arg(obj["sid"].toInt());
        execSQL(qs);
    }

    /* получаем новые данные из items внесенные извне */
    QJsonArray newItemsFromAnotherPlace = j1["newItemsFromAnotherPlace"].toArray();
    for(int j=0;j<newItemsFromAnotherPlace.count();j++){
        QJsonObject obj = newItemsFromAnotherPlace[j].toObject();

        CVItem itm;
        itm = CVItem( 0,
                      obj["id"].toString(),
                      obj["parent"].toString(),
                      obj["lvl"].toInt(),
                      obj["qr"].toString(),
                      obj["name"].toString(),
                      obj["description"].toString(),
                      obj["type"].toString(),
                      obj["value1"].toString(),
                      obj["value2"].toString(),
                      obj["value3"].toString(),
                      obj["u"].toInt(),
                      0,
                      obj["sid"].toInt());
        itm.toDB(true);
        fItems.append(itm);
    }

    /* получаем новые данные из items внесенные извне */
    QJsonArray newEventsFromAnotherPlace = j1["newEventsFromAnotherPlace"].toArray();
    for(int j=0;j<newEventsFromAnotherPlace.count();j++){
        QJsonObject obj = newEventsFromAnotherPlace[j].toObject();
        CVEvent e = CVEvent(0,
                            obj["id"].toString(),
                            obj["parent"].toString(),
                            obj["description"].toString(),
                            obj["type"].toInt(),
                            obj["time"].toInt(),
                            obj["u"].toInt(),
                            0,
                            obj["sid"].toInt());
        e.toDB(true);
    }

    QString qs = QString("UPDATE localsets SET lastservertime=%1").arg(serverTime);
    execSQL(qs);

    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("inserted reveived data in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    syncLog.append( QString("sync time %1 msecs").arg(st0.msecsTo(st2)) );

    ui->lbUpdated->setText( QString("last sync %1 (%2)")
                            .arg(QDateTime::currentDateTime().toString("hh:mm:ss"))
                            .arg(QDateTime::currentDateTime().toTime_t()));

    //qDebug() << syncLog;

    getItems();
    //getEvents();
    fillTree();
}

void MainWindow::syncItemsReplyFinished(QNetworkReply *reply)
{
    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("received response from server in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    ui->lbUpdated->setText("processing");

    QByteArray content = reply->readAll();

    logForm->addText("-= ПОЛУЧЕНО =-");
   // logForm->addText(content);

qDebug() << "\nreceived:" << content;
    QJsonDocument json(QJsonDocument::fromJson(content));

    logForm->addText(json.toJson());
//    qDebug() << json;

    QJsonObject j1 = json.object();

    int serverTime = j1["servertime"].toInt();

    /* получаем sid от отправленных данных в таблицу items */
    QJsonArray newItemsSids = j1["newItemsSids"].toArray();
    logForm->addText("newItemsSids");
    for(int j=0;j<newItemsSids.count();j++){
        QJsonObject obj = newItemsSids[j].toObject();
        QString qs = QString("UPDATE items SET "
                             "u=%1, sid=%3, modified=0 WHERE id LIKE '%2'")
                .arg(serverTime)
                .arg(obj["id"].toString())
                .arg(obj["sid"].toInt());
        execSQL(qs);
        logForm->addText(qs);
    }

    /* апдейтим время изменения на серверное для измененнных позиций */
    QJsonArray modifiedItemsSids = j1["modifiedItems"].toArray();
    logForm->addText("modifiedItems");
    for(int j=0;j<modifiedItemsSids.count();j++){
        QJsonObject obj = modifiedItemsSids[j].toObject();
        QString qs = QString("UPDATE items SET "
                             "u=%1, modified=0 WHERE id LIKE '%2'")
                .arg(obj["u"].toInt())
                .arg(obj["id"].toString());
        execSQL(qs);
        logForm->addText(qs);
    }

    QSqlQuery q;
    QString qs1;
    qs1 = QString("SELECT sid FROM items WHERE d=0");
    q = execSQLq(qs1);
    QList<int> sids;
    while(q.next()){
        sids.append(q.record().value("sid").toInt());
    }

    /* получаем измененные данные из items внесенные извне */
    QJsonArray updatedItemsInAnotherPlace = j1["updatedItemsInAnotherPlace"].toArray();
    logForm->addText("updatedItemsInAnotherPlace");
    for(int j=0;j<updatedItemsInAnotherPlace.count();j++){
        QJsonObject obj = updatedItemsInAnotherPlace[j].toObject();

        CVItem itm;
        itm = CVItem( 0,
                      obj["id"].toString(),
                obj["parent"].toString(),
                obj["lvl"].toInt(),
                obj["qr"].toString(),
                obj["name"].toString(),
                obj["description"].toString(),
                obj["type"].toString(),
                obj["value1"].toString(),
                obj["value2"].toString(),
                obj["value3"].toString(),
                obj["u"].toInt(),
                0,
                obj["sid"].toInt());
        itm.updateToDBbySID();
    }

    /* получаем новые данные из items внесенные извне */
    QJsonArray newItemsFromAnotherPlace = j1["newItemsFromAnotherPlace"].toArray();
    logForm->addText("newItemsFromAnotherPlace");
    for(int j=0;j<newItemsFromAnotherPlace.count();j++){
        QJsonObject obj = newItemsFromAnotherPlace[j].toObject();

        int e = 0;
        for(int j=0;j<sids.count();j++){
            if(obj["sid"].toInt()==sids[j])
                e++;
        }

        if(e==0){
            CVItem itm;
            itm = CVItem( 0,
                          obj["id"].toString(),
                          obj["parent"].toString(),
                          obj["lvl"].toInt(),
                          obj["qr"].toString(),
                          obj["name"].toString(),
                          obj["description"].toString(),
                          obj["type"].toString(),
                          obj["value1"].toString(),
                          obj["value2"].toString(),
                          obj["value3"].toString(),
                          obj["u"].toInt(),
                          0,
                          obj["sid"].toInt());
            //itm.reHash();
            itm.toDB(true);
            fItems.append(itm);
        } else {
            // updatedItemsInAnotherPlace
        }
    }

    int currectTime = QDateTime::currentDateTime().toTime_t();
    int timeDelta = currectTime-serverTime;
    QString qs = QString("UPDATE localsets SET "
                         "lastservertime=%1, "
                         "timedelta=%2")
            .arg(serverTime)
            .arg(timeDelta);
    execSQL(qs);
    ui->lbLastServerTime->setText( QString("%1 (%2)")
                                   .arg(QDateTime::fromTime_t(serverTime).toString("dd.MM.yyyy hh:mm:ss"))
                                   .arg(serverTime) );
    ui->lbDelta->setText( QString::number(timeDelta) );
    logForm->addText(qs);

    st2 = QDateTime::currentDateTime();
    syncLog.append( QString("inserted reveived data in %1 msecs").arg(st1.msecsTo(st2)) );
    st1 = st2;

    syncLog.append( QString("sync time %1 msecs").arg(st0.msecsTo(st2)) );

    ui->lbUpdated->setText( QString("last sync %1").arg(QDateTime::currentDateTime().toString("hh:mm:ss")) );

    getItems();
//    //getEvents();
    fillTree();
}

void MainWindow::webhookReplyFinished(QNetworkReply *reply)
{
    QByteArray content = reply->readAll();

qDebug() << "\nreceived:" << content;
//    QJsonDocument json(QJsonDocument::fromJson(content));

//    logForm->addText(json.toJson());
}



void MainWindow::on_btDelete_clicked()
{
    QString selId = tree->currentItem()->text(colId);
    bool hasChilds = tree->currentItem()->childCount()>0;
    int pos = -1;
    for(int i=0;i<fItems.count();i++){
        if(fItems[i].id().compare(selId)==0)
            pos = i;
    }
    if(pos>-1 && !hasChilds){
        fItems[pos].markToDelete();
        fItems[pos].setModified(true);
        fItems[pos].toDB(true);
        getItems();
        fillTree();
    }
}

void MainWindow::on_action_sql_triggered()
{
    CVSqlDialog *d = new CVSqlDialog();
    d->exec();
}

void MainWindow::on_toolBox_currentChanged(int index)
{
    switch(index){
        case 0: ui->stackedWidget->setCurrentIndex(0); break;
        case 1: ui->stackedWidget->setCurrentIndex(1); break;
    }
}

void MainWindow::on_btLog_clicked()
{
    logForm->show();
}

void MainWindow::on_actClearBase_triggered()
{
    QSqlQuery q;
    QString qs;

    qs = QString("DELETE FROM items");
    execSQL(qs);

    qs = QString("DELETE FROM events");
    execSQL(qs);
}
