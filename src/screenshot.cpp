#include "screenshot.h"
#include "toolbar.h"
#include "ui_screenshot.h"

using namespace std;

ScreenShot::ScreenShot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenShot)
{
    ui->setupUi(this);

    ui->widget->hide();
    show_bg();
    mousePressed = false;
    cutAreaSelected = false;
    cutOpa = 0.0;
    otherOpa = 0.6;
    for(int i=0; i<sizeof(areaPos)/sizeof(areaPos[0]); i++)
        areaPos[i] = -1;
}

//If the press point is in the cut area before, then move mouse means move this cut area.
//If the press point is out of the previous area, then move mouse means draw a new cut area.
void ScreenShot::mousePressEvent(QMouseEvent *e)
{
    int px = e->pos().x(), py = e->pos().y();
    mousePressed = true;
    begin_point.setX(e->pos().x());
    begin_point.setY(e->pos().y());

    if(px>=areaPos[0] && px<=areaPos[2] && py>=areaPos[1] && py<=areaPos[3]) {
        cutAreaSelected = true;
    } else {
        ui->widget->hide();
        cutAreaSelected = false;
    }
}

//Release mouse means cut area is selected
void ScreenShot::mouseReleaseEvent(QMouseEvent *e)
{
    ui->widget->move(areaPos[2]-ui->widget->width(), areaPos[3]);
    ui->widget->show();
    mousePressed = false;
}

//Deal with select cut area, show different color and Opacity between cut area and other areas.
void ScreenShot::mouseMoveEvent(QMouseEvent *e)
{
    vector<QRect> pr;
    QRect r;
    int bx, by, ex, ey;

    //bx,by is the begin_point(left-up point) of the rectangle.
    //ex,ey is the end_point(right-down point) of the rectangle.
    bx = begin_point.x()>e->pos().x()?e->pos().x():begin_point.x();
    ex = begin_point.x()>e->pos().x()?begin_point.x():e->pos().x();
    by = begin_point.y()>e->pos().y()?e->pos().y():begin_point.y();
    ey = begin_point.y()>e->pos().y()?begin_point.y():e->pos().y();

    if(mousePressed && cutAreaSelected) {
        int mx = e->pos().x() - begin_point.x(), my = e->pos().y() - begin_point.y();
        bx = areaPos[0] + mx; by = areaPos[1] + my;
        ex = areaPos[2] + mx; ey = areaPos[3] + my;

        //update the new begin_point can avoid big shake when drag the cut area.
        begin_point.setX(e->pos().x()); begin_point.setY(e->pos().y());
    }

    //No matter the press point is in or out of cut area. We both need update cut area position.
    areaPos[0] = bx; areaPos[1] = by; areaPos[2] = ex; areaPos[3] = ey;

    //Up of cut area
    r.setX(bx); r.setY(by);
    r.setWidth(ex-bx); r.setHeight(ey-by);
    pr.push_back(r);

    //Left of cut area
    r.setX(0); r.setY(0);
    r.setWidth(this->width()); r.setHeight(by);
    pr.push_back(r);

    //Right of cut area
    r.setX(0); r.setY(by);
    r.setWidth(bx); r.setHeight(ey-by);
    pr.push_back(r);

    //Down of cut area
    r.setX(ex); r.setY(by);
    r.setWidth(this->width()-ex); r.setHeight(ey-by);
    pr.push_back(r);

    r.setX(0); r.setY(ey);
    r.setWidth(this->width()); r.setHeight(this->height()-ey);
    pr.push_back(r);

    draw_rec(pr);
}

//Exit program when mouse double click
void ScreenShot::mouseDoubleClickEvent(QMouseEvent *e)
{
    exit_without_copy();
}

//Save file to clipbord when pressing return or enter
//Exit programe when pressing esc
void ScreenShot::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        copy_img_clipboard();
        QApplication::exit();
    } else if(e->key() == Qt::Key_Escape) {
        exit_without_copy();
    }
}

//Get the background image and set it to the label.
void ScreenShot::show_bg()
{
    QScreen *screen = QApplication::primaryScreen();
    bg = screen->grabWindow(0);
    ui->label->setPixmap(bg);
    ui->label->resize(bg.width(), bg.height());
}

void ScreenShot::draw_rec(vector<QRect> r)
{
    QPixmap tmp = bg;
    QPainter painter(&tmp);
    QPen pen = painter.pen();
    pen.setStyle(Qt::NoPen);
    painter.setPen(pen);
    for(int i=r.size()-1; i>=0; i--) {
        if(0 == i) {
            painter.setBrush(Qt::white);
            painter.setOpacity(cutOpa);
        } else {
            painter.setBrush(Qt::gray);
            painter.setOpacity(otherOpa);
        }
        painter.drawRect(r[i].x(), r[i].y(), r[i].width(), r[i].height());
    }
    ui->label->setPixmap(tmp);
}

ScreenShot::~ScreenShot()
{
    delete ui;
}

void ScreenShot::copy_img_clipboard()
{
    QClipboard *clip = QApplication::clipboard();
    clip->setPixmap(bg.grabWindow(NULL, areaPos[0], areaPos[1],
                    areaPos[2]-areaPos[0], areaPos[3]-areaPos[1]));
}

void ScreenShot::exit_without_copy()
{
    QApplication::exit();
}

void ScreenShot::on_check_clicked()
{
    copy_img_clipboard();
    QApplication::exit();
}

void ScreenShot::on_cancel_clicked()
{
    exit_without_copy();
}

void ScreenShot::on_character_clicked()
{

}

void ScreenShot::on_rectangle_clicked()
{
    rectArea = true;
}
