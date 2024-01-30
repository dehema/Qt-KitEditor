#include "kitcolordialog.h"
#include "ui_kitcolordialog.h"

kitColorDialog::kitColorDialog(QWidget *parent,int _spoolIndex) :
    QDialog(parent),spoolIndex(_spoolIndex),
    ui(new Ui::kitColorDialog)
{
    this->setWindowModality(Qt::WindowModal);
    this->setFocus();
    this->setWindowFlags(Qt::FramelessWindowHint|Qt::WindowType_Mask);

    ui->setupUi(this);

    colorOption =  KitMgr::ins().getColorOption();
    const int colCount = 4;
    const int itemSize = 64;
    int itemCount = colorOption.keys().length();
    const int rowCount = ceil((double)itemCount/colCount);
//    int contentHeight = rowCount * itemSize;
//    ui->widgetMain->setFixedHeight(contentHeight);
//    ui->scrollAreaWidgetContents->setFixedHeight(contentHeight);
    ui->layoutMain->setSpacing(25);
    buttonGroup = new QButtonGroup();
    for(int i = 0;i < itemCount;i++)
    {
        QString colorName = colorOption.keys().at(i);
        QString colorValue = colorOption[colorName];
        QPushButton* bt = new QPushButton(this);
        QString style = "background-color:rgb(%1);"
                        "border-radius:%3px;"
                        "border:0px;"
                        "color:rgb(%2);"
                        "font-family:'思源黑体 CN Normal';"
                        "font-size:12pt;";
        style = style.arg(colorValue);
        style = style.arg(KitMgr::ins().getInverseFontColor(colorValue));
        style = style.arg(itemSize/2);
//        bt->setText(colorName);
        bt->setStyleSheet(style);
        bt->setFixedSize(itemSize,itemSize);
        ui->layoutMain->addWidget(bt,i/colCount,i%colCount);
        buttonGroup->addButton(bt,buttonGroup->buttons().length());
    }
    connect(buttonGroup,SIGNAL(buttonClicked(int)),this,SLOT(onclickColor(int)));
}

void kitColorDialog::onclickColor(int _index)
{
    QString colorName = colorOption.keys().at(_index);
    QString colorValue = colorOption[colorName];
    emit signal_onChooseColor(spoolIndex, colorValue);
    close();
}

kitColorDialog::~kitColorDialog()
{
    delete ui;
}

void kitColorDialog::on_pushButton_clicked()
{
    close();
}
