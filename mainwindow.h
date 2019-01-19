#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "index_search.h"
#include "match_string.h"
#include "tgram.h"

#include <QMetaType>
#include <QMainWindow>
#include <QThread>
#include <QTime>
#include <QMutex>
#include <QTreeWidgetItem>
#include <QVector>

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

public slots:
    void add_path(std::pair<QString, QVector<QString>>);
    void select_directory();

    void index_end();
    void search_end();
    void open_file(QTreeWidgetItem *item);

private:
    QString cur_dir;

    std::unique_ptr<Ui::MainWindow> ui;
    std::unique_ptr<QThread> index_thread;
    std::unique_ptr<QThread> search_thread;

    QTime index_time;
    QTime search_time;

    QMutex mtx;
    QMap<QString, std::set<tgram>> paths_to_tgram;

    void start_index();
    void start_search();

    void stop_index();
    void stop_search();
};

#endif // MAINWINDOW_H
