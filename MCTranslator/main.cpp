#include "mctranslator.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MCTranslator w;
    w.show();
    return a.exec();
}
