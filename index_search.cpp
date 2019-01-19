#include "index_search.h"

#include <QDirIterator>

#include <fstream>
#include <set>

index_search::index_search(QMutex &mtx, QMap<QString, QSet<tgram>> &paths_to_tgram) : mtx(mtx), paths_to_tgram(paths_to_tgram){}


void index_search::start_index(QString const dir_path){
    QDirIterator it(dir_path, QDir::NoDotAndDotDot | QDir::Hidden | QDir::NoSymLinks | QDir::AllEntries, QDirIterator::Subdirectories);
    emit set_max_progress(0);
    emit set_progress(0);
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
    emit set_max_progress(1);
    emit set_progress(1);
    emit finished();
}

void index_search::changed_index(QString const dir_path) {
    //do it later
}

void index_search::add_to_map(QFile const &file) {
    static std::set<char> non_valid_char = {0};
    std::ifstream fin(file.fileName().toStdString(), std::ios::in | std::ios::binary);
    if (!fin) {
        return;
    }
    std::vector<char> buffer(READ_BLOCK);

    fin.read(buffer.data(), READ_BLOCK);

    QSet<tgram> tgram_set;
    size_t count = 0;
    size_t dependent_count = 0;
    bool flag = true;
    char last[2];

    while (!fin.eof() && flag) {
        fin.read(buffer.data(), READ_BLOCK);

        count = static_cast<size_t>(fin.gcount());

        for (size_t i = 0; i < count; ++i) {
            if(dependent_count){
                if((static_cast<unsigned char>(buffer[i]) >> 6) != 0x2){
                    flag = false;
                    break;
                }
                --dependent_count;
            } else {
                if((static_cast<unsigned char>(buffer[i]) >> 3) == 0x1E) {
                    dependent_count += 3;
                } else if((static_cast<unsigned char>(buffer[i]) >> 4) == 0xE) {
                    dependent_count += 2;
                } else if((static_cast<unsigned char>(buffer[i]) >> 5) == 0x6) {
                    dependent_count += 1;
                } else if((static_cast<unsigned char>(buffer[i]) >> 7) != 0x0 || non_valid_char.count(buffer[i])) {
                    flag = false;
                    break;
                }
            }
            if(i >= 2) {
                tgram_set.insert(tgram(buffer[i-2], buffer[i-1], buffer[i]));
            } else {
                tgram_set.insert(tgram(last[0], last[1], buffer[i]));
                last[0] = last[1];
                last[1] = buffer[i];
            }
        }

        last[0] = buffer[count - 2];
        last[1] = buffer[count - 1];

        if(tgram_set.size() > 40000){
            flag = false;
        }
    }

    if(flag) {
        mtx.lock();
        paths_to_tgram.insert(file.fileName(), tgram_set);
        mtx.unlock();
    }

    fin.close();
}
