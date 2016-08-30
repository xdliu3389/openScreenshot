#include "screenshot.h"
#include "tray.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setQuitOnLastWindowClosed(false);

    Tray *t = new Tray();
    t->hide();

    return a.exec();
}
