#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "index_search.h"

#include <QMainWindow>
#include <QThread>
#include <QTime>


#include <memory>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QThread> thread;
    QTime time;

    void stop();
};

#endif // MAINWINDOW_H
