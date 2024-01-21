#include "kiteditorview.h"
#include "ui_kiteditorview.h"

KitEditorView::KitEditorView(QString _kitFileParams,QString _tcpParams, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KitEditor)
{
    ui->setupUi(this);
    kitFileParams = KitMgr::ins().getKitEditorParams(_kitFileParams);
    tcpParams = KitMgr::ins().getKitEditorParams(_tcpParams);

    kitFileParams.preview = "E015,REF1066547_E015";
    KitEditorParamsList list1;
    list1.map["REF1060167_E015"] = 0;
    list1.map["REF1066547_E015"] = 1;
    list1.map["REF1561661_E015"] = 0;
    kitFileParams.list["E015"] = list1;
    KitEditorParamsList list2;
    list2.map["REF1041231_E0161"] = 0;
    list2.map["REF1041231_E0162"] = 1;
    list2.map["REF1067891_E016"] = 0;
    kitFileParams.list["E016"] = list2;

    KitEditorParamsList tcpList1;
    tcpList1.map["XC1"] = 0;
    tcpList1.map["XC2"] = 0;
    tcpList1.map["XC3"] = 0;
    tcpParams.list["E015"] = tcpList1;
    KitEditorParamsList tcpList2;
    tcpList2.map["XC1"] = 0;
    tcpList2.map["XC2"] = 0;
    tcpList2.map["XC3"] = 0;
    tcpParams.list["E016"] = tcpList2;

    machineID = kitFileParams.preview.split(",")[0];

    //UI
    ui->editKitName->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));

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
    ui->mainTable->horizontalHeader()->setMinimumHeight(45);


    modelPositive = new QStandardItemModel();
    QStringList headerPositive;
    headerPositive << QString::fromLocal8Bit("序号");
    headerPositive << QString::fromLocal8Bit("指标");
    headerPositive << QString::fromLocal8Bit("阳性质控ct上限");
    headerPositive << QString::fromLocal8Bit("阴性质控ct下限");
    modelPositive->setHorizontalHeaderLabels(headerPositive);
    ui->positiveTable->setModel(modelPositive);
    ui->positiveTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->positiveTable->resizeRowsToContents();
    ui->positiveTable->resizeColumnsToContents();
    ui->positiveTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->positiveTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->positiveTable->setSelectionMode (QAbstractItemView::SingleSelection);
    ui->positiveTable->verticalHeader()->setHidden(true);
    ui->positiveTable->horizontalHeader()->setMinimumHeight(45);
}

KitEditorView::~KitEditorView()
{
    delete ui;
}

void KitEditorView::on_btOpenFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(this,QStringLiteral("选择配置文件"),KitMgr::ins().filePath,QStringLiteral("(*json);"));
    ui->lbPath->setText(filePath);
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
    kitData = KitMgr::ins().getProcedureListByData(_byteArray);
    ui->editKitName->setText(kitData.abbrName);
    ui->editFullName->setText(kitData.fullName);
    ui->editDosage->setText(QString::number(kitData.dosage));
    QStringList kitFileList = kitFileParams.list.keys();
    for(QString key:kitFileList)
    {
        ui->cbMachineType->addItem(key);
    }
    QMap<QString,int> tcpList = tcpParams.list.value(machineID).map;
    for(QString key:tcpList.keys())
    {
        ui->cbTCPType->addItem(key);
    }
    initPositiveTable();
    initMainTable();
    initSubMainTable();
    initSubPositiveTable();
}

void KitEditorView::initMainTable()
{
    while (modelMain->rowCount() > 0) {
        modelMain->removeRow(0);
    }
    modelMain->setRowCount(kitData.spoolList.length());
    for(int rowIndex = 0;rowIndex < modelMain->rowCount();rowIndex++)
    {
        ui->mainTable->setRowHeight(rowIndex,35);
    }
    for(int i = 0;i<kitData.spoolList.length();i++)
    {
        SpoolModel spoolModel = kitData.spoolList[i];
        SpoolModel spoolConfig =KitMgr::ins().kitConfig.spoolList[i];
        //序号
        QLabel* lbIndex = new QLabel(QString::number(i + 1));
        lbIndex->setAlignment(Qt::AlignCenter);
        lbIndex->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(i,0), lbIndex);
        //探针染料
        QComboBox* cbAbbrName = new QComboBox(this);
        cbAbbrName->setObjectName(QString("cbAbbrName") + QString::number(i));
        int abbrNameIndex = 0;
        QStringList fullNameStringList = spoolConfig.fullName.split("/");
        for(int j = 0;j < fullNameStringList.length();j++)
        {
            QString str = fullNameStringList[j];
            cbAbbrName->addItem(str);
            if(str == spoolModel.abbrName)
                abbrNameIndex = j;
        }
        cbAbbrName->setCurrentIndex(abbrNameIndex);
        cbAbbrName->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(i,1), cbAbbrName);
        connect(cbAbbrName,SIGNAL(currentIndexChanged(QString)),this,SLOT(slot_onAbbrNameTypeIndexChange(QString)));
        //指标类型
        QComboBox* cbSpecimenType = new QComboBox(this);
        cbSpecimenType->setObjectName(QString("cbSpecimenType") + QString::number(i));
        for(QString str:KitMgr::ins().normTypeStr)
            cbSpecimenType->addItem(str);
        cbSpecimenType->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(i,2), cbSpecimenType);
        connect(cbSpecimenType,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_onSpecimenTypeIndexChange(int)));
        //指标
        QLineEdit* editFullName = new QLineEdit(this);
        editFullName->setObjectName(QString("editFullName") + QString::number(i));
        editFullName->setMaxLength(30);
        editFullName->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editFullName->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9_]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(i,3), editFullName);
        connect(editFullName,SIGNAL(textChanged(QString)),this,SLOT(slot_onFullNameTextChanged(QString)));
        editFullName->setText(spoolModel.fullName);
    }
}

void KitEditorView::initSubMainTable()
{
    for(int i = 0;i<kitData.spoolList.length();i++)
    {
        SpoolModel spoolModel = kitData.spoolList[i];
        SpoolModel spoolConfig =KitMgr::ins().kitConfig.spoolList[i];
        //ct下限
        QLineEdit* editCtMin = new QLineEdit(this);
        editCtMin->setObjectName(QString("editCtMin") + QString::number(i));
        editCtMin->setText(QString::number(spoolModel.ctMin));
        editCtMin->setMaxLength(2);
        editCtMin->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMin->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(i,4), editCtMin);
        connect(editCtMin,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
        //ct上限
        QLineEdit* editCtMax = new QLineEdit(this);
        editCtMax->setObjectName(QString("editCtMax") + QString::number(i));
        editCtMax->setText(QString::number(spoolModel.ctMax));
        editCtMax->setMaxLength(2);
        editCtMax->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMax->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(i,5), editCtMax);
        connect(editCtMax,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
        //基线下限
        QLineEdit* editDatumMin = new QLineEdit(this);
        editDatumMin->setObjectName(QString("editDatumMin") + QString::number(i));
        editDatumMin->setText(QString::number(spoolModel.datumMin));
        editDatumMin->setMaxLength(2);
        editDatumMin->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editDatumMin->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(i,6), editDatumMin);
        connect(editDatumMin,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
        //基线上限
        QLineEdit* editDatumMax = new QLineEdit(this);
        editDatumMax->setObjectName(QString("editDatumMax") + QString::number(i));
        editDatumMax->setText(QString::number(spoolModel.datumMax));
        editDatumMax->setMaxLength(2);
        editDatumMax->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editDatumMax->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(i,7), editDatumMax);
        connect(editDatumMax,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
        //颜色
        QPushButton* btColor= new QPushButton(ui->mainTable);
        btColor->setObjectName(QString("btColor") + QString::number(i));
        btColor->setStyleSheet(QString("background-color:rgba(%1,255);border:0px;border-radius:0px;").arg(spoolModel.curveColor));
        connect(btColor,SIGNAL(clicked()),this,SLOT(slot_onclickColor()));
        ui->mainTable->setIndexWidget(modelMain->index(i,8), btColor);
        //data
//        ui->cbSpecimenType->setCurrentIndex(spoolModel.specimenType);
    }
}

void KitEditorView::initPositiveTable()
{
    while (modelPositive->rowCount() > 0) {
        modelPositive->removeRow(0);
    }
    modelPositive->setRowCount(kitData.spoolList.length());
    for(int rowIndex = 0;rowIndex < modelPositive->rowCount();rowIndex++)
    {
        ui->positiveTable->setRowHeight(rowIndex,35);
    }
    for(int i = 0;i<kitData.spoolList.length();i++)
    {
        SpoolModel spoolModel = kitData.spoolList[i];
        //序号
        QLabel* lbIndex = new QLabel(QString::number(i + 1));
        lbIndex->setAlignment(Qt::AlignCenter);
        lbIndex->setStyleSheet("background-color:white;border:0px;");
        ui->positiveTable->setIndexWidget(modelPositive->index(i,0), lbIndex);
        //指标
        QLabel* lbFullName = new QLabel(this);
        lbFullName->setObjectName(QString("lbPositiveFullName") + QString::number(i));
        lbFullName->setText(spoolModel.fullName);
        lbFullName->setAlignment(Qt::AlignCenter);
        lbFullName->setStyleSheet("background-color:white;border:0px;");
        ui->positiveTable->setIndexWidget(modelPositive->index(i,1), lbFullName);
    }
}

void KitEditorView::initSubPositiveTable()
{
    for(int i = 0;i<kitData.spoolList.length();i++)
    {
        SpoolModel spoolModel = kitData.spoolList[i];
        //ct下限
        QLineEdit* editCtMin = new QLineEdit(this);
        editCtMin->setObjectName(QString("editPositiveCtMin") + QString::number(i));
        editCtMin->setText(QString("<") + QString::number(spoolModel.ctMin));
        editCtMin->setMaxLength(3);
        editCtMin->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMin->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
        ui->positiveTable->setIndexWidget(modelPositive->index(i,2), editCtMin);
        connect(editCtMin,SIGNAL(textChanged(QString)),this,SLOT(slot_onPositiveCtMinTextChanged(QString)));
        //ct上限
        QLineEdit* editCtMax = new QLineEdit(this);
        editCtMax->setObjectName(QString("editPositiveCtMax") + QString::number(i));
        editCtMax->setText(QString(">") + QString::number(spoolModel.ctMax));
        editCtMax->setMaxLength(3);
        editCtMax->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMax->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
        ui->positiveTable->setIndexWidget(modelPositive->index(i,3), editCtMax);
        connect(editCtMax,SIGNAL(textChanged(QString)),this,SLOT(slot_onPositiveCtMaxTextChanged(QString)));
    }
}

void KitEditorView::slot_onAbbrNameTypeIndexChange(QString _str)
{
    QString indexStr = sender()->objectName().at(sender()->objectName().length()-1);
    int index = indexStr.toInt();
    QLineEdit* edit = ui->mainRoot->findChild<QLineEdit*>(QString("editFullName") + QString::number(index));
    edit->setText(_str);
}

void KitEditorView::slot_onSpecimenTypeIndexChange(int _index)
{
    QString indexStr = sender()->objectName().at(sender()->objectName().length()-1);
    int index = indexStr.toInt();
    QLineEdit* editCtMin = ui->mainRoot->findChild<QLineEdit*>(QString("editCtMin") + QString::number(index));
    QLineEdit* editCtMax = ui->mainRoot->findChild<QLineEdit*>(QString("editCtMax") + QString::number(index));
    if(_index == NormType::unDefine)
    {
    }
    else if(_index == NormType::normal)
    {
        editCtMin->setText("-");
        editCtMin->setVisible(false);
        editCtMin->setAlignment(Qt::AlignCenter);
        editCtMax->setText("-");
        editCtMax->setVisible(false);
        editCtMax->setAlignment(Qt::AlignCenter);
    }
    else if(_index == NormType::internal)
    {
        editCtMin->setVisible(true);
        editCtMin->setAlignment(Qt::AlignLeft);
        editCtMax->setVisible(true);
        editCtMax->setAlignment(Qt::AlignLeft);
    }
}

void KitEditorView::slot_onFullNameTextChanged(QString _str)
{
    QString indexStr = sender()->objectName().at(sender()->objectName().length()-1);
    int index = indexStr.toInt();
    QLabel* label = ui->mainRoot->findChild<QLabel*>(QString("lbPositiveFullName") + QString::number(index));
    label->setText(_str);
}

void KitEditorView::slot_onCtTextChanged(QString _str)
{
    QLineEdit* edit = ui->mainRoot->findChild<QLineEdit*>(sender()->objectName());
    int ctVal = _str.toInt();
    if(ctVal > 45)
        ctVal = 45;
    edit->setText(QString::number(ctVal));
}

void KitEditorView::slot_onPositiveCtMinTextChanged(QString _str)
{
    QLineEdit* edit = ui->mainRoot->findChild<QLineEdit*>(sender()->objectName());
    int ctVal = _str.replace("<","").toInt();
    if(ctVal > 45)
        ctVal = 45;
    edit->setText(QString("<") + QString::number(ctVal));
}

void KitEditorView::slot_onPositiveCtMaxTextChanged(QString _str)
{
    QLineEdit* edit = ui->mainRoot->findChild<QLineEdit*>(sender()->objectName());
    int ctVal = _str.replace(">","").toInt();
    if(ctVal > 45)
        ctVal = 45;
    edit->setText(QString(">") + QString::number(ctVal));
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

void KitEditorView::on_btCreatKit_clicked()
{

}

void KitEditorView::on_btCheckInfo_clicked()
{

}

void KitEditorView::on_btPushlish_clicked()
{

}

void KitEditorView::on_btSaveAs_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this,QStringLiteral("选择配置文件"),KitMgr::ins().filePath,QStringLiteral("(*json);"));
    if(filePath.isEmpty())
        return;
    if(QFileInfo(filePath).suffix() != "json")
        filePath+=".json";
    QFile file(filePath);
    KitModel kitModel = getPublishKitModel();
    QString str = KitMgr::ins().getKitJsonStr(kitModel);
    string str1 = str.toStdString();
    const char* ch = str1.data();
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(ch);
        file.close();
//        JustShowWarningMsg(QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("备份成功"));
    }
}

KitModel KitEditorView::getPublishKitModel()
{
    KitModel kitModel;
    kitModel.abbrName = ui->editKitName->text();
    kitModel.barcode = KitMgr::ins().kitConfig.barcode;
    kitModel.dosage = ui->editDosage->text().toInt();
    kitModel.filetype = KitMgr::ins().kitConfig.filetype;
    kitModel.fullName = ui->editFullName->text();
    kitModel.distillFile = KitMgr::ins().kitConfig.distillFile;
    kitModel.ampFile = KitMgr::ins().kitConfig.ampFile;
    for(int i = 0;i < modelMain->rowCount();i++)
    {
        //
        SpoolModel spool;
        spool.copy(kitData.spoolList[i]);
        QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(ui->mainTable->indexWidget(modelMain->index(i,1)));
        spool.abbrName = cbAbbrName->currentText();
        QComboBox* cbSpecimenType = dynamic_cast<QComboBox*>(ui->mainTable->indexWidget(modelMain->index(i,2)));
        spool.specimenType = cbSpecimenType->currentIndex();
        QLineEdit* editFullName = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,3)));
        spool.fullName = editFullName->text();
        QLineEdit* editCtMin = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,4)));
        spool.ctMin = editCtMin->text().toInt();
        QLineEdit* editCtMax = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,5)));
        spool.ctMax = editCtMax->text().toInt();
        QLineEdit* editDatumMin = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,6)));
        spool.datumMin = editDatumMin->text().toInt();
        QLineEdit* editDatumMax = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,7)));
        spool.datumMax = editDatumMax->text().toInt();
        kitModel.spoolList.append(spool);
        //
        SpoolModel spoolPositive;
        spoolPositive.copy(spool);
        kitModel.positiveSpoolList.append(spoolPositive);
        SpoolModel spoolNegative;
        spoolNegative.copy(spool);
        kitModel.negativeSpoolList.append(spoolNegative);
    }
    KitMgr::ins().sortKitByPoolIndex(kitModel);
    return kitModel;
}
