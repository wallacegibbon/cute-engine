#pragma once

#include "PathingMap.h"
#include "Vendor.h"

namespace cute {

class Worker : public QObject {
    Q_OBJECT

public slots:
    void find_path(const PathingMap &pathing_map, const QPointF &start, const QPointF &end);

signals:
    void path_found(std::vector<QPointF> result);
};

class AsyncShortestPathFinder : public QObject {
    Q_OBJECT

public:
    AsyncShortestPathFinder();
    ~AsyncShortestPathFinder();

signals:
    void path_found(std::vector<QPointF> path);
    void find_path(const PathingMap &pathing_map, const QPointF &start, const QPointF &end);

public slots:
    void on_path_found(std::vector<QPointF> path);

private:
    Worker worker_;
    QThread worker_thread_;
};

} // namespace cute
