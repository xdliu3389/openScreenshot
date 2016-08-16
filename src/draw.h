#ifndef DRAW_H
#define DRAW_H

#include <QRect>
#include <QBrush>
#include <QColor>

class draw
{
public:
    draw();
    draw(int _s, float _o, QRect _p, QBrush _b, QColor _c)
        :shape(_s), opa(_o), pt(_p), brush(_b), color(_c) {}

    int getShape();
    int getWidth();
    float getOpa();
    bool isMove();
    QRect getPt();
    QBrush getBrush();
    QColor getColor();

    void setShape(int s);
    void setWidth(int w);
    void setOpa(float o);
    void setMove(bool m);
    void setPt(QRect p);
    void setBrush(QBrush b);
    void setColor(QColor c);

private:
    //Draw shape information
    int shape;
    int width;
    float opa;
    QRect pt;
    QBrush brush;
    QColor color;

    //Move information
    int shapeId;
    QPoint mvXY;
    bool move;
};

#endif // DRAW_H
