#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QColor>
#include <QGraphicsScene>
#include <QGraphicsRectItem>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <QFuture>
#include <QtConcurrentRun>
#include <stdlib.h>
#include <time.h>
#include "../assembler/assembler.h"
#include "../cpu/cpu.h"
#include "../mmc/basicmemory.h"

#define MAX_BLOCKS 1024
#define BLOCK_DIM  10
#define ROW_BLOCKS (320/BLOCK_DIM)
#define COL_BLOCKS (320/BLOCK_DIM)

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_btnAssemble_clicked();
    void addStatusLine(QString toAdd);

    void on_btnStep_clicked();

    void on_btnExecute_clicked();
    void updateMemory();
private:
    void genRandom(byte addr);
    void keyPressEvent(QKeyEvent *);
    void updateRegs();
    void fillMem();
    void initColors();
    void execute();
    unsigned short lastMem;
    Ui::MainWindow *ui;
    Assembler   asmber;
    CPU*        theCpu;
    BasicMemory theMem;
    stringstream superSS;
    QGraphicsScene* memScene;
    QGraphicsRectItem* rects[MAX_BLOCKS];
    vector<QColor*> colors;
    //QString      statusString;

};

#endif // MAINWINDOW_H
