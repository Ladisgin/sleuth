#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QSizePolicy>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), index_thread(new QThread()), search_thread(new QThread) {
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
}
void MainWindow::start_index() {

}

void MainWindow::start_search() {

}

void MainWindow::stop_index() {
    if(index_thread != nullptr && index_thread->isRunning()){
        index_thread->requestInterruption();
        index_thread->quit();
        index_thread->wait();
        qDebug() << "index canceled";
    }
}

void MainWindow::stop_search() {
    if(index_thread != nullptr && index_thread->isRunning()){
        index_thread->requestInterruption();
        index_thread->quit();
        index_thread->wait();
        qDebug() << "search canceled";
    }
}
