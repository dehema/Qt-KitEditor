#ifndef KITCREATEDIALOG_H
#define KITCREATEDIALOG_H

#include <QDialog>
#include <QStringList>
#include <QDebug>

namespace Ui {
class kitCreateDialog;
}

class kitCreateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit kitCreateDialog(QWidget *parent,QStringList _strList);
    ~kitCreateDialog();

private slots:
    void on_pushButton_clicked();
    void onclickStrBt();

signals:
    void signal_onCreateNewKit(QString);

private:
    Ui::kitCreateDialog *ui;
};

#endif // KITCREATEDIALOG_H
