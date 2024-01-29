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
    const int colCount = 5;
    const int itemWidth = ui->widgetMain->rect().width()/colCount;
    const int itemHeight = 40;
    int itemCount = colorOption.keys().length();
    const int rowCount = ceil((double)itemCount/5);
    int contentHeight = rowCount * itemHeight;
    ui->widgetMain->setFixedHeight(contentHeight);
    ui->scrollAreaWidgetContents->setFixedHeight(contentHeight);
    buttonGroup = new QButtonGroup();
    for(int i = 0;i < itemCount;i++)
    {
        QString colorName = colorOption.keys().at(i);
        QString colorValue = colorOption[colorName];
        QPushButton* bt = new QPushButton(this);
        QString style = "background-color:rgb(%1);"
                        "border-radius:0px;"
                        "border:0px;"
                        "color:rgb(%2);"
                        "font-family:'思源黑体 CN Normal';"
                        "font-size:12pt;";
        style = style.arg(colorValue);
        style = style.arg(KitMgr::ins().getInverseFontColor(colorValue));
        bt->setText(colorName);
        bt->setStyleSheet(style);
        bt->setFixedSize(itemWidth,itemHeight);
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
