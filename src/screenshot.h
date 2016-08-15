#ifndef SCREENSHOT_H
#define SCREENSHOT_H

#include <QMainWindow>
#include <QPixmap>
#include <QWidget>
#include <QScreen>
#include <QMouseEvent>
#include <QPainter>
#include <QClipboard>
#include <QVector>
#include <QStack>
#include <iostream>
#include "draw.h"

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
    void keyPressEvent(QKeyEvent *e);

    void show_bg();
    void draw_all(int move);
    void draw_cut();
    void draw_rec(QRect r);
    void draw_cicle(QRect r);
    void draw_arrow(QRect r);
    ~ScreenShot();

private slots:
    void on_check_clicked();

    void on_cancel_clicked();

    void on_character_clicked();

    void on_rectan_clicked();

    void on_round_clicked();

    void on_arrow_clicked();

private:
    void clrButtonClicks(int k);
    int selectArea(int x, int y);
    bool ifButtonClicked();
    void update_around_area();
    void copy_img_clipboard();
    void exit_without_copy();

    bool mousePressed, cutAreaExits, cutAreaSelected;
    bool buttonClick[4];
    int drawShape, sa;
    int areaPos[4];    
    float cutOpa,otherOpa;
    QPixmap bg, cut, top;
    QPoint begin_point;
    QVector<QRect> aroundArea;
    QVector<draw> drawHis;
    QStack<QVector<draw>> wholeHis;
    draw topDrawPoint;
    draw *dropDrawEle;

    Ui::ScreenShot *ui;
};

#endif // SCREENSHOT_H
