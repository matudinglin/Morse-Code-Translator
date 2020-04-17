#ifndef CANVAS_H
#define CANVAS_H

#include <QWidget>
#include <QPainter>
#include "helper.h"
#include <QtMultimedia/QMediaPlayer>

class Canvas : public QWidget
{
    Q_OBJECT
public:
    Canvas(QWidget *parent = nullptr);
    QString codedText;
    void animate();
    void stop();
    void music();
protected:
    void paintEvent(QPaintEvent *event) override;
private:
    QBrush fill;
};

#endif // CANVAS_H
