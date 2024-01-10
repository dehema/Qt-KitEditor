#include "kiteditorview.h"
#include "ui_kiteditorview.h".h"

KitEditorView::KitEditorView(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KitEditor)
{
    ui->setupUi(this);


    QStandardItemModel* modelMain = new QStandardItemModel();
    QStringList headerMain;
    headerMain<<"序号"<<"通道"<<"指标类型"<<"指标"<<"ct下限"<<"ct上限"<<"基线下限"<<"基线上限"<<"阈值"<<"颜色";
    modelMain->setHorizontalHeaderLabels(headerMain);
    ui->mainTable->setModel(modelMain);


    QStandardItemModel* modelPosi = new QStandardItemModel();
    QStringList headerPosi;
    headerPosi<<"序号"<<"指标"<<"ct上限"<<"阈值";
    modelPosi->setHorizontalHeaderLabels(headerPosi);
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
    KitModel kitData = KitMgr::ins().getProcedureListByData(_byteArray);
    for(SpoolModel spoolModel:kitData.spoolList)
    {
        QList<QStandardItem> itemList;
        itemList.append(QStandardItem(QString(spoolModel.poolIndex)));
        itemList.append(QStandardItem(spoolModel.aisle));
        itemList.append(QStandardItem(QString(spoolModel.specimenType)));
        itemList.append(QStandardItem(QString(spoolModel.specimenNo)));
        itemList.append(QStandardItem(QString(spoolModel.ctMin)));
        itemList.append(QStandardItem(QString(spoolModel.ctMax)));
        itemList.append(QStandardItem(QString(spoolModel.datumMin)));
        itemList.append(QStandardItem(QString(spoolModel.datumMax)));
        itemList.append(QStandardItem(spoolModel.curveColor));
        modelMain->appendRow(itemList);
    }
}
