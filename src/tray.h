#ifndef TRAY_H
#define TRAY_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QApplication>
#include <QThread>

class Tray : public QMainWindow
{
    Q_OBJECT

public:
    Tray();

public slots:
    void openScreenshot();

private:
    void create_actions();

    void create_tray();

    QMenu *trayMenu;
    QSystemTrayIcon *trayIcon;
    QAction *newScreenShot, *exitScreenShot;

};

#endif // TRAY_H
