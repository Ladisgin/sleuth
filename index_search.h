#ifndef INDEX_SEARCH_H
#define INDEX_SEARCH_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QThread>
#include <QFile>
#include <QFileSystemWatcher>
#include <memory>
#include <limits>
#include <set>
#include "tgram.h"

const size_t READ_BLOCK = 4096 * 16;

class index_search : public QObject {
    Q_OBJECT
public:
    index_search(QMutex &mtx, QMap<QString, std::set<tgram>> &paths_to_tgram, QString dir_path);
    ~index_search();
signals:
    void index_finished();
    void set_progress(int value);
    void set_max_progress(int value);
    void finished();

public slots:
    void start_index();
    void start_index(QString path);
    void changed_index();
    void add_to_map(QString const &path);

private:
    std::unique_ptr<QFileSystemWatcher> fsw;
    QMutex& mtx;
    QMap<QString, std::set<tgram>> &paths_to_tgram;
    QString dir_path;

};

#endif // INDEX_SEARCH_H
