#include "screenshot.h"
#include "ui_screenshot.h"

using namespace std;

ScreenShot::ScreenShot(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ScreenShot)
{
    ui->setupUi(this);
    this->setMouseTracking(true);
    ui->label->setMouseTracking(true);
    ui->centralWidget->setMouseTracking(true);

    init_widgets();
}

//Set all the widgets initial position and status
void ScreenShot::init_widgets()
{
    //Hide most widget in the begining of program
    ui->widget->hide();
    ui->SizeRgbDisplay->setStyleSheet("background-color: rgb(0, 0, 0, 98);");
    ui->SizeRgbDisplay->hide();
    ui->ImgShow->hide();

    show_bg();

    mousePressed = false;
    cutAreaExits = false;
    cutAreaSelected = false;

    for(int i=0; i<sizeof(buttonClick)/sizeof(buttonClick[0]); i++)
        buttonClick[i] = false;
}

//If the press point is in the cut area before, then move mouse means move this cut area.
//If the press point is out of the previous area, then move mouse means draw a new cut area.
void ScreenShot::mousePressEvent(QMouseEvent *e)
{
    bound = areaPos(0, 0, this->width(), this->height());
    int px = e->pos().x(), py = e->pos().y();
    mousePressed = true;

    //ap.initBxy(e->pos());
    mp.initBxy(e->pos());
    dp.initBxy(e->pos());

    sa = selectArea(px, py);

    if(ifButtonClicked())
        draw_all();

    if(CLICKOUTSIDE == sa) {
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
            cout << "dpbx:" << dp.bx() << ";apbx:" << ap.bx() << endl;
            t.setPt(QRect(dp.bx()-ap.bx(), dp.by()-ap.by(), dp.ex()-dp.bx(), dp.ey()-dp.by()));
            drawHis.push_back(t);
        }
    }

    ui->widget->move(ap.ex()-ui->widget->width(), ap.ey());
    ui->widget->show();
    mousePressed = false;
}

//Deal with select cut area, show different color and Opacity between cut area and other areas.
void ScreenShot::mouseMoveEvent(QMouseEvent *e)
{
    update_imgview_area(e->pos().x(), e->pos().y());

    update_size_font(e->pos().x(), e->pos().y());

    if(mousePressed) {
        vector<QRect> pr;
        QRect r;

        mp.initExy(e->pos());

        //We have to get mxy before dp=mp.normalPos()
        //after dp=mp.normalPos(), the mxy will be >0 forever.
        dp.initExy(e->pos());
        QPoint mxy = dp.disBE();

        //we get the correct position:ex is bigger than bx, ey is bigger than by
        //Because mp is the mouse position, so the mp store the press point. While dp not.
        dp = mp.normalPos();


        if(ifButtonClicked()) {
            QRect t;
            dp.resetPosIn(ap);
            t.setX(dp.bx()); t.setY(dp.by());
            t.setWidth(dp.ex() - dp.bx()); t.setHeight(dp.ey() - dp.by());
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
            areaPos tmp = dp;
            if(sa != CLICKOUTSIDE) {
                dp = ap;
                if(!dp.outBound(bound, mxy)) {
                    dp.move(mxy);
                }
                tmp = dp;

                //update the new begin_point can avoid big shake when drag the cut area.
                dp.initBxy(e->pos());
            }

            //No matter the press point is in or out of cut area. We both need update cut area position.
            ap = tmp;

            update_img_rgb_area(e->pos().x(), e->pos().y());

            update_drawbutton_area();

            update_around_area();

            draw_all();
        }
    }
}

//Exit program when mouse double click
void ScreenShot::mouseDoubleClickEvent(QMouseEvent *e)
{
    exit_without_copy();
}

//Save file to clipbord when pressing return or enter
//Clear draw buttons click when pressing esc
void ScreenShot::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        copy_img_clipboard();
        QApplication::exit();
    } else if(e->key() == Qt::Key_Escape) {
        ui->rectan->setStyleSheet("border-image: url(:/imgs/rec.png);");
        ui->round->setStyleSheet("border-image: url(:/imgs/round.png);");
        clrButtonClicks(-1);
    } else if(e->key() == Qt::Key_Delete) {
        if(sa >= 0) {
            drawHis.erase(selectIt);
            sa = CLICKCUTAREA;
            draw_all();
        }
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
void ScreenShot::draw_all()
{
    draw_cut();

    QPainter painter(&top);
    QPen pen = painter.pen();

    for(int i=0; i<drawHis.size(); i++) {
        QRect r = drawHis[i].getPt();
        int w = r.width(), h = r.height();
        r.setX(r.x()+ap.bx());
        r.setY(r.y()+ap.by());
        r.setWidth(w);
        r.setHeight(h);

        pen.setColor(drawHis[i].getColor());
        pen.setWidth(drawHis[i].getWidth());
        painter.setPen(pen);

        switch(drawHis[i].getShape()) {
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
    painter.setOpacity(CUTOPA);
    painter.drawRect(ap.bx(), ap.by(), ap.ex()-ap.bx(), ap.ey()-ap.by());

    for(int i=0; i<aroundArea.size(); i++) {
        painter.setBrush(Qt::gray);
        painter.setOpacity(AROUNDOPA);
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
        if(x>=ap.bx()+t.x() && y>=ap.by()+t.y() && x<=ap.bx()+t.x()+t.width() && y<ap.by()+t.y()+t.height()) {
            selectIt = &drawHis[i];
            return i;
        }
    }

    //The click place is on no element but in the cut area
    if(x>=ap.bx() && x<=ap.ex() && y>=ap.by() && y<=ap.ey())
        return CLICKCUTAREA;

    //The click place is out of cut area
    return CLICKOUTSIDE;
}

bool ScreenShot::ifButtonClicked()
{
    bool f = false;
    for(int i=0; i<sizeof(buttonClick)/sizeof(buttonClick[0]); i++)
        f = f || buttonClick[i];
    return f;
}

void ScreenShot::update_mouse_position()
{
    int bx, by, ex, ey, tmp;
    bx = dp.bx();
    by = dp.by();
    ex = dp.ex();
    ey = dp.ey();
    if(bx > ex)
        swap(bx, ex);
    if(by > ey)
        swap(by, ey);
    dp.setBx(bx);
    dp.setBy(by);
    dp.setEx(ex);
    dp.setEy(ey);
}

void ScreenShot::update_imgview_area(int px, int py)
{
    areaPos square;
    square.setBx(px - ui->ImgShow->width()/2+1);
    square.setBy(py - ui->ImgShow->height()/2+1);
    square.setEx(px + ui->ImgShow->width()/2-1);
    square.setEy(py + ui->ImgShow->height()/2-1);

    //Set square pixmap in the left-upper position
    QPixmap squarePixmap = bg.grabWindow(NULL, square.bx(), square.by(), square.ex()-square.bx(), square.ey()-square.by());
    QGraphicsScene *scene = new QGraphicsScene();
    scene->addPixmap(squarePixmap);
    ui->ImgShow->setScene(scene);
}

void ScreenShot::update_img_rgb_area(int px, int py)
{

    int w = ui->ImgStrip->width();
    int h = ui->ImgStrip->height();
    if(ap.by()-h > 0) {
        imgPos.setX(ap.bx());
        imgPos.setY(ap.by()-h);
    } else if(ap.bx()-w > 0) {
        imgPos.setX(ap.bx()-w);
        imgPos.setY(ap.by());
    } else if(ap.ex()+w < this->width()) {
        imgPos.setX(ap.ex());
        imgPos.setY(ap.by());
    } else {
        imgPos.setX(ap.bx());
        imgPos.setY(ap.by());
    }
    ui->ImgStrip->move(imgPos);

    ui->ImgShow->show();
    ui->SizeRgbDisplay->show();
}

void ScreenShot::update_size_font(int px, int py)
{
    QImage tmp = top.toImage();
    QColor colorRes = tmp.pixel(px, py);
    //QColor colorRes = tmp.pixelColor(px, py);   pixelColor is for linux qt, pixel is for windows qt
    int r = colorRes.red();
    int g = colorRes.green();
    int b = colorRes.blue();
    string s = "<font style='color:white; font-size:10px; font-weight:bold;'>RGB:";
    s += to_string(r)+","+to_string(g)+","+to_string(b)+")<br>";
    s += "Size:(";
    s += to_string(ap.ex()-ap.bx());
    s += ",";
    s += to_string(ap.ey()-ap.by());
    s += ")</font><br><br><font style='color:white; font-size:9px;'>Double-click to exit the screenshot</font>";

    ui->SizeRgbDisplay->setHtml(QString::fromStdString(s));
}

void ScreenShot::update_drawbutton_area()
{
    int w = ui->widget->width();
    int h = ui->widget->height();
    if(ap.ey()+h < this->height()) {
        buttonPos.setX(ap.ex()-w);
        buttonPos.setY(ap.ey());
    } else if(ap.by()-h > 0) {
        buttonPos.setX(ap.ex()-w);
        buttonPos.setY(ap.by()-h);
    } else {
        buttonPos.setX(ap.ex()-w);
        buttonPos.setY(ap.by());
    }

    ui->widget->move(buttonPos);
}

void ScreenShot::update_around_area()
{
    aroundArea.clear();

    QRect tmp;
    //up of cut area
    tmp.setX(0); tmp.setY(0);
    tmp.setWidth(this->width()); tmp.setHeight(ap.by());
    aroundArea.push_back(tmp);

    //left of cut area
    tmp.setX(0); tmp.setY(ap.by());
    tmp.setWidth(ap.bx()); tmp.setHeight(ap.ey() - ap.by());
    aroundArea.push_back(tmp);

    //right of cut area
    tmp.setX(ap.ex()); tmp.setY(ap.by());
    tmp.setWidth(this->width() - ap.ex()); tmp.setHeight(ap.ey() - ap.by());
    aroundArea.push_back(tmp);

    //down of cut area
    tmp.setX(0); tmp.setY(ap.ey());
    tmp.setWidth(this->width()); tmp.setHeight(this->height() - ap.ey());
    aroundArea.push_back(tmp);
}

void ScreenShot::copy_img_clipboard()
{
    QClipboard *clip = QApplication::clipboard();
    clip->setPixmap(bg.grabWindow(NULL, ap.bx(), ap.by(),
                    ap.ex()-ap.bx(), ap.ey()-ap.by()));
}

void ScreenShot::exit_without_copy()
{
    QApplication::exit();
}

void ScreenShot::swap(int &n1, int &n2)
{
    int t;
    t = n1;
    n1 = n2;
    n2 = t;
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
    if(!buttonClick[0]) {
        ui->rectan->setStyleSheet("border-image: url(:/imgs/rec_press.png);");
        ui->round->setStyleSheet("border-image: url(:/imgs/round.png);");
    } else {
        ui->rectan->setStyleSheet("border-image: url(:/imgs/rec.png);");
    }
    clrButtonClicks(0);
    buttonClick[0] = !buttonClick[0];
}

void ScreenShot::on_round_clicked()
{
    if(!buttonClick[1]) {
        ui->rectan->setStyleSheet("border-image: url(:/imgs/rec.png);");
        ui->round->setStyleSheet("border-image: url(:/imgs/round_press.png);");
    } else {
        ui->round->setStyleSheet("border-image: url(:/imgs/round.png);");
    }
    clrButtonClicks(1);
    buttonClick[1] = !buttonClick[1];
}

void ScreenShot::on_arrow_clicked()
{
    clrButtonClicks(2);
    buttonClick[2] = !buttonClick[2];
}
