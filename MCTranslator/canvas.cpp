#include "canvas.h"
#include <synchapi.h>
#include "make_noisy.h"
#include <QtMultimedia/QMediaPlayer>

Canvas::Canvas(QWidget *parent) : QWidget(parent){}

void Canvas::animate()
{
    QBrush background = QBrush(QColor(0, 0, 0));
    QBrush signal = QBrush(QColor(255, 255, 255));
    QString::iterator codedItr = codedText.begin();
    while(codedItr!=codedText.end())
    {
        if(*codedItr=='.')
        {
            fill=signal;
            Sleep(100);
            repaint();
        }
        else if(*codedItr=='-')
        {
            fill=signal;
            Sleep(300);
            repaint();
        }
        else
        {
            fill=background;
            Sleep(100);
            repaint();
        }
        codedItr++;
    }
}

void Canvas::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.fillRect(event->rect(),fill);
    painter.end();
}

void Canvas::stop()
{
    fill = QBrush(QColor(0, 0, 0));
    update();
}

void Canvas::music()
{
    QMediaPlayer *player=new QMediaPlayer;
    make_noisy();
    player->setMedia(QUrl::fromLocalFile("./output.wav"));
    player->setVolume(50);

    QString::iterator codedItr = codedText.begin();
    while(codedItr!=codedText.end())
    {
        if(*codedItr=='.')
        {
            player->play();
            Sleep(200);
            player->stop();
        }
        else if(*codedItr=='-')
        {
            player->play();
            Sleep(600);
            player->stop();
        }
        else
        {
            player->play();
            Sleep(200);
            player->stop();
        }
        codedItr++;
    }
}
