#include "index_search.h"

#include <QDirIterator>
#include <QMimeType>
#include <QMimeDatabase>
#include <QDebug>
#include <QVector>

#include <fstream>
#include <set>
#include <stdio.h>

index_search::index_search(QMutex &mtx, QMap<QString, std::set<tgram>> &paths_to_tgram, QString const dir_path) : fsw(new QFileSystemWatcher), mtx(mtx), paths_to_tgram(paths_to_tgram), dir_path(dir_path){
    connect(fsw.get(), SIGNAL(directoryChanged(QString)), this, SLOT(start_index(QString)));
    connect(fsw.get(), SIGNAL(fileChanged(QString)), this, SLOT(add_to_map(QString)));
}

index_search::~index_search(){}

void index_search::start_index(){
    start_index(dir_path);
}

void index_search::start_index(QString cur_path) {
    QDirIterator it(cur_path, QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks | QDir::AllEntries, QDirIterator::Subdirectories);
    emit set_max_progress(0);
    emit set_progress(0);

    QVector<QString> paths;
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        QFileInfo fi(it.next());
        if(fi.permission(QFile::ReadUser)) {
            if(fi.isFile()) {
                paths.push_back(fi.filePath());
            }
            fsw->addPath(fi.filePath());
        } else {
            //do it later or not
        }
    }

    emit set_max_progress(paths.size());

    for(int i = 0; i< paths.size(); ++i) { //size int
        if (QThread::currentThread()->isInterruptionRequested()) {
            break;
        }
        emit set_progress(i);
        add_to_map(paths[i]);
    }

    emit set_max_progress(1);
    emit set_progress(1);
    qDebug() << "index end";
//    emit finished();
}


void index_search::add_to_map(QString const &path) {
    static QMimeDatabase mimeDatabase;
    const QMimeType mimeType = mimeDatabase.mimeTypeForFile(path);
    if (mimeType.isValid() && !mimeType.inherits(QStringLiteral("text/plain"))) { //QT can check is file text :(
        return;
    }

    static std::set<char> non_valid_char = {0, 8, 24, 0x7F};
    std::ifstream fin(path.toStdString(), std::ios::in | std::ios::binary);
    if (!fin) {
        return;
    }

    std::vector<char> buffer(READ_BLOCK);
    std::set<tgram> tgram_set;
    size_t count = 0;
//    size_t dependent_count = 0;
    bool flag = true;
    char last[2] = {0, 0};
    while (!fin.eof() && flag) {
        fin.read(buffer.data(), READ_BLOCK);

        count = static_cast<size_t>(fin.gcount());

        for (size_t i = 0; i < count; ++i) {
//            if(dependent_count){
//                if((static_cast<unsigned char>(buffer[i]) >> 6) != 0x2){
//                    flag = false;
//                    break;
//                }
//                --dependent_count;
//            } else {
//                if((static_cast<unsigned char>(buffer[i]) >> 3) == 0x1E) {
//                    dependent_count += 3;
//                } else if((static_cast<unsigned char>(buffer[i]) >> 4) == 0xE) {
//                    dependent_count += 2;
//                } else if((static_cast<unsigned char>(buffer[i]) >> 5) == 0x6) {
//                    dependent_count += 1;
//                } else if((static_cast<unsigned char>(buffer[i]) >> 7) != 0x0 || non_valid_char.count(buffer[i])) {
//                    flag = false;
//                    break;
//                }
//            }
            if(i >= 2) {
                tgram_set.insert(tgram(buffer[i-2], buffer[i-1], buffer[i]));
            } else if(last[0] != 0){
                tgram_set.insert(tgram(last[0], last[1], buffer[i]));
                last[0] = last[1];
                last[1] = buffer[i];
            }
        }

        last[0] = buffer[count - 2];
        last[1] = buffer[count - 1];

        if(tgram_set.size() > 20000){
            flag = false;
        }
    }

    if(flag) {
        mtx.lock();
        paths_to_tgram.insert(path, tgram_set);
        mtx.unlock();
    }

    fin.close();
}
