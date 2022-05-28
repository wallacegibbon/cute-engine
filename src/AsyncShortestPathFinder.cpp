#include "AsyncShortestPathFinder.h"

using namespace cute;

/// Calculates a Path from a starting point to an ending point in the specified PathingMap.
void Worker::find_path(const PathingMap &pathing_map, const QPointF &start, const QPointF &end) {
    std::vector<QPointF> path_points;
    path_points = pathing_map.shortest_path(start, end);
    emit path_found(path_points);
}

AsyncShortestPathFinder::AsyncShortestPathFinder() {
    worker_.moveToThread(&worker_thread_);
    connect(&worker_, &Worker::path_found, this, &AsyncShortestPathFinder::on_path_found);
    connect(this, &AsyncShortestPathFinder::find_path, &worker_, &Worker::find_path);
    worker_thread_.start();
}

AsyncShortestPathFinder::~AsyncShortestPathFinder() {
    worker_thread_.quit();
    /// TODO: the main thread sometimes blocks here (when the pathmap of the entity is big), fix the bug
    /// I guess the reason is that `pathing_map.shortest_path` never returns in the child thread.
    /// if the path can not be calculated by the A* algorighm, the `shortest_path` function will not return.
    worker_thread_.wait();
}

void AsyncShortestPathFinder::on_path_found(std::vector<QPointF> path) { emit path_found(path); }
