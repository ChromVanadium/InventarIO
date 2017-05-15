#include "item_dialog.h"
#include "ui_item_dialog.h"

CVItemDialog::CVItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CVItemDialog)
{
    ui->setupUi(this);

    connect(ui->ck0,SIGNAL(toggled(bool)),this,SLOT(onCKtoggled()));
    connect(ui->ck1,SIGNAL(toggled(bool)),this,SLOT(onCKtoggled()));
}

CVItemDialog::~CVItemDialog()
{
    delete ui;
}

void CVItemDialog::setItem(CVItem _item)
{
    f_item = _item;
    ui->edQR->setText(f_item.QR());
    ui->edName->setText(f_item.name());
    ui->edDescription->setText(f_item.description());
    ui->edValue1->setText(f_item.value1());
    ui->edValue2->setText(f_item.value2());
    ui->edValue3->setText(f_item.value3());

    for(int i=0;i<f_types2.count();i++)
        if(f_item.type().compare(f_types2[i])==0)
            ui->cbType->setCurrentIndex(i);

    fillEvents();
}

void CVItemDialog::setGlobalData(CVData *_data)
{
    globalData = _data;

    fillTypes();
}

CVItem CVItemDialog::item()
{
    return f_item;
}

void CVItemDialog::on_buttonBox_accepted()
{
    f_item.setQR( ui->edQR->text() );
    f_item.setName( ui->edName->text() );
    f_item.setDescription( ui->edDescription->toPlainText() );
    f_item.setValue1( ui->edValue1->text() );
    f_item.setValue2( ui->edValue2->text() );
    f_item.setValue3( ui->edValue3->text() );

    //f_item.setType(f_types[ui->cbType->currentIndex()]);
    f_item.setType(f_types2[ui->cbType->currentIndex()]);

    accept();
}

void CVItemDialog::on_buttonBox_rejected()
{
    reject();
}

void CVItemDialog::onCKtoggled()
{
    fillEvents();
}

void CVItemDialog::fillTypes()
{
    ui->cbType->clear();
    f_types.clear();
    f_types2.clear();

    for(int i=0;i<globalData->types.count();i++)
    {
        ui->cbType->addItem(globalData->types[i].name);
        f_types.append(globalData->types[i]);
        f_types2.append( QString::number(globalData->types[i].index) );
    }
}

void CVItemDialog::fillEvents()
{
    ui->tableWidget->clear();
    QStringList labels;
    labels << "id" << "дата" << "событие";
    ui->tableWidget->setColumnCount(labels.count());
    ui->tableWidget->setHorizontalHeaderLabels(labels);
    ui->tableWidget->setRowCount(0);

    ui->tableWidget->setColumnWidth(0,24);
    ui->tableWidget->setColumnWidth(1,100);
    ui->tableWidget->setColumnWidth(2,240);

    int r = 0;
    for(int i=0;i<f_item.events.count();i++){

        bool show = false;

        if(f_item.events[i].type()==0){
            ui->ck0->isChecked() ? show = true : show = false;
        } else
        if(f_item.events[i].type()==1){
            ui->ck1->isChecked() ? show = true : show = false;
        }

        if( show )
        {
            ui->tableWidget->setRowCount(r+1);

            QTableWidgetItem *wId = new QTableWidgetItem();
            wId->setText( QString("%1").arg(f_item.events[i].id()) );
            ui->tableWidget->setItem(r,0,wId);

            QTableWidgetItem *wDate = new QTableWidgetItem();
            wDate->setText( QString("%1").arg( dateToHuman(f_item.events[i].dateTime()) ) );
            ui->tableWidget->setItem(r,1,wDate);

            QTableWidgetItem *wDescription = new QTableWidgetItem();
            wDescription->setText( QString("%1").arg(f_item.events[i].text()) );
            ui->tableWidget->setItem(r,2,wDescription);

            r++;
        }

    }

    ui->tableWidget->resizeRowsToContents();
    ui->tableWidget->hideColumn(0);
}

void CVItemDialog::on_tableWidget_cellActivated(int row, int column)
{

}

void CVItemDialog::on_btAddEvent_clicked()
{
    CVEvent e;
    e.setParent(f_item.id());
    CVEventDialog *dlg = new CVEventDialog();
    dlg->setEvent(e);

    if(dlg->exec() == QDialog::Accepted){
        e = dlg->event();
        e.toDB(true);
        f_item.addEvent(e);
        fillEvents();
    }
}
