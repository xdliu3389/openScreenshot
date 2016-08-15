#include "screenshot.h"
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
    cutAreaExits = false;
    cutAreaSelected = false;
    cutOpa = 0.0;
    otherOpa = 0.6;

    for(int i=0; i<sizeof(buttonClick)/sizeof(buttonClick[0]); i++)
        buttonClick[i] = false;

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

    sa = selectArea(px, py);

    if(ifButtonClicked())
        draw_all(0);

    if(-1 == sa) {
        drawHis.clear();
        ui->widget->hide();
    }
}

//Release mouse means cut area is selected
void ScreenShot::mouseReleaseEvent(QMouseEvent *e)
{
    //Record every draw operation.
    //If the buttonClick[i] is true, it means some shape has been drawed, so record it into drawHis.
    for(int i=0; i<sizeof(buttonClick)/sizeof(buttonClick[0]); i++) {
        if(buttonClick[i]) {
            draw t;
            t.setShape(i);
            t.setPt(QRect(begin_point.x(), begin_point.y(), e->pos().x()-begin_point.x(), e->pos().y()-begin_point.y()));
            drawHis.push_back(t);
        }
    }
    if(sa != -1)
        wholeHis.push_back(drawHis);

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

    if(ifButtonClicked()) {
        QRect t;
        t.setX(bx); t.setY(by);
        t.setWidth(ex - bx); t.setHeight(ey - by);
        for(int i=0; i<sizeof(buttonClick)/sizeof(buttonClick[0]); i++) {
            if(buttonClick[i]) {
                switch (i) {
                case 0:
                    draw_rec(t); break;
                case 1:
                    draw_cicle(t); break;
                case 2:
                    break;
                case 3:
                    break;
                }
            }
        }

    } else {
        if(mousePressed && sa != -1) {
            int mx = e->pos().x() - begin_point.x(), my = e->pos().y() - begin_point.y();
            if(0 == sa) {
                bx = areaPos[0] + mx; by = areaPos[1] + my;
                ex = areaPos[2] + mx; ey = areaPos[3] + my;
                for(int i=0; i<drawHis.size(); i++) {
                    QRect t = drawHis[i].getPt();
                    t.moveTo(t.x()+mx, t.y()+my);
                    drawHis[i].setPt(t);
                }
            } else {
                bx = areaPos[0]; by = areaPos[1];
                ex = areaPos[2]; ey = areaPos[3];
                QRect t = dropDrawEle->getPt();
                t.moveTo(t.x()+mx, t.y()+my);
                dropDrawEle->setPt(t);
            }
            //update the new begin_point can avoid big shake when drag the cut area.
            begin_point.setX(e->pos().x()); begin_point.setY(e->pos().y());
        }

        //No matter the press point is in or out of cut area. We both need update cut area position.
        areaPos[0] = bx; areaPos[1] = by; areaPos[2] = ex; areaPos[3] = ey;

        update_around_area();

        draw_all(1);
    }
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
    } else if(e->key() == Qt::Key_Z && e->modifiers().testFlag(Qt::ControlModifier)) {
        cout << "ctrl z" << endl;
        if(!wholeHis.empty())
            wholeHis.pop();
        draw_all(0);
    }
}

//Get the background image and set it to the label.
void ScreenShot::show_bg()
{
    QScreen *screen = QApplication::primaryScreen();
    bg = screen->grabWindow(0);
    top = bg;
    ui->label->setPixmap(bg);
    ui->label->resize(bg.width(), bg.height());
}

//Update current draw.
void ScreenShot::draw_all(int move)
{
    draw_cut();

    QPainter painter(&top);
    QPen pen = painter.pen();

    QVector<draw> last;
    if(1 == move)
        last = drawHis;
    else if(!wholeHis.empty())
        last = wholeHis.top();

    for(int i=0; i<last.size(); i++) {
        QRect r = last[i].getPt();

        pen.setColor(last[i].getColor());
        pen.setWidth(last[i].getWidth());
        painter.setPen(pen);

        switch(last[i].getShape()) {
        case 0:
            painter.drawRect(r); break;
        case 1:
            painter.drawArc(r, 0, 360*36); break;
        case 2:
            break;
        case 3:
            break;
        }
    }

    ui->label->setPixmap(top);
}

void ScreenShot::draw_cut()
{
    QPixmap tmp = bg;
    QPainter painter(&tmp);
    QPen pen = painter.pen();
    pen.setStyle(Qt::NoPen);
    painter.setPen(pen);

    painter.setBrush(Qt::white);
    painter.setOpacity(cutOpa);
    painter.drawRect(areaPos[0], areaPos[1], areaPos[2]-areaPos[0], areaPos[3]-areaPos[1]);

    for(int i=0; i<aroundArea.size(); i++) {
        painter.setBrush(Qt::gray);
        painter.setOpacity(otherOpa);
        painter.drawRect(aroundArea[i]);
    }

    ui->label->setPixmap(tmp);
    top = tmp;
}

void ScreenShot::draw_rec(QRect r)
{
    QPixmap tmp = top;
    QPainter painter(&tmp);
    QPen pen = painter.pen();
    pen.setColor(Qt::red);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawRect(r);
    ui->label->setPixmap(tmp);
}

void ScreenShot::draw_cicle(QRect r)
{
    QPixmap tmp = top;
    QPainter painter(&tmp);
    QPen pen = painter.pen();
    pen.setColor(Qt::red);
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawArc(r, 0, 16*360);
    ui->label->setPixmap(tmp);
}

void ScreenShot::draw_arrow(QRect r)
{
    QPixmap tmp = top;
    QPainter painter(&tmp);
    QPen pen = painter.pen();
    pen.setColor(Qt::red);
    pen.setWidth(5);
    painter.setPen(pen);
    ui->label->setPixmap(tmp);
}

ScreenShot::~ScreenShot()
{
    delete ui;
}

void ScreenShot::clrButtonClicks(int k)
{
    for(int i=0; i<sizeof(buttonClick)/sizeof(buttonClick[0]); i++) {
        if(i != k)
            buttonClick[i] = false;
    }
}

int ScreenShot::selectArea(int x, int y)
{
    //The click place is on the element in the cut area
    for(int i=0; i<drawHis.size(); i++) {
        QRect t = drawHis[i].getPt();
        if(x>t.x() && x<(t.x()+t.width()) && y>t.y() && y<(t.y()+t.height())) {
            dropDrawEle = &drawHis[i];
            return 1;
        }
    }

    //The click place is on no element but in the cut area
    if(x>=areaPos[0] && x<=areaPos[2] && y>=areaPos[1] && y<=areaPos[3])
        return 0;

    //The click place is out of cut area
    return -1;
}

bool ScreenShot::ifButtonClicked()
{
    bool f = false;
    for(int i=0; i<sizeof(buttonClick)/sizeof(buttonClick[0]); i++)
        f = f || buttonClick[i];
    return f;
}

void ScreenShot::update_around_area()
{
    aroundArea.clear();

    QRect tmp;
    //up of cut area
    tmp.setX(0); tmp.setY(0);
    tmp.setWidth(this->width()); tmp.setHeight(areaPos[1]);
    aroundArea.push_back(tmp);

    //left of cut area
    tmp.setX(0); tmp.setY(areaPos[1]);
    tmp.setWidth(areaPos[0]); tmp.setHeight(areaPos[3] - areaPos[1]);
    aroundArea.push_back(tmp);

    //right of cut area
    tmp.setX(areaPos[2]); tmp.setY(areaPos[1]);
    tmp.setWidth(this->width() - areaPos[2]); tmp.setHeight(areaPos[3] - areaPos[1]);
    aroundArea.push_back(tmp);

    //down of cut area
    tmp.setX(0); tmp.setY(areaPos[3]);
    tmp.setWidth(this->width()); tmp.setHeight(this->height() - areaPos[3]);
    aroundArea.push_back(tmp);
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


void ScreenShot::on_rectan_clicked()
{
    clrButtonClicks(0);
    buttonClick[0] = !buttonClick[0];
}

void ScreenShot::on_round_clicked()
{
    clrButtonClicks(1);
    buttonClick[1] = !buttonClick[1];
}

void ScreenShot::on_arrow_clicked()
{
    clrButtonClicks(2);
    buttonClick[2] = !buttonClick[2];
}
