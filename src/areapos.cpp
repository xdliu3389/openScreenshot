#include "areapos.h"

areaPos::areaPos(int a, int b, int c, int d)
    :lx(a), ly(b), rx(c), ry(d)
{}

void areaPos::operator =(areaPos ap)
{
    lx = ap.bx();
    ly = ap.by();
    rx = ap.ex();
    ry = ap.ey();
}

int areaPos::bx()
{
    return lx;
}

int areaPos::by()
{
    return ly;
}

int areaPos::ex()
{
    return rx;
}

int areaPos::ey()
{
    return ry;
}

void areaPos::setBx(int n)
{
    lx = n;
}

void areaPos::setBy(int n)
{
    ly = n;
}

void areaPos::setEx(int n)
{
    rx = n;
}

void areaPos::setEy(int n)
{
    ry = n;
}

void areaPos::initBxy(QPoint bxy)
{
    lx = bxy.x();
    ly = bxy.y();
}

void areaPos::initExy(QPoint exy)
{
    rx = exy.x();
    ry = exy.y();
}

areaPos areaPos::normalPos()
{
    areaPos res;
    res.setBx(lx>rx?rx:lx);
    res.setBy(ly>ry?ry:ly);
    res.setEx(lx>rx?lx:rx);
    res.setEy(ly>ry?ly:ry);
    return res;
}

QPoint areaPos::disBE()
{
    QPoint res;
    res.setX(rx - lx);
    res.setY(ry - ly);
    return res;
}

void areaPos::resetPosIn(areaPos ap)
{
    lx = lx < ap.bx() ? ap.bx() : lx;
    ly = ly < ap.by() ? ap.by() : ly;
    rx = rx > ap.ex() ? ap.ex() : rx;
    ry = ry > ap.ey() ? ap.ey() : ry;
}

void areaPos::move(QPoint mxy)
{
    lx = lx + mxy.x();
    ly = ly + mxy.y();
    rx = rx + mxy.x();
    ry = ry + mxy.y();
}

//If after moving mxy, the area is out of bound. Then we need set the are to the bound boundary.
bool areaPos::outBound(areaPos bound, QPoint mxy)
{
    //To set close to the boundary, we need width and height.
    int w = disBE().x(), h = disBE().y();

    bool flag = false;
    if(lx+mxy.x() < bound.bx()) {
        lx = bound.bx();
        rx = lx + w;
        flag = true;
    }
    if(ly+mxy.y() < bound.by()) {
        ly = bound.by();
        ry = ly + h;
        flag = true;
    }
    if(rx+mxy.x() > bound.ex()) {
        rx = bound.ex();
        lx = rx - w;
        flag = true;
    }
    if(ry+mxy.y() > bound.ey()) {
        ry = bound.ey();
        ly = ry - h;
        flag = true;
    }

    return flag;
}
