#include "draw.h"

draw::draw()
{
    move = false;
    color = Qt::red;
    width = 3;
}

int draw::getShape()
{
    return shape;
}

int draw::getWidth()
{
    return width;
}

float draw::getOpa()
{
    return opa;
}

bool draw::isMove()
{
    return move;
}

QRect draw::getPt()
{
    return pt;
}

QBrush draw::getBrush()
{
    return brush;
}

QColor draw::getColor()
{
    return color;
}

void draw::setShape(int s)
{
    shape = s;
}

void draw::setWidth(int w)
{
    width = w;
}

void draw::setOpa(float o)
{
    opa = o;
}

void draw::setMove(bool m)
{
    move = m;
}

void draw::setPt(QRect p)
{
    pt = p;
}

void draw::setBrush(QBrush b)
{
    brush = b;
}

void draw::setColor(QColor c)
{
    color = c;
}
