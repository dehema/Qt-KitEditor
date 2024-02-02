#include "kiteditorview.h"
#include "kitcreatedialog.h"
#include "kitchoosedialog.h"
#include "kitcolordialog.h"
#include "ui_kiteditorview.h"

KitEditorView::KitEditorView(QString _kitFileParams,QString _tcpParams, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KitEditor)
{
    setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    ui->setupUi(this);
    kitFileParams = KitMgr::ins().getKitEditorParams(_kitFileParams);
    tcpParams = KitMgr::ins().getKitEditorParams(_tcpParams);

    kitFileParams.preview = "E015,REF1060167E015";
    KitEditorParamsList list1;
    list1.map["REF1060167E015"] = 0;
    list1.map["REF1060168E015"] = 1;
    list1.map["REF1060169E015"] = 0;
    kitFileParams.list["E015"] = list1;
    KitEditorParamsList list2;
    list2.map["REF1060167E015"] = 0;
    list2.map["REF1060168E015"] = 1;
    list2.map["REF1060169E015"] = 0;
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

    KitMgr::ins().machineID = kitFileParams.preview.split(",")[0];
    KitMgr::ins().kitName = kitFileParams.preview.split(",")[1];
    KitMgr::ins().loadKitConfig();

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


    QStringList kitFileList = kitFileParams.list.keys();
    for(QString key:kitFileList)
    {
        ui->cbMachineType->addItem(key);
    }
    QMap<QString,int> tcpList = tcpParams.list.value(KitMgr::ins().machineID).map;
    //温控协议
    for(QString key:tcpList.keys())
    {
        ui->cbAmpFile->addItem(key);
    }
    //协议列表 有一个就选唯一那个 没有或者有多个就增加一个空选中这个空
    if(ui->cbAmpFile->count() == 0)
        ui->cbAmpFile->insertItem(0,"");
    QString openFilePath = KitMgr::ins().getPublishPath() + KitMgr::ins().kitName + ".json";
    openKitFile(openFilePath);
}

KitEditorView::~KitEditorView()
{
    delete ui;
}

void KitEditorView::on_btOpenFile_clicked()
{
    kitChooseDialog* dialog = new kitChooseDialog(this,kitFileParams.list);
    connect(dialog,SIGNAL(signal_onChooseKit(QString,QString)),this,SLOT(slot_onChooseKit(QString,QString)));
    connect(dialog,SIGNAL(signal_onBrowserKit(QString)),this,SLOT(slot_onBrowserKit(QString)));
    dialog->setWindowModality(Qt::WindowModal);
    dialog->setFocus();
    dialog->exec();
}

void KitEditorView::openKitFile(QString _path)
{
    QFile file(_path);
    if(file.open(QFile::ReadOnly))
    {
        QByteArray byteArray = file.readAll();
        onLoadJsonFile(byteArray);
        setCurrFilePath(_path);
        file.close();
    }
    else{
        qDebug()<<"打开文件失败";
    }
}

void KitEditorView::onLoadJsonFile(QByteArray _byteArray)
{
    kitData = KitMgr::ins().getProcedureListByData(_byteArray);
    //如果温控协议列表没有这个选项则增加
    int ampIndex = ui->cbAmpFile->findText(kitData.ampFile);
    if(ampIndex==-1)
    {
        ui->cbAmpFile->addItem(kitData.ampFile);
        ui->cbAmpFile->setCurrentIndex(ui->cbAmpFile->count()-1);
    }
    KitMgr::ins().sortPoolByConfigSpecimenNo(kitData);
    refreshUI();
}

void KitEditorView::refreshUI()
{
    ui->editKitName->setText(kitData.abbrName);
    ui->editFullName->setText(kitData.fullName);
    ui->editDosage->setText(QString::number(kitData.dosage));
    int ampIndex = ui->cbAmpFile->findText(kitData.ampFile);
    if(ampIndex>=0)
        ui->cbAmpFile->setCurrentIndex(ampIndex);
    ui->cbAmpFile->setCurrentText(kitData.ampFile);
    int machineIndex = ui->cbMachineType->findText(kitData.filetype);
    machineIndex = qMax(0,machineIndex);
    ui->cbMachineType->setCurrentIndex(machineIndex);
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
        QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(_row, 1));
        onAbbrNameTypeIndexChange(_row, cbAbbrName->currentText());
        //指标类型
        QComboBox* cbSpecimenType = dynamic_cast<QComboBox*>(getWidgetMainTable(_row,2));
        cbSpecimenType->setCurrentIndex(spoolModel.specimenType);
        connect(cbSpecimenType,SIGNAL(currentIndexChanged(int)),this,SLOT(slot_onSpecimenTypeIndexChange(int)));
        onSpecimenTypeIndexChange(_row, cbSpecimenType->currentIndex());
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
    else if(_colIndex == 8)
        return ui->mainRoot->findChild<QPushButton*>(QString("btColor") + QString::number(_rowIndex));
    return nullptr;
}

QWidget* KitEditorView::getWidgetPositiveTable(int _rowIndex,int _colIndex)
{
    if(_colIndex == 1)
        return ui->mainRoot->findChild<QLabel*>(QString("lbPositiveAbbrName") + QString::number(_rowIndex));
    else if(_colIndex == 2)
        return ui->mainRoot->findChild<QWidget*>(QString("editPositiveCtMax") + QString::number(_rowIndex));
    else if(_colIndex == 3)
        return ui->mainRoot->findChild<QWidget*>(QString("editPositiveCtMin") + QString::number(_rowIndex));
    return nullptr;
}

void KitEditorView::initMainTable()
{
//    while (modelMain->rowCount() > 0) {
//        modelMain->removeRow(0);
//    }
    if(modelMain->rowCount()==0)
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
        QWidget* w0 = getWidgetMainTable(_row,0);
        if(w0 == nullptr)
        {
            QLabel* lbIndex = new QLabel(QString::number(_row + 1));
            lbIndex->setAlignment(Qt::AlignCenter);
            lbIndex->setStyleSheet("background-color:white;border:0px;");
            ui->mainTable->setIndexWidget(modelMain->index(_row,0), lbIndex);
        }
        //探针染料
        QWidget* w1 = getWidgetMainTable(_row,1);
        QComboBox* cbAbbrName;
        bool isNewAbbrName = false;
        if(w1 == nullptr)
        {
            isNewAbbrName = true;
            cbAbbrName = new QComboBox(this);
            cbAbbrName->setObjectName(QString("cbAbbrName") + QString::number(_row));
            cbAbbrName->setStyleSheet("background-color:white;border:0px;");
            ui->mainTable->setIndexWidget(modelMain->index(_row,1), cbAbbrName);
        }
        else
        {
            cbAbbrName = dynamic_cast<QComboBox*>(w1);
            disconnect(cbAbbrName,SIGNAL(currentIndexChanged(QString)),this,SLOT(slot_onAbbrNameTypeIndexChange(QString)));
        }
        int abbrNameIndex = 0;
        QStringList fullNameStringList = spoolConfig.fullName.split("/");
        cbAbbrName->clear();
        for(int j = 0;j < fullNameStringList.length();j++)
        {
            QString str = fullNameStringList[j];
            cbAbbrName->addItem(str);
            if(str == spoolModel.abbrName)
                abbrNameIndex = j;
        }
        cbAbbrName->setCurrentIndex(abbrNameIndex);
        connect(cbAbbrName,SIGNAL(currentIndexChanged(QString)),this,SLOT(slot_onAbbrNameTypeIndexChange(QString)));
        //指标类型
        QWidget* w2 = getWidgetMainTable(_row,2);
        if(w2 != nullptr)
            delete w2;
        QComboBox* cbSpecimenType = new QComboBox(this);
        cbSpecimenType->setObjectName(QString("cbSpecimenType") + QString::number(_row));
        for(QString str:KitMgr::ins().normTypeStr)
            cbSpecimenType->addItem(str);
        cbSpecimenType->setStyleSheet("background-color:white;border:0px;");
        ui->mainTable->setIndexWidget(modelMain->index(_row,2), cbSpecimenType);
        //颜色
        QPushButton* btColor;
        QWidget* w8 = getWidgetMainTable(_row,8);
        if(w8 == nullptr)
        {
            btColor = new QPushButton(ui->mainTable);
            btColor->setObjectName(QString("btColor") + QString::number(_row));
            btColor->setStyleSheet(QString("background-color:rgba(%1,255);border:0px;border-radius:0px;margin:5px;").arg(spoolModel.curveColor));
            connect(btColor,SIGNAL(clicked()),this,SLOT(slot_onclickSpoolColor()));
            ui->mainTable->setIndexWidget(modelMain->index(_row,8), btColor);
        }
        else
            btColor = dynamic_cast<QPushButton*>(w8);
        btColor->setStyleSheet(QString("background-color:rgba(%1,255);border:0px;border-radius:0px;margin:5px;").arg(spoolModel.curveColor));
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
//    while (modelPositive->rowCount() > 0) {
//        modelPositive->removeRow(0);
//    }
    if(modelPositive->rowCount()==0)
        modelPositive->setRowCount(kitData.spoolList.length());
    for(int rowIndex = 0;rowIndex < modelPositive->rowCount();rowIndex++)
    {
        ui->positiveTable->setRowHeight(rowIndex,35);
    }
    for(int row = 0;row<kitData.spoolList.length();row++)
    {
        SpoolModel spoolModel = kitData.spoolList[row];
        //序号
        QWidget* w0 = getWidgetPositiveTable(row,0);
        if(w0 == nullptr)
        {
            QLabel* lbIndex = new QLabel(QString::number(row + 1));
            lbIndex->setAlignment(Qt::AlignCenter);
            lbIndex->setStyleSheet("background-color:white;border:0px;");
            ui->positiveTable->setIndexWidget(modelPositive->index(row,0), lbIndex);
        }
        //探针染料
        QWidget* w1 = getWidgetPositiveTable(row,1);
        if(w1 == nullptr)
        {
            QLabel* lbPositiveAbbrName = new QLabel(this);
            lbPositiveAbbrName->setObjectName(QString("lbPositiveAbbrName") + QString::number(row));
            lbPositiveAbbrName->setAlignment(Qt::AlignCenter);
            lbPositiveAbbrName->setStyleSheet("background-color:white;border:0px;");
            ui->positiveTable->setIndexWidget(modelPositive->index(row,1), lbPositiveAbbrName);
        }
    }
}

void KitEditorView::initSubPositiveTable(int _row,int _specimenType)
{
    QWidget* w2 = getWidgetPositiveTable(_row,2);
    if(w2 != nullptr)
        delete w2;
    QWidget* w3 = getWidgetPositiveTable(_row,3);
    if(w3 != nullptr)
        delete w3;
    if(_specimenType >= NormType::normal)
    {
        SpoolModel positiveSpool = kitData.positiveSpoolList[_row];
        SpoolModel negativeSpool = kitData.negativeSpoolList[_row];
        //阳性质控ct上限
        QLineEdit* editCtMax = new QLineEdit(this);
        editCtMax->setObjectName(QString("editPositiveCtMax") + QString::number(_row));
        editCtMax->setText(QString("<") + QString::number(positiveSpool.ctMax));
        editCtMax->setMaxLength(3);
        editCtMax->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMax->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
        ui->positiveTable->setIndexWidget(modelPositive->index(_row,2), editCtMax);
        connect(editCtMax,SIGNAL(textChanged(QString)),this,SLOT(slot_onPositiveCtMaxTextChanged(QString)));
        //阴性质控ct下限
        QLineEdit* editCtMin = new QLineEdit(this);
        editCtMin->setObjectName(QString("editPositiveCtMin") + QString::number(_row));
        editCtMin->setText(QString(">") + QString::number(negativeSpool.ctMin));
        editCtMin->setMaxLength(3);
        editCtMin->setStyleSheet("background-color:white;border:0px;padding-left:5px;padding-right:5px;");
        editCtMin->setValidator(new QRegExpValidator(QRegExp("[0-9]+$")));
        ui->positiveTable->setIndexWidget(modelPositive->index(_row,3), editCtMin);
        connect(editCtMin,SIGNAL(textChanged(QString)),this,SLOT(slot_onPositiveCtMinTextChanged(QString)));
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
    QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(rowIndex,1));
    onAbbrNameTypeIndexChange(rowIndex, cbAbbrName->currentText());
}

void KitEditorView::onAbbrNameTypeIndexChange(int _rowIndex, QString _str)
{
    //探针染料 质控
    QComboBox* cbPositiveAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(_rowIndex, 1));
    QString abbrName = cbPositiveAbbrName->currentText();
    QLabel* lbPositiveAbbrName = dynamic_cast<QLabel*>(getWidgetPositiveTable(_rowIndex, 1));
    lbPositiveAbbrName->setText(abbrName);
    //主表格 指标
    QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(getWidgetMainTable(_rowIndex, 2));
    int specimenType = cbAbbrName->currentIndex();
    if(specimenType >= NormType::normal)
    {
        QLineEdit* edit = ui->mainRoot->findChild<QLineEdit*>(QString("editFullName") + QString::number(_rowIndex));
        edit->setText(_str);
    }
}

void KitEditorView::slot_onSpecimenTypeIndexChange(int _specimenType)
{
    QString indexStr = sender()->objectName().at(sender()->objectName().length()-1);
    int rowIndex = indexStr.toInt();
    onSpecimenTypeIndexChange(rowIndex,_specimenType);
}

void KitEditorView::onSpecimenTypeIndexChange(int _rowIndex,int _specimenType)
{
    SpoolModel spoolConfig = KitMgr::ins().kitConfig.spoolList[_rowIndex];
    SpoolModel spoolData = kitData.spoolList[_rowIndex];
    if(_specimenType <= NormType::normal)
    {
        spoolData.ctMin = spoolConfig.ctMin;
        spoolData.ctMax = spoolConfig.ctMax;
    }
    if(_specimenType <= NormType::unDefine)
    {
        spoolData.datumMin = spoolConfig.datumMin;
        spoolData.datumMax = spoolConfig.datumMax;
    }
    initSubMainTable(_rowIndex, _specimenType);
    initSubPositiveTable(_rowIndex, _specimenType);
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

void KitEditorView::slot_onclickSpoolColor()
{
    QString name = sender()->objectName();
    QChar charName = name.at(name.length()-1);
    QString strIndex = QString(charName);
    int index = strIndex.toInt();
    kitColorDialog* dialog = new kitColorDialog(this,index);
    connect(dialog,SIGNAL(signal_onChooseColor(int,QString)),this,SLOT(slot_onChooseColor(int,QString)));
    dialog->exec();
}

void KitEditorView::slot_onChooseColor(int _spoolIndex, QString _spoolColor)
{
    kitData.spoolList[_spoolIndex].curveColor = _spoolColor;
    QPushButton* bt = dynamic_cast<QPushButton*>(getWidgetMainTable(_spoolIndex,8));
    bt->setStyleSheet(QString("background-color:rgba(%1,255);border:0px;border-radius:0px;margin:5px;").arg(_spoolColor));
}

void KitEditorView::on_btCreatKit_clicked()
{
    QStringList strList;
    for(QString str:kitFileParams.list.keys())
    {
        strList.append(str);
    }
    kitCreateDialog* dialog = new kitCreateDialog(this,strList);
    connect(dialog,SIGNAL(signal_onCreateNewKit(QString)),this,SLOT(slot_onCreateNewKit(QString)));
    dialog->exec();
}

void KitEditorView::slot_onCreateNewKit(QString _str)
{
    KitMgr::ins().machineID = _str;
    KitMgr::ins().loadKitConfig();
    kitData.copy(KitMgr::ins().kitConfig);
    refreshUI();
    ui->editKitName->setText(QString());
}

void KitEditorView::slot_onChooseKit(QString _machineID,QString _fileName)
{
    KitMgr::ins().machineID = _machineID;
    QString openFilePath = KitMgr::ins().getPublishPath() + _fileName + ".json";
    openKitFile(openFilePath);
}

void KitEditorView::slot_onBrowserKit(QString _path)
{
    openKitFile(_path);
}

void KitEditorView::on_btCheckInfo_clicked()
{
    checkKitTips(getPublishKitModel());
}

bool KitEditorView::checkKitTips(KitModel _kitModel)
{
    QList<QString> tips = KitMgr::ins().checkKitTips(_kitModel);
    for(QString tip:tips)
        qDebug()<<tip;
    return tips.isEmpty();
}

void KitEditorView::on_btPushlish_clicked()
{
    QFile file(currFilePath);
    KitModel kitModel = getPublishKitModel();
    if(!checkKitTips(kitModel))
        return;
    QString str = KitMgr::ins().getKitJsonStr(kitModel);
    string str1 = str.toStdString();
    const char* ch = str1.data();
    if(file.open(QIODevice::WriteOnly))
    {
        file.write(ch);
        file.close();
    //        JustShowWarningMsg(QString::fromLocal8Bit("提示"),QString::fromLocal8Bit("发布成功"));
        qDebug()<<QString::fromLocal8Bit("发布至") + currFilePath;
        kitFileParams.list[KitMgr::ins().machineID].map[kitModel.abbrName] = 0;
    }
    else{
        qDebug()<<QString::fromLocal8Bit("发布失败");
    }
}

void KitEditorView::on_btSaveAs_clicked()
{
    if(currFilePath.isEmpty())
        return;
    QString filePath = QFileDialog::getSaveFileName(this,QStringLiteral("另存为"),QString(),QStringLiteral("(*json);"));
    if(filePath.contains(QCoreApplication::applicationDirPath()))
    {
        qDebug()<<QString::fromLocal8Bit("不能保存至此目录");
        return;
    }
    if(filePath.isEmpty())
        return;
    if(QFileInfo(filePath).suffix() != "json")
        filePath+=".json";
    QFile file(filePath);
    KitModel kitModel = getPublishKitModel();
    if(!checkKitTips(kitModel))
        return;
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
    kitModel.ampFile = ui->cbAmpFile->currentText();
    int _specimenNo = 1;
    for(int i = 0;i < modelMain->rowCount();i++)
    {
        //通道信息
        SpoolModel spool;
        SpoolModel spoolConfig = KitMgr::ins().kitConfig.spoolList[i];
        //
        spool.copy(kitData.spoolList[i]);
        QComboBox* cbAbbrName = dynamic_cast<QComboBox*>(ui->mainTable->indexWidget(modelMain->index(i,1)));
        spool.abbrName = cbAbbrName->currentText();
        QComboBox* cbSpecimenType = dynamic_cast<QComboBox*>(ui->mainTable->indexWidget(modelMain->index(i,2)));
        spool.specimenType = cbSpecimenType->currentIndex();
        if(spool.specimenType == NormType::unDefine)
        {
            spool.specimenNo = 0;
        }
        else
        {
            spool.specimenNo = _specimenNo;
            _specimenNo++;
        }
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

        //阳性质控上限
        SpoolModel spoolPositive;
        spoolPositive.copy(spool);
        if(spool.specimenType >= NormType::normal)
        {
            QLineEdit* editPosiCtMax = dynamic_cast<QLineEdit*>(getWidgetPositiveTable(i,2));
            spoolPositive.ctMax = editPosiCtMax->text().remove(0,1).toInt();
        }
        else
        {
            spoolPositive.ctMax = spoolConfig.ctMax;
        }
        kitModel.positiveSpoolList.append(spoolPositive);
        //阴性质控下限
        SpoolModel spoolNegative;
        spoolNegative.copy(spool);
        if(spool.specimenType >= NormType::normal)
        {
            QLineEdit* editNegaCtMin = dynamic_cast<QLineEdit*>(getWidgetPositiveTable(i,3));
            spoolNegative.ctMin = editNegaCtMin->text().remove(0,1).toInt();
        }
        else
        {
            spoolNegative.ctMin = spoolConfig.ctMin;
        }
        kitModel.negativeSpoolList.append(spoolNegative);
    }
    //重新处理specimenNo字段，向前排序
    KitMgr::ins().sortKitBySpecimenNo(kitModel);
    int zeroSpecimenTypeIndex = 1;
    for(SpoolModel& spoolModel:kitModel.spoolList)
    {
        if(spoolModel.specimenType != 0)
        {
            spoolModel.specimenNo = zeroSpecimenTypeIndex;
            zeroSpecimenTypeIndex++;
        }
    }
    //排序
    KitMgr::ins().sortKitByPoolIndex(kitModel);
    for(int i = 0;i < kitModel.spoolList.length();i++)
    {
        SpoolModel spoolModel = kitModel.spoolList[i];
        SpoolModel positiveSpool = kitModel.positiveSpoolList[i];
        SpoolModel negativeSpool = kitModel.negativeSpoolList[i];
        positiveSpool.specimenNo = spoolModel.specimenNo;
        negativeSpool.specimenNo = spoolModel.specimenNo;
    }
    return kitModel;
}

void KitEditorView::setCurrFilePath(QString _path)
{
    currFilePath = _path;
    ui->lbPath->setText(currFilePath);
}

void KitEditorView::on_editKitName_textChanged(const QString &arg1)
{
    setCurrFilePath(KitMgr::ins().getPublishPath() + arg1 + ".json");
}

void KitEditorView::on_cbMachineType_activated(const QString &arg1)
{
    KitMgr::ins().machineID = arg1;
    setCurrFilePath(KitMgr::ins().getPublishPath() + ui->editKitName->text() + ".json");
}

void KitEditorView::on_btClose_clicked()
{
    close();
}
