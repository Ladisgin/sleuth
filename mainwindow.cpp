#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QSizePolicy>
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow), index_thread(new QThread()), search_thread(new QThread) {
    ui->setupUi(this);

    qRegisterMetaType<std::pair<QString, QVector<QString>>>();

    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(open_file(QTreeWidgetItem*)));

    connect(ui->SelectDirectory, &QPushButton::clicked, this, &MainWindow::select_directory);
    connect(ui->IndexStop, &QPushButton::clicked, this, &MainWindow::stop_index);
    connect(ui->StartStopSearch, &QPushButton::clicked, this, &MainWindow::start_search);
}

MainWindow::~MainWindow() {
    stop_index();
    stop_search();
}

void MainWindow::select_directory() {
    cur_dir = QFileDialog::getExistingDirectory(this, "Select Directory for Scanning",
                                                    QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    ui->label->setText(cur_dir);
    start_index();
}

void MainWindow::start_index() {
    stop_index();

    index_time.start();
    paths_to_tgram.clear();
    index_search* worker = new index_search(mtx, paths_to_tgram, cur_dir);
    worker->moveToThread(index_thread.get());

    connect(worker, SIGNAL(set_max_progress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(set_progress(int)), ui->progressBar, SLOT(setValue(int)));

    connect(index_thread.get(), SIGNAL (started()), worker, SLOT (start_index()));
    connect(worker, SIGNAL (finished()), index_thread.get(), SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(worker, SIGNAL (finished()), this, SLOT (index_end()));

    index_thread->start();
    qDebug() << "index thread start";
}

void MainWindow::start_search() {
//    if(index_thread->isRunning()) {
//        ui->treeWidget->clear();
//        QTreeWidgetItem *ti = new QTreeWidgetItem();
//        ti->setText(0, "Directory now indexing");
//        ui->treeWidget->addTopLevelItem(ti);
//        return;
//    }
    qDebug() << paths_to_tgram.size();
    ui->treeWidget->clear();
    ui->lcdNumber->display(0);
    stop_search();

    search_time.start();
    match_string* worker = new match_string(mtx, paths_to_tgram, ui->MatchString->text());
    worker->moveToThread(search_thread.get());

    connect(worker, SIGNAL(set_max_progress(int)), ui->progressBar, SLOT(setMaximum(int)));
    connect(worker, SIGNAL(set_progress(int)), ui->progressBar, SLOT(setValue(int)));
    connect(worker, SIGNAL(matched(std::pair<QString, QVector<QString>>)), this, SLOT(add_path(std::pair<QString, QVector<QString>>)));

    connect(search_thread.get(), SIGNAL(started()), worker, SLOT(start_search()));
    connect(worker, SIGNAL (finished()), search_thread.get(), SLOT (quit()));
    connect(worker, SIGNAL (finished()), worker, SLOT (deleteLater()));
    connect(worker, SIGNAL (finished()), this, SLOT (search_end()));


    search_thread->start();
    qDebug() << "search thread start";
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
    if(search_thread != nullptr && search_thread->isRunning()){
        search_thread->requestInterruption();
        search_thread->quit();
        search_thread->wait();
        qDebug() << "search canceled";
    }
}

void MainWindow::add_path(std::pair<QString, QVector<QString>> v){
    QTreeWidgetItem *ti = new QTreeWidgetItem();
    ti->setText(0, v.first);
    for (auto const & k: v.second) {
        QTreeWidgetItem *child = new QTreeWidgetItem();
        child->setText(0, k);
        ti->addChild(child);
    }
    ui->treeWidget->addTopLevelItem(ti);

    ui->lcdNumber->display(ui->treeWidget->topLevelItemCount());
}

void MainWindow::index_end() {
    qDebug() << "index end time: " << QTime::fromMSecsSinceStartOfDay(index_time.restart()).toString("HH:mm:ss:zzz");
}

void MainWindow::search_end() {
    qDebug() << "search end time: " << QTime::fromMSecsSinceStartOfDay(search_time.restart()).toString("HH:mm:ss:zzz");
}

void MainWindow::open_file(QTreeWidgetItem *item){
    qDebug() << "+";
    if(item->childCount()){
        QDesktopServices::openUrl(QUrl("file:" + item->text(0)));
    }
}
