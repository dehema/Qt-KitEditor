#ifndef KITEDITOR_H
#define KITEDITOR_H

#include <QDebug>
#include <QMainWindow>
#include <QFileDialog>
#include <QStandardItemModel>
#include "KitMgr.h"

namespace Ui {
class KitEditor;
}

class KitEditorView : public QMainWindow
{
    Q_OBJECT

public:
    explicit KitEditorView(QWidget *parent = 0);
    ~KitEditorView();

private slots:
    void on_btOpenFile_clicked();
    //读取json
    void onLoadJsonFile(QByteArray _byteArray);
    void slot_onclickColor();

private:
    Ui::KitEditor *ui;
    //体系数据
    KitModel kitData;
    //表格数据
    QStandardItemModel* modelMain;
    void addComboBoxToTable(int _rowIndex);
};

#endif // KITEDITOR_H
