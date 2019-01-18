#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QSizePolicy>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), thread(new QThread())  {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
}

void MainWindow::stop() {
    if(thread != nullptr && thread->isRunning()){
        thread->requestInterruption();
        thread->quit();
        thread->wait();
        qDebug() << "process canceled";
    }
}
