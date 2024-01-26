#include "kitcreatedialog.h"
#include "ui_kitcreatedialog.h"

kitCreateDialog::kitCreateDialog(QWidget *parent,QStringList _strList) :
    QDialog(parent),
    ui(new Ui::kitCreateDialog)
{
    this->setWindowModality(Qt::WindowModal);
    this->setFocus();
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowType_Mask);

    ui->setupUi(this);
    ui->layoutMain->setAlignment(Qt::AlignCenter);
    ui->layoutMain->setSpacing(25);
    for(QString str:_strList)
    {
        QPushButton* bt = new QPushButton(this);
        bt->setObjectName(str);
        bt->setText(str);
        ui->layoutMain->addWidget(bt, Qt::AlignCenter);
        bt->setMinimumSize(120,35);
        bt->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
        bt->setStyleSheet("QPushButton {"
                          "     background-color: rgb(0, 160, 233);"
                          "     border-radius: 17px; "
                          "     color: rgb(255, 255, 255); "
                          "     padding-left:10px;"
                          "     padding-right:10px;"
                          "}"
                          "QPushButton:hover {"
                          "     background: qradialgradient(cx:0.5, cy:0.5, radius: 0.5,fx:0.5, fy:0.5,stop:0 rgb(0,122,232),stop:1 rgb(53,190,246) ) "
                          "}"
                          "QPushButton:disabled { "
                          "     background-color: rgb(236, 244,255); "
                          "     color: rgb(180, 180, 180);"
                          "}");
        connect(bt,SIGNAL(clicked()),this,SLOT(onclickStrBt()));
    }
}

kitCreateDialog::~kitCreateDialog()
{
    delete ui;
}

void kitCreateDialog::on_pushButton_clicked()
{
    close();
}

void kitCreateDialog::onclickStrBt()
{
    emit signal_onCreateNewKit(sender()->objectName());
    close();
}
