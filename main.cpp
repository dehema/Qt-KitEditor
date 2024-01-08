#include "kiteditor.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KitEditor w;
    w.show();

    return a.exec();
}
