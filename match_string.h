#ifndef MATCH_STRING_H
#define MATCH_STRING_H

#include <QObject>
#include <QMap>
#include <QSet>
#include <QByteArray>
#include <QThread>
#include <set>
#include "tgram.h"

class match_string: public QObject {
    Q_OBJECT
public:
    match_string(QMutex &mtx, QMap<QString, std::set<tgram>> const &paths_to_tgram, QString match_s);

signals:
    void search_finished();
    void set_progress(int value);
    void set_max_progress(int value);
    void finished();
    void matched(std::pair<QString, QVector<QString>> v);

public slots:
    void start_search();

private:
    QString match_s;
    QMutex& mtx;
    QMap<QString, std::set<tgram>> const &paths_to_tgram;
};

#endif // MATCH_STRING_H
