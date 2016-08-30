#include "tray.h"
#include "screenshot.h"
#include <iostream>

using namespace std;

Tray::Tray()
{
    create_actions();

    create_tray();
}

void Tray::openScreenshot()
{
    ScreenShot *s = new ScreenShot();

    //Release the resource of screenshot window when it's closed.
    //Otherwise the memory will leak here.
    s->setAttribute(Qt::WA_DeleteOnClose);

    s->showFullScreen();
}

void Tray::create_actions()
{
    newScreenShot = new QAction(tr("&New Screenshot"), this);
    connect(newScreenShot, &QAction::triggered, this, &Tray::openScreenshot);

    exitScreenShot = new QAction(tr("&Exit"), this);
    connect(exitScreenShot, &QAction::triggered, qApp, &QCoreApplication::quit);
}

void Tray::create_tray()
{
    QIcon icon = QIcon(":/imgs/screenIcon.png");
    trayMenu = new QMenu(this);
    trayMenu->addAction(newScreenShot);
    trayMenu->addSeparator();
    trayMenu->addAction(exitScreenShot);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("fuckshit");
    trayIcon->show();
    trayIcon->setContextMenu(trayMenu);
}
