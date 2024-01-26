#include "kitchoosedialog.h"
#include "ui_kitchoosedialog.h"

kitChooseDialog::kitChooseDialog(QWidget *parent, QMap<QString,KitEditorParamsList> _kitEditorParamsList) :
    QDialog(parent),
    ui(new Ui::kitChooseDialog)
{
    this->setWindowModality(Qt::WindowModal);
    this->setFocus();
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowType_Mask);

    ui->setupUi(this);
    ui->layoutMain->setAlignment(Qt::AlignCenter);
    ui->layoutMain->setSpacing(0);
    int totalHeight = 0;
    for(QString machinedID:_kitEditorParamsList.keys())
    {
        QLabel* lbMachinedID = new QLabel(this);
        lbMachinedID->setText(machinedID);
        lbMachinedID->setAlignment(Qt::AlignCenter);
        lbMachinedID->setStyleSheet("QLabel {"
        "   font-family:'思源黑体 CN Bold';"
        "   font-size:22px;"
        "   color: rgb(0, 160, 233); "
        "}");
        lbMachinedID->setFixedHeight(60);
        lbMachinedID->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
        totalHeight+=60;
        ui->layoutMain->addWidget(lbMachinedID, Qt::AlignCenter);
        for(QString fileName:_kitEditorParamsList[machinedID].map.keys())
        {
            QPushButton* bt = new QPushButton(this);
            QString objName = QString("%1,%2").arg(machinedID).arg(fileName);
            bt->setObjectName(objName);
            bt->setText(fileName);
            ui->layoutMain->addWidget(bt, Qt::AlignCenter);
            bt->setMinimumSize(120,35);
            bt->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
            bt->setFixedHeight(40);
            totalHeight+=40;
            bt->setStyleSheet("QPushButton {"
                              "     font-family:'思源黑体 CN Normal';"
                              "     border-radius: 17px; "
                              "     color: rgb(0, 160, 233); "
                              "     padding-left:10px;"
                              "     padding-right:10px;"
                              "}"
                              "QPushButton:hover {"
                              "     color: rgb(246, 180, 43); "
//                              "     background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5,fx:0.5, fy:0.5,stop:0 rgb(0,122,232),stop:1 rgb(53,190,246) ) "
                              "}"
                              "QPushButton:disabled { "
                              "     background-color: rgb(236, 244,255); "
                              "     color: rgb(180, 180, 180);"
                              "}");
            connect(bt,SIGNAL(clicked()),this,SLOT(slot_onclickKitFile()));
        }
    }
    totalHeight = qMax(totalHeight, ui->scrollArea->height());
    ui->widgetMain->setFixedHeight(totalHeight);
    ui->contentMain->setFixedHeight(totalHeight);
}

kitChooseDialog::~kitChooseDialog()
{
    delete ui;
}

void kitChooseDialog::slot_onclickKitFile()
{
    QStringList args = sender()->objectName().split(",");
    QString machinedID = args[0];
    QString fileName = args[1];
    emit signal_onChooseKit(machinedID, fileName);
    close();
}

void kitChooseDialog::on_btClose_clicked()
{
    close();
}

void kitChooseDialog::on_btBrowser_clicked()
{
    QString path = QFileDialog::getOpenFileName(this,QStringLiteral("选择配置文件"),KitMgr::ins().getPublishPath(),QStringLiteral("(*json);"));
    if (!path.isEmpty())
    {
        emit signal_onBrowserKit(path);
        close();
    }
}
