#ifndef INDEX_SEARCH_H
#define INDEX_SEARCH_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QSet>
#include <QThread>
#include <QFile>

#include "tgram.h"

const size_t READ_BLOCK = 4096 * 4;

class index_search : public QObject {
    Q_OBJECT
public:
    index_search(QMutex &mtx, QMap<QString, QSet<tgram>> &paths_to_tgram);

signals:
    void index_finished();
    void set_progress(int value);
    void set_max_progress(int value);
    void finished();

public slots:
    void start_index(QString const dir_path);
    void changed_index(QString const dir_path);

private:
    QMutex& mtx;
    QMap<QString, QSet<tgram>> &paths_to_tgram;

    void add_to_map(QFile const &path);
};

#endif // INDEX_SEARCH_H
