#include "kiteditor.h"
#include "ui_kiteditor.h"

KitEditor::KitEditor(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::KitEditor)
{
    ui->setupUi(this);
}

KitEditor::~KitEditor()
{
    delete ui;
}
