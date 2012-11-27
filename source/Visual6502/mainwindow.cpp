#include "mainwindow.h"
#include "ui_mainwindow.h"

#define MAX_BLOCKS 1024
#define BLOCK_DIM  10
#define ROW_BLOCKS (320/BLOCK_DIM)
#define COL_BLOCKS (320/BLOCK_DIM)

void fillMemory(QGraphicsScene* scene)
{
    QPen blockPen(Qt::SolidLine);
    QBrush blockBrush(Qt::SolidPattern);
    blockBrush.setColor(Qt::black);

    for(int i = 0; i < ROW_BLOCKS; i++)
    {
        for(int j = 0; j < COL_BLOCKS; j++)
        {
            ((j%2) ? blockBrush.setColor(Qt::red) : blockBrush.setColor(Qt::black));
            scene->addRect(i*BLOCK_DIM, j*BLOCK_DIM, BLOCK_DIM, BLOCK_DIM, blockPen, blockBrush);
        }

    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    theCpu = new CPU(&theMem);
    ui->setupUi(this);
    QGraphicsScene* memScene;

    memScene = new QGraphicsScene(0, 0, 320, 320);
    ui->memoryView->setScene(memScene);

    fillMemory(memScene);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::addStatusLine(QString toAdd)
{
    ui->txtStatus->appendPlainText(toAdd);
   // QScrollBar *tmpSb = ui->txtStatus->verticalScrollBar();

}

void MainWindow::on_btnAssemble_clicked()
{
    ui->txtStatus->setPlainText("");
    //attempt an assemble.
    addStatusLine("Assembling...");
    if(ui->codeEdit->toPlainText() == "")
    {
        addStatusLine("Error: No code set!");
        return;
    }

    string asmData = ui->codeEdit->toPlainText().toAscii().constData();
    //addStatusLine("Done");
    asmber.setText(asmData);
    //set offset
    asmber.setOffset(0x600);    //starting here
    int res = asmber.assemble();
    if(res == -1)
    {
        //we failed somehow.
        stack<string>* errors = asmber.getErrors();
        while(!errors->empty())
        {
            QString tmp = ((string)("Asm: " + errors->top())).c_str();
            addStatusLine(tmp);
            errors->pop();
        }
        return;
    }
    //announce complete + data
    superSS << "Generated  " << res << " bytes!" << endl << "Hex: ";
    addStatusLine(QString(superSS.str().c_str()));
    superSS.flush();
    superSS.str("");

    byte* codePtr = asmber.getBinary();
    for(int i = 0; i < res; i++)
    {
        if(!(i % 0xA))
        {
            if(i)
                superSS << endl;
            superSS << setw(4) << hex << setfill('0') << i << ": ";
        }
        superSS << setw(2) << hex << setfill('0') << (short)*codePtr << " ";
        codePtr++;
    }
    addStatusLine(QString(superSS.str().c_str()));
    superSS.str("");
    return;
}
