#ifndef AREAPOS_H
#define AREAPOS_H

#include <QPoint>
#include <iostream>

class areaPos {
public:
    areaPos(int a=-1, int b=-1, int c=-1, int d=-1);
    void operator = (areaPos ap);

    int bx();
    int by();
    int ex();
    int ey();

    void setBx(int n);
    void setBy(int n);
    void setEx(int n);
    void setEy(int n);

    void initBxy(QPoint bxy);
    void initExy(QPoint exy);
    areaPos normalPos();
    QPoint disBE();
    void resetPosIn(areaPos ap);
    void move(QPoint mxy);
    bool outBound(areaPos bound, QPoint mxy);

private:
    int lx, ly, rx, ry;
};

#endif // AREAPOS_H
