#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "../assembler/assembler.h"
#include "../cpu/cpu.h"
#include "../mmc/basicmemory.h"

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

private:
    Ui::MainWindow *ui;
    Assembler   asmber;
    CPU*        theCpu;
    BasicMemory theMem;
    stringstream superSS;
    //QString      statusString;

};

#endif // MAINWINDOW_H
