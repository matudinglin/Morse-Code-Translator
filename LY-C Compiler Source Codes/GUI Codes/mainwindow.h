#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
#include <QString>
#include "pkgs.h"
using namespace std;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionNew_triggered();
    void on_actionOpen_triggered();
    void on_actionResult_triggered();
    void on_actionSave_triggered();
    void on_actionSave_As_triggered();
    void on_actionInfo_triggered();
    void on_actionSyntax_Result_triggered();
    void on_pushButton_clicked();
    void on_actionError_Result_triggered();
    void on_actionSemantics_Result_triggered();
    void on_actionGenerate_Quaternion_triggered();
    void on_actionNo_3_triggered();
    void on_actionNo_1_triggered();
    void on_actionNo_2_triggered();
    void on_actionNo_4_triggered();
    void myInit();
private:
    Ui::MainWindow *ui;
    QString currFile;

    CodeGenerator *cod;
    ErrorAnalyzer *err;
    LexicalAnalyzer *lex;
    SemanticAnalyzer *sem;
    SyntaxAnalyzer *syn;
};
#endif // MAINWINDOW_H
