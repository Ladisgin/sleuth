#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "index_search.h"
#include "tgram.h"

#include <QMainWindow>
#include <QThread>
#include <QTime>
#include <QMutex>

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
    std::unique_ptr<QThread> index_thread;
    std::unique_ptr<QThread> search_thread;
    QTime time;


    QMutex mtx;
    QMap<QString, QSet<tgram>> paths_to_tgram;

    void start_index();
    void start_search();

    void stop_index();
    void stop_search();
};

#endif // MAINWINDOW_H
