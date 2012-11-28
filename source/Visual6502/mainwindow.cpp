#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::initColors()
{
    //first is gonna be black.

    colors.push_back(new QColor("#000000")); // 0 == black
    colors.push_back(new QColor("#FFFFFF")); // 1 == white
    colors.push_back(new QColor("#838B8B")); // 2 == dark grey
    colors.push_back(new QColor("#CCCCCC")); // 3 == light grey
    colors.push_back(new QColor("#CD3333")); // 4 == red
    colors.push_back(new QColor("#76EEC6")); // 5 == cyan
    colors.push_back(new QColor("#C13100")); // 6 == blood orange
    colors.push_back(new QColor("#0000CD")); // 7 == blue
    colors.push_back(new QColor("#66CD00")); // 8 == green
    colors.push_back(new QColor("#EE7621")); // 9 == orange
    colors.push_back(new QColor("#87CEEB")); // A == light blue
    colors.push_back(new QColor("#EEAD0E")); // B == gold
    colors.push_back(new QColor("#68228B")); // C == purple
    colors.push_back(new QColor("#FFFF00")); // D == yellow
    colors.push_back(new QColor("#8B4513")); // E == brown
    colors.push_back(new QColor("#228B22")); // F == forest green

}

void MainWindow::updateRegs()
{
    superSS.flush();
    superSS.str("");
    if(!theCpu)
    {
        //we don't have a CPU instantiated.
        return;
    }
    superSS << "$" << setw(2) << hex << setfill('0') << (int)theCpu->A;
    ui->lineAReg->setText(QString(superSS.str().c_str()));
    superSS.str("");

    superSS << "$" << setw(2) << hex << setfill('0') << (int)theCpu->X;
    ui->lineXReg->setText(QString(superSS.str().c_str()));
    superSS.str("");

    superSS << "$" << setw(2) << hex << setfill('0') << (int)theCpu->Y;
    ui->lineYReg->setText(QString(superSS.str().c_str()));
    superSS.str("");

    superSS << "$" << setw(2) << hex << setfill('0') << (int)theCpu->SP;
    ui->lineSPReg->setText(QString(superSS.str().c_str()));
    superSS.str("");


    superSS << "$" << setw(4) << hex << setfill('0') << (int)theCpu->PC;
    ui->linePCReg->setText(QString(superSS.str().c_str()));
    superSS.str("");

    //update status bits
    ui->lineCarry->setText("0");
    ui->lineZero->setText("0");
    ui->lineInt->setText("0");
    ui->lineDecimal->setText("0");
    ui->lineBreak->setText("0");
    ui->lineOver->setText("0");
    ui->lineNeg->setText("0");

    if(theCpu->carryFlag)
        ui->lineCarry->setText("1");
    if(theCpu->zeroFlag)
        ui->lineZero->setText("1");
    if(theCpu->intFlag)
        ui->lineInt->setText("1");
    if(theCpu->decFlag)
        ui->lineDecimal->setText("1");
    if(theCpu->brkFlag)
        ui->lineBreak->setText("1");
    if(theCpu->overFlag)
        ui->lineOver->setText("1");
    if(theCpu->signFlag)
        ui->lineNeg->setText("1");


}

void MainWindow::fillMem()
{
    QPen blockPen(Qt::SolidLine);
    QBrush blockBrush(Qt::SolidPattern);
    //blockBrush.setColor(colors[0]);
    memScene->clear();

    for(int i = 0; i < ROW_BLOCKS; i++)
    {
        for(int j = 0; j < COL_BLOCKS; j++)
        {
            byte cellData = theMem.loadByte(0x200 + (i * ROW_BLOCKS) + j);
            //((j%2) ? blockBrush.setColor(Qt::red) : blockBrush.setColor(Qt::black));
            blockBrush.setColor(*(colors[cellData&0xF]));
            rects[(i* ROW_BLOCKS) + j] = memScene->addRect(j*BLOCK_DIM, i*BLOCK_DIM, BLOCK_DIM, BLOCK_DIM, blockPen, blockBrush);
        }

    }
}

void MainWindow::updateMemory()
{
   lastMem = theMem.lastAddr;
    if(lastMem >= 0x200 && lastMem < 0x600)
    {
        lastMem -= 0x200;
        byte cellData = theMem.loadByte(lastMem + 0x200);

        QBrush tmpBrush(*colors[cellData&0xF]);
        //update cell according to theMem.lastAddr
        (rects[lastMem])->setBrush(tmpBrush);
        ui->memoryView->repaint();
    }

    return;
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //theMem.memChange = (void*)&MainWindow::updateMemory;
    theCpu = new CPU(&theMem);
    ui->setupUi(this);
    memScene = new QGraphicsScene(0, 0, 320, 320);

    ui->memoryView->setScene(memScene);
    initColors();
    fillMem();
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

void MainWindow::execute()
{
    int tmp = 0;
    theCpu->PC = theCpu->codeBegin;

    while(tmp != -1 && !theCpu->brkFlag)
    {
        tmp = theCpu->step();
        updateRegs();
        updateMemory();
        this->repaint();
    }
    addStatusLine("Halted.");
    return;
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
            superSS << setw(4) << hex << setfill('0') << i + 0x600 << ": ";
        }
        superSS << setw(2) << hex << setfill('0') << (short)*codePtr << " ";
        codePtr++;
    }
    addStatusLine(QString(superSS.str().c_str()));
    superSS.str("");
    theMem.loadProgram(0x600, asmber.getBinary(), res);
    theCpu->codeBegin = 0x600;
    theCpu->codeEnd = 0x600 + res;
    theCpu->PC = 0x600;
    theCpu->clearFlags();
    updateRegs();
    return;
}

void MainWindow::on_btnStep_clicked()
{
    int res = theCpu->step();
    if(res == -1)
    {
        addStatusLine("Cannot step further, resetting");
        theCpu->PC = theCpu->codeBegin;
        theCpu->clearFlags();
        theCpu->clearRegs();
        theCpu->step();
    }
    if(theCpu->brkFlag)
    {
        addStatusLine("Break flag set! Halted.");
        return;
    }
    updateRegs();
    updateMemory();
}

void MainWindow::on_btnExecute_clicked()
{
    QFuture<void> future = QtConcurrent::run(MainWindow::execute);
}
