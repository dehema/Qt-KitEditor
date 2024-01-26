#ifndef KITCHOOSEDIALOG_H
#define KITCHOOSEDIALOG_H

#include <QDialog>
#include <QFileDialog>
#include "kitMgr.h"

namespace Ui {
class kitChooseDialog;
}

class kitChooseDialog : public QDialog
{
    Q_OBJECT

public:
    explicit kitChooseDialog(QWidget *parent, QMap<QString,KitEditorParamsList> _kitEditorParamsList);
    ~kitChooseDialog();

private slots:
    void slot_onclickKitFile();
    void on_btClose_clicked();

    void on_btBrowser_clicked();

signals:
    void signal_onChooseKit(QString _machineID,QString _fileName);
    void signal_onBrowserKit(QString _path);


private:
    Ui::kitChooseDialog *ui;
};

#endif // KITCHOOSEDIALOG_H
