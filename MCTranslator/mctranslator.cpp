#include "mctranslator.h"
#include "ui_mctranslator.h"
#include "canvas.h"

QMap<QChar,QString> MorseTable{{'a',".-"}, {'b',"-..."}, {'c',"-.-."}, {'d',"-." } ,{ 'e',"." }  ,
                               {'f',"..-."},{ 'g',"--." },{ 'h',"...."},{ 'i',".." } , {'j',".---"},
                               {'k',"-.-" }, {'l',".-.."},{ 'm',"--"  }, {'n',"-." } , {'o',"---" },
                               { 'p',".--."},{ 'q',"--.-"},{ 'r',".-." }, {'s',"..."} , {'t',"-"   },
                               {'u',"..-" },{ 'v',"...-"},{ 'w',".--" }, {'x',"-..-"}, {'y',"-.--"},{ 'z',"--.."},
                               { '0',"-----" }, {'1',".----"} ,{ '2',"..---" },{ '3', "...--"}, {'4', "....-" },
                               { '5', "....."},{ '6', "-...."}, {'7', "--..."}, {'8', "---.."}, {'9', "----." }};

QString Encode(QString rawText)
{
    QString codedText;
    QString::iterator rawItr = rawText.begin();
    while(rawItr!=rawText.end())
    {
        if(*rawItr==' '||*rawItr==','||*rawItr=='.') codedText+="/";
        else if((*rawItr>='a'&&*rawItr<='z')||(*rawItr>='0'&&*rawItr<='9'))
        {
            codedText+=MorseTable.value(*rawItr);
            codedText+=' ';
        }
        else codedText += '*';
        rawItr++;
    }
    return codedText;
}

QString Decode(QString codedText)
{
    QString rawText;
    QString::iterator codedItr = codedText.begin();
    QString temp;
    while(codedItr!=codedText.end())
    {
        if(*codedItr=='/')
            rawText+=' ';
        else if(*codedItr=='-'||*codedItr=='.')
        {
            temp+=*codedItr;
        }
        else if(*codedItr==' ')
        {
            rawText+=MorseTable.key(temp);
            temp.clear();
        }
        else rawText+='*';
        codedItr++;
    }
    return rawText;
}

//-------------------------------------------------------------

MCTranslator::MCTranslator(QWidget *parent)
    : QWidget(parent), ui(new Ui::MCTranslator)
{
    ui->setupUi(this);

    canvas = new Canvas(this);
    canvas->setFixedHeight(250); //默认大小是0
    QPalette background;
    background.setColor(QPalette::Background,Qt::black);
    canvas->setAutoFillBackground(true);
    canvas->setPalette(background);
    ui->verticalLayout->addWidget(canvas);


    ui->rawText->setFontPointSize(15);
    ui->codedText->setFontPointSize(15);

}

MCTranslator::~MCTranslator()
{
    delete ui;
}

void MCTranslator::on_btnEncode_clicked()
{
    QString rawText = ui->rawText->toPlainText();
    QString codedText = Encode(rawText.toLower());
    ui->codedText->setText(codedText);
}

void MCTranslator::on_btnDecode_clicked()
{
    QString codedText=ui->codedText->toPlainText();
    QString rawText = Decode(codedText);
    ui->rawText->setText(rawText);
}

void MCTranslator::on_btnPlay_clicked()
{
    canvas->codedText = ui->codedText->toPlainText();
    canvas->animate();
}


void MCTranslator::on_btnAudio_clicked()
{
    canvas->codedText = ui->codedText->toPlainText();
    canvas->music();
}
