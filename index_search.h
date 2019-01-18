#ifndef INDEX_SEARCH_H
#define INDEX_SEARCH_H

#include <QObject>
#include <QString>
#include <QVector>
#include <QMap>
#include <QThread>
#include <QFile>

const size_t READ_BLOCK = 4096 * 4;

struct tgram {
    char gram[3];
};

inline uint qHash(tgram* g) {
    return (static_cast<uint>(g->gram[0]) << (std::numeric_limits<decltype(g->gram[0])>::digits >> 1)) ^
            (static_cast<uint>(g->gram[1]) << std::numeric_limits<decltype(g->gram[0])>::digits) ^
            static_cast<uint>(g->gram[2]);
}

bool operator==(tgram const & l, tgram const & r){
    return (l.gram[0] == r.gram[0] && l.gram[1] == r.gram[1] && l.gram[2] == r.gram[2]);
}


class index_search : public QObject {
    Q_OBJECT
public:
    explicit index_search(QObject *parent = nullptr);

signals:
    void index_finished();
    void search_result(QVector<QString> ans);
    void search_finished();
    void set_progress(int value);
    void set_max_progress(int value);
    void finished();

public slots:
    void start_index(QString const dir_path);
    void changed_index(QString const dir_path);
    void start_search(QString const matched);

private:
    QMap<QString, QSet<tgram>> paths_to_tgram;

    void add_to_map(QFile const &path);
};

#endif // INDEX_SEARCH_H
