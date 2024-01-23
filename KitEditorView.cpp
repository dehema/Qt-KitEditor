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
    headerMain << QString::fromLocal8Bit("基线起点");
    headerMain << QString::fromLocal8Bit("基线终点");
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
    headerPositive << QString::fromLocal8Bit("探针染料");
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
    currFilePath = QFileDialog::getOpenFileName(this,QStringLiteral("选择配置文件"),KitMgr::ins().filePath,QStringLiteral("(*json);"));
    ui->lbPath->setText(currFilePath);
    if (!currFilePath.isEmpty())
    {
        QFile file(currFilePath);
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
    initMainTable();
    initPositiveTable();
    initMainTableData();
}

void KitEditorView::initMainTableData()
{
    for(int _row = 0;_row < kitData.spoolList.length();_row++)
    {
        SpoolModel spoolModel = kitData.spoolList[_row];
        SpoolModel spoolConfig =KitMgr::ins().kitConfig.spoolList[_row];
        //探针染料
        int abbrNameIndex = 0;
        QStringList fullNameStringList = spoolConfig.fullName.split("/");
        QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(_row, 1));
        for(int j = 0;j < fullNameStringList.length();j++)
        {
            QString str = fullNameStringList[j];
            cbAbbrName->addItem(str);
            if(str == spoolModel.abbrName)
                abbrNameIndex = j;
        }
        cbAbbrName->setCurrentIndex(abbrNameIndex);
        //指标类型
        QComboBox* cbSpecimenType = dynamic_cast<QComboBox*>(getWidgetMainTable(_row,2));
        cbSpecimenType->setCurrentIndex(spoolModel.specimenType);
    }
}

QWidget* KitEditorView::getWidgetMainTable(int _rowIndex,int _colIndex)
{
    if(_colIndex == 1)
        return ui->mainRoot->findChild<QComboBox*>(QString("cbAbbrName") + QString::number(_rowIndex));
    else if(_colIndex == 2)
        return ui->mainRoot->findChild<QComboBox*>(QString("cbSpecimenType") + QString::number(_rowIndex));
    else if(_colIndex == 3)
        return ui->mainRoot->findChild<QWidget*>(QString("editFullName") + QString::number(_rowIndex));
    else if(_colIndex == 4)
        return ui->mainRoot->findChild<QWidget*>(QString("editCtMin") + QString::number(_rowIndex));
    else if(_colIndex == 5)
        return ui->mainRoot->findChild<QWidget*>(QString("editCtMax") + QString::number(_rowIndex));
    else if(_colIndex == 6)
        return ui->mainRoot->findChild<QWidget*>(QString("editDatumMin") + QString::number(_rowIndex));
    else if(_colIndex == 7)
        return ui->mainRoot->findChild<QWidget*>(QString("editDatumMax") + QString::number(_rowIndex));
    return nullptr;
}

QWidget* KitEditorView::getWidgetPositiveTable(int _rowIndex,int _colIndex)
{
    if(_colIndex == 1)
        return ui->mainRoot->findChild<QLabel*>(QString("lbPositiveAbbrName") + QString::number(_rowIndex));
    else if(_colIndex == 2)
        return ui->mainRoot->findChild<QWidget*>(QString("editPositiveCtMin") + QString::number(_rowIndex));
    else if(_colIndex == 3)
        return ui->mainRoot->findChild<QWidget*>(QString("editPositiveCtMax") + QString::number(_rowIndex));
    return nullptr;
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
    for(int _row = 0;_row < kitData.spoolList.length();_row++)
    {
        SpoolModel spoolModel = kitData.spoolList[_row];
        SpoolModel spoolConfig =KitMgr::ins().kitConfig.spoolList[_row];
        //序号
        QLabel* lbIndex = new QLabel(QString::number(_row + 1));
        lbIndex->setAlignment(Qt::AlignCenter);
        lbIndex->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(_row,0), lbIndex);
        //探针染料
        QComboBox* cbAbbrName = new QComboBox(this);
        cbAbbrName->setObjectName(QString("cbAbbrName") + QString::number(_row));
        cbAbbrName->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(_row,1), cbAbbrName);
        connect(cbAbbrName,SIGNAL(currentIndexChanged(QString)),this,SLOT(slot_onAbbrNameTypeIndexChange(QString)));
        //指标类型
        QComboBox* cbSpecimenType = new QComboBox(this);
        cbSpecimenType->setObjectName(QString("cbSpecimenType") + QString::number(_row));
        for(QString str:KitMgr::ins().normTypeStr)
            cbSpecimenType->addItem(str);
        cbSpecimenType->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(_row,2), cbSpecimenType);
        connect(cbSpecimenType,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_onSpecimenTypeIndexChange(int)));
        //颜色
        QPushButton* btColor= new QPushButton(ui->mainTable);
        btColor->setObjectName(QString("btColor") + QString::number(_row));
        btColor->setStyleSheet(QString("background-color:rgba(%1,255);border:0px;border-radius:0px;margin:5px;").arg(spoolModel.curveColor));
        connect(btColor,SIGNAL(clicked()),this,SLOT(slot_onclickColor()));
        ui->mainTable->setIndexWidget(modelMain->index(_row,8), btColor);
    }
}

void KitEditorView::initSubMainTable(int _row,int _specimenType)
{
    SpoolModel spoolModel = kitData.spoolList[_row];
    SpoolModel spoolConfig =KitMgr::ins().kitConfig.spoolList[_row];
    QWidget* w3 = getWidgetMainTable(_row,3);
    if(w3 != nullptr)
        delete w3;
    QWidget* w4 = getWidgetMainTable(_row,4);
    if(w4 != nullptr)
        delete w4;
    QWidget* w5 = getWidgetMainTable(_row,5);
    if(w5 != nullptr)
        delete w5;
    QWidget* w6 = getWidgetMainTable(_row,6);
    if(w6 != nullptr)
        delete w6;
    QWidget* w7 = getWidgetMainTable(_row,7);
    if(w7 != nullptr)
        delete w7;
    if(_specimenType >= NormType::normal)
    {
        //指标
        QLineEdit* editFullName = new QLineEdit(this);
        editFullName->setObjectName(QString("editFullName") + QString::number(_row));
        QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(_row, 1));
        editFullName->setText(cbAbbrName->currentText());
        editFullName->setMaxLength(30);
        editFullName->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editFullName->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9_]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(_row, 3), editFullName);
        connect(editFullName,SIGNAL(textChanged(QString)),this,SLOT(slot_onFullNameTextChanged(QString)));
        //基线起点
        QLineEdit* editDatumMin = new QLineEdit(this);
        editDatumMin->setObjectName(QString("editDatumMin") + QString::number(_row));
        editDatumMin->setText(QString::number(spoolModel.datumMin));
        editDatumMin->setMaxLength(2);
        editDatumMin->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editDatumMin->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(_row, 6), editDatumMin);
        connect(editDatumMin,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
        //基线终点
        QLineEdit* editDatumMax = new QLineEdit(this);
        editDatumMax->setObjectName(QString("editDatumMax") + QString::number(_row));
        editDatumMax->setText(QString::number(spoolModel.datumMax));
        editDatumMax->setMaxLength(2);
        editDatumMax->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editDatumMax->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(_row, 7), editDatumMax);
        connect(editDatumMax,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
    }
    else
    {
        fillBlankMainTable(_row,3);
        fillBlankMainTable(_row,6);
        fillBlankMainTable(_row,7);
    }
    if(_specimenType >= NormType::internal)
    {
        //ct下限
        QLineEdit* editCtMin = new QLineEdit(this);
        editCtMin->setObjectName(QString("editCtMin") + QString::number(_row));
        editCtMin->setText(QString::number(spoolModel.ctMin));
        editCtMin->setMaxLength(2);
        editCtMin->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMin->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(_row, 4), editCtMin);
        connect(editCtMin,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
        //ct上限
        QLineEdit* editCtMax = new QLineEdit(this);
        editCtMax->setObjectName(QString("editCtMax") + QString::number(_row));
        editCtMax->setText(QString::number(spoolModel.ctMax));
        editCtMax->setMaxLength(2);
        editCtMax->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMax->setValidator(new QRegExpValidator(QRegExp("[a-zA-Z0-9]+$")));
        ui->mainTable->setIndexWidget(modelMain->index(_row, 5), editCtMax);
        connect(editCtMax,SIGNAL(textChanged(QString)),this,SLOT(slot_onCtTextChanged(QString)));
    }
    else
    {
        fillBlankMainTable(_row,4);
        fillBlankMainTable(_row,5);
    }
}

void KitEditorView::fillBlankMainTable(int _row,int _col)
{
    QLabel* lb = new QLabel(this);
    lb->setText("-");
    lb->setAlignment(Qt::AlignCenter);
    lb->setStyleSheet("border:0px;background-color:rgb(140, 147, 163);");
    ui->mainTable->setIndexWidget(modelMain->index(_row,_col), lb);
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
        //探针染料
        QLabel* lbAbbrName = new QLabel(this);
        QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(i, 1));
        lbAbbrName->setText(cbAbbrName->currentText());
        lbAbbrName->setObjectName(QString("lbPositiveAbbrName") + QString::number(i));
        lbAbbrName->setAlignment(Qt::AlignCenter);
        lbAbbrName->setStyleSheet("background-color:white;border:0px;");
        ui->positiveTable->setIndexWidget(modelPositive->index(i,1), lbAbbrName);
    }
}

void KitEditorView::initSubPositiveTable(int _row,int _specimenType)
{
    if(_specimenType >= NormType::normal)
    {
        SpoolModel spoolModel = kitData.spoolList[_row];
        //ct下限
        QLineEdit* editCtMin = new QLineEdit(this);
        editCtMin->setObjectName(QString("editPositiveCtMin") + QString::number(_row));
        editCtMin->setText(QString("<") + QString::number(spoolModel.ctMin));
        editCtMin->setMaxLength(3);
        editCtMin->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMin->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
        ui->positiveTable->setIndexWidget(modelPositive->index(_row,2), editCtMin);
        connect(editCtMin,SIGNAL(textChanged(QString)),this,SLOT(slot_onPositiveCtMinTextChanged(QString)));
        //ct上限
        QLineEdit* editCtMax = new QLineEdit(this);
        editCtMax->setObjectName(QString("editPositiveCtMax") + QString::number(_row));
        editCtMax->setText(QString(">") + QString::number(spoolModel.ctMax));
        editCtMax->setMaxLength(3);
        editCtMax->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMax->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
        ui->positiveTable->setIndexWidget(modelPositive->index(_row,3), editCtMax);
        connect(editCtMax,SIGNAL(textChanged(QString)),this,SLOT(slot_onPositiveCtMaxTextChanged(QString)));
    }
    else
    {
        fillBlankPositiveTable(_row,2);
        fillBlankPositiveTable(_row,3);
    }
}

void KitEditorView::fillBlankPositiveTable(int _row,int _col)
{
    QLabel* lb = new QLabel(this);
    lb->setText("-");
    lb->setAlignment(Qt::AlignCenter);
    lb->setStyleSheet("border:0px;background-color:rgb(140, 147, 163);");
    ui->positiveTable->setIndexWidget(modelPositive->index(_row,_col), lb);
}

void KitEditorView::slot_onAbbrNameTypeIndexChange(QString _str)
{
    QString indexStr = sender()->objectName().at(sender()->objectName().length()-1);
    int rowIndex = indexStr.toInt();
    //质控 指标
    QComboBox* cbPositiveAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(rowIndex, 1));
    QLabel* lbPositiveAbbrName = dynamic_cast<QLabel*>(getWidgetPositiveTable(rowIndex, 1));
    lbPositiveAbbrName->setText(cbPositiveAbbrName->currentText());
    //主表格 指标
    QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(rowIndex, 2));
    int specimenType = cbAbbrName->currentIndex();
    if(specimenType >= NormType::normal)
    {
        QLineEdit* edit = ui->mainRoot->findChild<QLineEdit*>(QString("editFullName") + QString::number(rowIndex));
        edit->setText(_str);
    }
}

void KitEditorView::slot_onSpecimenTypeIndexChange(int _specimenType)
{
    QString indexStr = sender()->objectName().at(sender()->objectName().length()-1);
    int rowIndex = indexStr.toInt();
    initSubMainTable(rowIndex, _specimenType);
    initSubPositiveTable(rowIndex, _specimenType);
}

void KitEditorView::slot_onFullNameTextChanged(QString _str)
{
    QString indexStr = sender()->objectName().at(sender()->objectName().length()-1);
    int index = indexStr.toInt();
    QLabel* label = ui->mainRoot->findChild<QLabel*>(QString("lbPositiveAbbrName") + QString::number(index));
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
    if(currFilePath.isEmpty())
        return;
    QString filePath = QFileDialog::getSaveFileName(this,QStringLiteral("选择配置文件"),KitMgr::ins().filePath,QStringLiteral("(*json);"));
    if(filePath.isEmpty())
        return;
    if(QFileInfo(filePath).suffix() != "json")
        filePath+=".json";
    QFile file(filePath);
    KitModel kitModel = getPublishKitModel();
    QString str = KitMgr::ins().getKitJsonStr(kitModel);
    qDebug()<<str;
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
        SpoolModel spool;
        SpoolModel spoolConfig = KitMgr::ins().kitConfig.spoolList[i];
        //
        spool.copy(kitData.spoolList[i]);
        QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(ui->mainTable->indexWidget(modelMain->index(i,1)));
        spool.abbrName = cbAbbrName->currentText();
        QComboBox* cbSpecimenType = dynamic_cast<QComboBox*>(ui->mainTable->indexWidget(modelMain->index(i,2)));
        spool.specimenType = cbSpecimenType->currentIndex();
        if(spool.specimenType >= NormType::normal)
        {
            QLineEdit* editFullName = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,3)));
            spool.fullName = editFullName->text();
            QLineEdit* editDatumMin = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,6)));
            spool.datumMin = editDatumMin->text().toInt();
            QLineEdit* editDatumMax = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,7)));
            spool.datumMax = editDatumMax->text().toInt();
        }
        else
        {
            spool.fullName = spool.abbrName;
            spool.datumMin = spoolConfig.datumMin;
            spool.datumMax = spoolConfig.datumMax;
        }
        if(spool.specimenType >= NormType::internal)
        {
            QLineEdit* editCtMin = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,4)));
            spool.ctMin = editCtMin->text().toInt();
            QLineEdit* editCtMax = dynamic_cast<QLineEdit*>(ui->mainTable->indexWidget(modelMain->index(i,5)));
            spool.ctMax = editCtMax->text().toInt();
        }
        else
        {
            spool.ctMin = spoolConfig.ctMin;
            spool.ctMax = spoolConfig.ctMax;
        }
        kitModel.spoolList.append(spool);

        //
        SpoolModel spoolPositive;
        spoolPositive.copy(spool);
        if(spool.specimenType >= NormType::normal)
        {
            QLineEdit* editPosiCtMax = dynamic_cast<QLineEdit*>(ui->positiveTable->indexWidget(modelPositive->index(i,2)));
            spoolPositive.ctMax = editPosiCtMax->text().toInt();
        }
        else
        {
            spoolPositive.ctMax = spoolConfig.ctMax;
        }
        kitModel.positiveSpoolList.append(spoolPositive);
        SpoolModel spoolNegative;
        spoolNegative.copy(spool);
        if(spool.specimenType >= NormType::normal)
        {
            QLineEdit* editNegaCtMin = dynamic_cast<QLineEdit*>(ui->positiveTable->indexWidget(modelPositive->index(i,3)));
            spoolNegative.ctMin = editNegaCtMin->text().toInt();
        }
        else
        {
            spoolNegative.ctMin = spoolConfig.ctMin;
        }
        kitModel.negativeSpoolList.append(spoolNegative);
    }
    //排序
    KitMgr::ins().sortKitByPoolIndex(kitModel);
    return kitModel;
}
