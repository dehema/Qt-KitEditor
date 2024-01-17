#include "kiteditorview.h"
#include "ui_kiteditorview.h".h"

KitEditorView::KitEditorView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KitEditor)
{
    ui->setupUi(this);


    modelMain = new QStandardItemModel();
    QStringList headerMain;
    headerMain << QString::fromLocal8Bit("序号");
    headerMain << QString::fromLocal8Bit("探针染料");
    headerMain << QString::fromLocal8Bit("指标类型");
    headerMain << QString::fromLocal8Bit("指标");
    headerMain << QString::fromLocal8Bit("ct下限");
    headerMain << QString::fromLocal8Bit("ct上限");
    headerMain << QString::fromLocal8Bit("基线下限");
    headerMain << QString::fromLocal8Bit("基线上限");
    headerMain << QString::fromLocal8Bit("颜色");
    modelMain->setHorizontalHeaderLabels(headerMain);
    ui->mainTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->mainTable->setModel(modelMain);
    ui->mainTable->resizeRowsToContents();
    ui->mainTable->resizeColumnsToContents();
    ui->mainTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->mainTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->mainTable->setSelectionMode (QAbstractItemView::SingleSelection);
    ui->mainTable->verticalHeader()->setHidden(true);

    QStandardItemModel* modelPosi = new QStandardItemModel();
    QStringList headerPosi;
    headerPosi << QString::fromLocal8Bit("序号");
    headerPosi << QString::fromLocal8Bit("指标");
    headerPosi << QString::fromLocal8Bit("阳性质控ct上限");
    headerPosi << QString::fromLocal8Bit("阴性质控ct下限");
    modelPosi->setHorizontalHeaderLabels(headerPosi);
    ui->positiveTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->positiveTable->setModel(modelPosi);
}

KitEditorView::~KitEditorView()
{
    delete ui;
}

void KitEditorView::on_btOpenFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,QStringLiteral("选择配置文件"),KitMgr::ins().filePath,QStringLiteral("(*json);"));
    if (!filePath.isEmpty())
    {
        QFile file(filePath);
        if(file.open(QFile::ReadOnly))
        {
            QByteArray byteArray = file.readAll();
            onLoadJsonFile(byteArray);
            file.close();
        }
        else{
            qDebug()<<"打开文件失败";
        }
    }
    else{
    }
}

void KitEditorView::onLoadJsonFile(QByteArray _byteArray)
{
    while (modelMain->rowCount() > 0) {
        modelMain->removeRow(0);
    }
    KitModel kitData = KitMgr::ins().getProcedureListByData(_byteArray);
    modelMain->setRowCount(kitData.spoolList.length());
    for(int i = 0;i<kitData.spoolList.length();i++)
    {
        SpoolModel spoolModel = kitData.spoolList[i];
        QList<QStandardItem*> itemList;
        //序号
        QLabel* lbIndex = new QLabel(QString::number(spoolModel.poolIndex));
        lbIndex->setAlignment(Qt::AlignCenter);
        lbIndex->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(i,0), lbIndex);
        //通道
        QComboBox* cbAbbrName = new QComboBox(this);
        cbAbbrName->addItem(spoolModel.abbrName);
        cbAbbrName->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(i,1), cbAbbrName);
        //指标类型

        QComboBox* cbSpecimenType = new QComboBox(this);
        cbSpecimenType->addItem(spoolModel.abbrName);
        cbSpecimenType->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(i,2), cbSpecimenType);
//        itemList<<new QStandardItem(QString::number(spoolModel.specimenType));
//        //指标
//        itemList<<new QStandardItem(spoolModel.fullName);
//        //ct下限
//        itemList<<new QStandardItem(QString::number(spoolModel.ctMin));
//        //ct上限
//        itemList<<new QStandardItem(QString::number(spoolModel.ctMax));
//        //基线下限
//        itemList<<new QStandardItem(QString::number(spoolModel.datumMin));
//        //基线上限
//        itemList<<new QStandardItem(QString::number(spoolModel.datumMax));
//        modelMain->appendRow(itemList);
//        QLabel* lb = new QLabel(QString::number(spoolModel.poolIndex));
//        lb->setAlignment(Qt::AlignCenter);
//        ui->mainTable->setIndexWidget(modelMain->index(i,0), lb);
        //颜色
        QPushButton* btColor= new QPushButton(ui->mainTable);
        btColor->setObjectName(QString("btColor") + QString::number(i));
        btColor->setStyleSheet(QString("background-color:rgba(%1,255);border:0px;border-radius:0px;").arg(spoolModel.curveColor));
        connect(btColor,SIGNAL(clicked()),this,SLOT(slot_onclickColor()));
        ui->mainTable->setIndexWidget(modelMain->index(i,8), btColor);
    }
    for(int rowIndex = 0;rowIndex < modelMain->rowCount();rowIndex++)
    {
        ui->mainTable->setRowHeight(rowIndex,35);
    }

}
void KitEditorView::addComboBoxToTable(int _rowIndex)
{
    QComboBox *comboBox = new QComboBox();
    comboBox->addItem("x");
    comboBox->addItem("y");
//    ui->mainTable->setIndexWidget(QModelIndex(1,1), comboBox);
}

void KitEditorView::slot_onclickColor()
{
    qDebug()<<sender()->objectName();
}
