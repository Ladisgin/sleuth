#include "index_search.h"

#include <QDirIterator>

index_search::index_search(QObject *parent) : QObject(parent){}


void index_search::start_index(QString const dir_path){
    QDirIterator it(dir_path, QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks | QDir::AllEntries, QDirIterator::Subdirectories);
    while (it.hasNext()) {
        if (QThread::currentThread()->isInterruptionRequested()) {
            return;
        }
        QFile file(it.next());
        QFileInfo fi(file);
        if(fi.permission(QFile::ReadUser)) {
            if(fi.isFile()) {
                add_to_map(file);
            }
        } else {
            //do it later or not
        }
    }
}

void index_search::changed_index(QString const dir_path) {

}

void index_search::start_search(QString const matched) {

}

void index_search::add_to_map(QFile const &file) {

}
