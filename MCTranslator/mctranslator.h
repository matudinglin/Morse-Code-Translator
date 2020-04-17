#ifndef MCTRANSLATOR_H
#define MCTRANSLATOR_H

#include <QWidget>
#include "canvas.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MCTranslator; }
QT_END_NAMESPACE

class MCTranslator : public QWidget
{
    Q_OBJECT

public:
    MCTranslator(QWidget *parent = nullptr);
    ~MCTranslator();
private slots:
    void on_btnEncode_clicked();
    void on_btnDecode_clicked();
    void on_btnPlay_clicked();
    void on_btnAudio_clicked();

private:
    Ui::MCTranslator *ui;
    Canvas *canvas;
};
#endif // MCTRANSLATOR_H
