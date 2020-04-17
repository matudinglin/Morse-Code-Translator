#include "helper.h"


Helper::Helper(QObject *parent) : QObject(parent)
{
    background = QBrush(QColor(0,0,0));
    signal = QBrush(QColor(255,255,255));
}


void Helper::paint(QPainter *painter, QPaintEvent *event)
{
    painter->fillRect(event->rect(), background);
    painter->fillRect(event->rect(), signal);
}

