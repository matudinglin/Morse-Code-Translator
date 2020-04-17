#ifndef HELPER_H
#define HELPER_H

#include <QObject>
#include <QPainter>
#include <QPaintEvent>

class Helper : public QObject
{
    Q_OBJECT
public:
    Helper(QObject *parent = nullptr);
    void paint(QPainter *painter, QPaintEvent *event);
private:
    QBrush background;
    QBrush signal;
};

#endif // HELPER_H
