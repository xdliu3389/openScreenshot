#ifndef DRAWQUEUE_H
#define DRAWQUEUE_H

#include <QQueue>
#include <QRect>
#include <QMap>

class draw_queue : public QQueue
{
public:


private:
    int rect, circle, line;
    QQueue<QMap<int, QRect>> dq;
}

#endif // DRAWQUEUE_H
