#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QMainWindow>
#include <QPixmap>
#include <QWidget>
#include <QScreen>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <vector>
#include <iostream>

namespace Ui {
class ScreenShot;
}

using namespace std;

class ScreenShot : public QMainWindow
{
    Q_OBJECT

public:
    explicit ScreenShot(QWidget *parent = 0);
    void mousePressEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseDoubleClickEvent(QMouseEvent *e);

    void show_bg();
    void draw_rec(vector<QRect> r);
    ~ScreenShot();

private:
    bool mousePressed, cutAreaSelected;
    int areaPos[4];
    float cutOpa,otherOpa;
    QPixmap bg, cut;
    QPoint begin_point;

    Ui::ScreenShot *ui;
};

#endif // SCREENSHOT_H
