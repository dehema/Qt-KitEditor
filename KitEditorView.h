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

class KitEditorView : public QDialog
{
    Q_OBJECT

public:
    explicit KitEditorView(QString _kitFileParams,QString _tcpParams, QWidget *parent = 0);
    ~KitEditorView();

private slots:
    void on_btOpenFile_clicked();
    void slot_onclickColor();
    void slot_onAbbrNameTypeIndexChange(QString _str);
    void slot_onSpecimenTypeIndexChange(int _index);
    void slot_onCtTextChanged(QString _str);
    void slot_onPositiveCtMinTextChanged(QString _str);
    void slot_onPositiveCtMaxTextChanged(QString _str);
    void on_btCreatKit_clicked();
    void on_btCheckInfo_clicked();
    void on_btPushlish_clicked();
    void on_btSaveAs_clicked();
    void slot_onCreateNewKit(QString str);
    void slot_onChooseKit(QString _machineID,QString _fileName);
    void slot_onBrowserKit(QString _path);

    void on_editKitName_textChanged(const QString &arg1);

private:
    Ui::KitEditor *ui;
    QString currFilePath;
    //体系数据
    KitModel kitData;
    //表格数据
    QStandardItemModel* modelMain;
    QStandardItemModel* modelPositive;
    //参数 文件列表和默认打开的文件
    KitEditorParams kitFileParams;
    //参数 温控流程选项
    KitEditorParams tcpParams;
    //-----------------------------function-----------------------------
    KitModel getPublishKitModel();
    QWidget* getWidgetMainTable(int _rowIndex,int _colIndex);
    QWidget* getWidgetPositiveTable(int _rowIndex,int _colIndex);
    //读取json
    void onLoadJsonFile(QByteArray _byteArray);
    void initMainTable();
    void initSubMainTable(int _row,int _specimenType);
    void fillBlankMainTable(int _row,int _col);
    void initPositiveTable();
    void initSubPositiveTable(int _row,int _specimenType);
    void fillBlankPositiveTable(int _row,int _col);
    void initMainTableData();
    void openKitFile(QString _path);
    void refreshUI();
    void onAbbrNameTypeIndexChange(int _rowIndex,QString _str);
    void onSpecimenTypeIndexChange(int _rowIndex,int _specimenType);
    void setCurrFilePath(QString path);
};

#endif // KITEDITOR_H
