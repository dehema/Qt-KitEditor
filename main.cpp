#include "kiteditorview.h"
#include <QApplication>
#include <QDialog>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KitEditorView* w = new KitEditorView(QString(),QString());
    w->setFocus();
    w->exec();
    w->show();

    return a.exec();
}
