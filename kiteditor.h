#ifndef KITEDITOR_H
#define KITEDITOR_H

#include <QMainWindow>

namespace Ui {
class KitEditor;
}

class KitEditor : public QMainWindow
{
    Q_OBJECT

public:
    explicit KitEditor(QWidget *parent = 0);
    ~KitEditor();

private:
    Ui::KitEditor *ui;
};

#endif // KITEDITOR_H
