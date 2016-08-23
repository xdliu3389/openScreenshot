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
#include <string>
#include "draw.h"
#include "areapos.h"

#define CLICKCUTAREA -1
#define CLICKOUTSIDE -2
#define CUTOPA 0.0
#define AROUNDOPA 0.6

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
    void draw_all();
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
    void init_widgets();
    void clrButtonClicks(int k);
    int selectArea(int x, int y);
    bool ifButtonClicked();
    void update_mouse_position();
    void update_imgview_area(int px, int py);
    void update_img_rgb_area(int px, int py);
    void update_size_font(int px, int py);
    void update_size_area(int px, int py);
    void update_drawbutton_area();
    void update_around_area();
    void copy_img_clipboard();
    void exit_without_copy();
    void swap(int &n1, int &n2);

    bool mousePressed, cutAreaExits, cutAreaSelected;
    bool buttonClick[4];
    int sa;
    areaPos ap, mp, dp, bound; //ap--->area_position, mp--->mouse_position, dp--->dynamic_position
    QPixmap bg, cut, top;
    QPoint imgPos, sizePos, buttonPos;
    QVector<QRect> aroundArea;
    QVector<draw> drawHis;
    QVector<draw>::iterator selectIt;
    draw topDrawPoint;
    draw *dropDrawEle;

    Ui::ScreenShot *ui;
};

#endif // SCREENSHOT_H
