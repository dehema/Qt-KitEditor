#ifndef KITCOLORDIALOG_H
#define KITCOLORDIALOG_H

#include <QDialog>
#include <QButtonGroup>
#include "KitMgr.h"

namespace Ui {
class kitColorDialog;
}

class kitColorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit kitColorDialog(QWidget *parent,int _spoolIndex);
    ~kitColorDialog();

private slots:
    void onclickColor(int _index);

    void on_pushButton_clicked();

signals:
    void signal_onChooseColor(int, QString);

private:
    Ui::kitColorDialog *ui;
    int spoolIndex;
    QButtonGroup* buttonGroup;
    QMap<QString, QString> colorOption;
};

#endif // KITCOLORDIALOG_H
