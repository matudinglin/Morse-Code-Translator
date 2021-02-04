#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QTextStream>
#include <string>
#include <QVBoxLayout>
#include <QTextBrowser>
#include <windows.h>

//const int mwWidth = 1080;
//const int mwHeight = 720;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , err(new ErrorAnalyzer())
    , lex (new LexicalAnalyzer(err))
    , cod (new CodeGenerator)
    , sem (new SemanticAnalyzer(err,cod))
    , syn (new SyntaxAnalyzer(err,lex,sem,cod))
{
    ui->setupUi(this);
    myInit();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::myInit()
{
    setWindowTitle("Best Compiler in the world. By LinYao and YuQi.");
    setWindowIcon(QIcon(":/pic/icon.png"));
    //resize(mwWidth,mwHeight);
    showMaximized();
    ui->progressBar->reset();
    ui->terminalText->append(">>>Starting...");
}

void MainWindow::on_actionNew_triggered()
{
    currFile = "";
    ui->textEdit->clear();
    ui->resultText->clear();
}

void MainWindow::on_actionOpen_triggered()
{

    QString filePath = QFileDialog::getOpenFileName(NULL,QString::fromUtf8("Select Txt File"),"",QObject::tr("txt(*.txt)"));
    if(filePath == "")
    {
      QMessageBox::information(this,QString::fromUtf8("Info"),QString::fromUtf8("No file selected."),"OK");
    }
    else
    {
        currFile = filePath;
        if(!currFile.isEmpty())
        {
            QFile *file = new QFile;
            file->setFileName(currFile);
            bool ok = file->open(QIODevice::ReadOnly);
            if(ok)
            {
                QTextStream in(file);
                ui->textEdit->setText(in.readAll());
                ui->terminalText->append(">>>Open File : "+filePath);
                file->close();
                delete file;
            }
            else
            {
                QMessageBox::information(this,"Info","Wrong!!!:" + file->errorString());
                return;
            }
        }
    }
}

void MainWindow::on_actionResult_triggered()
{
    QString input = ui->textEdit->toPlainText();
    if(!input.isEmpty())
    {

        ui->terminalText->append(">>>Lexical Analyzing...");
        lex->clear();
        lex->parse(input.toStdString());
        string output2 = lex->getResult();

        QString output = QString::fromStdString(output2);

        //animate
        for(int i=0;i<11;i++)
        {
            ui->progressBar->setValue(10*i);
            Sleep(i*10);
        }
        ui->progressBar->reset();

        ui->resultText->setText(output);

    }
    else
    {
        QMessageBox::information(this,"Info","No content.");
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(currFile == "")
    {
      QMessageBox::information(this,QString::fromUtf8("Info"),QString::fromUtf8("No content."),"OK");
    }
    else
    {
        ui->terminalText->append(">>>Saving...");
        QFile *file = new QFile;
        file->setFileName(currFile);
        bool ok = file->open(QIODevice::WriteOnly);
        if(ok)
        {
            QTextStream in(file);
            in<<ui->textEdit->toPlainText();
            file->close();
            delete file;
        }
        else
        {
            QMessageBox::information(this,"Info","Wrong!!!:" + file->errorString());
            delete file;
            return;
        }
    }
}

void MainWindow::on_actionSave_As_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(NULL,QString::fromUtf8("Save As Txt File"),"",QObject::tr("txt(*.txt)"));
    if(filePath == "")
    {
      QMessageBox::information(this,QString::fromUtf8("Info"),QString::fromUtf8("No file selected."),"OK");
    }
    else
    {
        currFile = filePath;

        if(!currFile.isEmpty())
        {
            QFile *file = new QFile;
            file->setFileName(currFile);
            bool ok = file->open(QIODevice::WriteOnly);
            if(ok)
            {
                QTextStream in(file);
                in<<ui->textEdit->toPlainText();
                file->close();
                delete file;
            }
            else
            {
                QMessageBox::information(this,"Info","Wrong!!!:" + file->errorString());
                delete file;
                return;
            }
        }
    }
}

void MainWindow::on_actionInfo_triggered()
{
    QWidget *infoWidget = new QWidget;
    infoWidget->show();
    infoWidget->setWindowIcon(QIcon(":/pic/icon.png"));
    infoWidget->setWindowTitle("About this compiler");

    QVBoxLayout *infoLayout = new QVBoxLayout;
    QTextBrowser *text = new QTextBrowser;
    text->setText("The best useless compiler in the world.");
    infoLayout->addWidget(text);
    infoWidget->setLayout(infoLayout);
}

void MainWindow::on_actionSyntax_Result_triggered()
{

    QString input = ui->textEdit->toPlainText();

    if(!input.isEmpty())
    {
        //
        ui->terminalText->append(">>>Syntax Analyzing...");
        //
        lex->clear();
        syn->clear();
        lex->parse(input.toStdString());
        int pointer = 0;
        syn->isProgram(pointer);
        string output2;
        output2 = syn->getOutputs();
        //output
        QString output = QString::fromStdString(output2);

        //animate
        for(int i=0;i<11;i++)
        {
            ui->progressBar->setValue(10*i);
            Sleep(i*10);
        }
        ui->progressBar->reset();

        ui->resultText->setText(output);
    }
    else
    {
        QMessageBox::information(this,"Info","No content.");
    }
}

void MainWindow::on_pushButton_clicked()
{
    ui->terminalText->append(">>>Clear...");
    on_actionNew_triggered();
}

void MainWindow::on_actionError_Result_triggered()
{
    QString input = ui->textEdit->toPlainText();

    if(!input.isEmpty())
    {
        //
        ui->terminalText->append(">>>Error Analyzing...");

        lex->clear();
        syn->clear();

        lex->parse(input.toStdString());

        lex->searchError();
        syn->sendError();
        sem->sendError();
        err->sortErrors();

        int pointer = 0;

        // Syntax & Semantic Analysis
        bool flag = syn->isProgram(pointer);
        syn->sendError();
        sem->sendError();

        cout << flag;

        // Output results
        string output2 = err->getErrors();

        QString output = QString::fromStdString(output2);
        //animate
        for(int i=0;i<11;i++)
        {
            ui->progressBar->setValue(10*i);
            Sleep(i*10);
        }
        ui->progressBar->reset();

        if(output.isEmpty()) //没错
        {
            on_actionSyntax_Result_triggered();
            ui->errorText->setText("No Error!!!");
        }
        else
        {
            ui->resultText->setText("Detect Error!!!");
            ui->errorText->setText(output);
        }
    }
    else
    {
        QMessageBox::information(this,"Info","No content.");
    }
}

void MainWindow::on_actionSemantics_Result_triggered()
{
    QString input = ui->textEdit->toPlainText();

    if(!input.isEmpty())
    {
        ui->terminalText->append(">>>Code Running...");
        ui->terminalText->update();

        //跑代码
        lex->parse(input.toStdString());

        int pointer = 0;

        syn->isProgram(pointer);

        //animate
        for(int i=0;i<11;i++)
        {
            ui->progressBar->setValue(10*i);
            Sleep(i*10);
        }
        ui->progressBar->reset();

        ui->terminalText->append(">>>Code Running Done...");
    }
    else
    {
        QMessageBox::information(this,"Info","No content.");
    }
}

void MainWindow::on_actionGenerate_Quaternion_triggered()
{
    QString input = ui->textEdit->toPlainText();

    if(!input.isEmpty())
    {
        //
        ui->terminalText->append(">>>Generate Quaternion...");
        string output2 = cod->getFormula();
        //output
        QString output = QString::fromStdString(output2);

        //animate
        for(int i=0;i<11;i++)
        {
            ui->progressBar->setValue(10*i);
            Sleep(i*10);
        }
        ui->progressBar->reset();

        ui->resultText->setText(output);
    }
    else
    {
        QMessageBox::information(this,"Info","No content.");
    }
}

void MainWindow::on_actionNo_1_triggered()
{

}

void MainWindow::on_actionNo_2_triggered()
{

}

void MainWindow::on_actionNo_3_triggered()
{

}

void MainWindow::on_actionNo_4_triggered()
{

}
