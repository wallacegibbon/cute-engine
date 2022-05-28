#include "Game.h"
#include "Entity.h"
#include "GUI.h"
#include "Map.h"
#include "MapGrid.h"
#include "QtUtilities.h"
#include "stl_helper.h"

using namespace cute;

Game *Game::game = nullptr;

Game::Game(MapGrid *map_grid, int x_pos_of_starting, int y_pos_of_starting) : map_grid_(map_grid) {
    if (game != nullptr) {
        std::logic_error("Game is a singleton. You can only construct one instance.");
    }
    Game::game = this;

    gui_layer_ = new QGraphicsRectItem();
    gui_layer_->setZValue(INFINITY);

    /// register types that needed to be used in cross thread signal-slot stuff
    qRegisterMetaType<PathingMap>("PathingMap");
    qRegisterMetaType<PathingMap>("PathingMap&");
    qRegisterMetaType<std::vector<QPointF>>();

    for (Map *map : map_grid_->maps()) {
        map->set_game(this);
    }
    setMouseTracking(true);

    current_map_ = map_grid_->map_at(x_pos_of_starting, y_pos_of_starting);
    setScene(current_map_->scene());
    scene()->addItem(gui_layer_);

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    gui_update_timer_ = new QTimer(this);
    connect(gui_update_timer_, &QTimer::timeout, this, &Game::update_GUI_positions);
    gui_update_timer_->start(0);

    /// When ECCameraFollower is being used, the GUI position should be updated along with camera move.
    /// Or annoying flash will appear.
    /// (The previous connect to gui_update_timer_ will update the postion on the end of the event loop,
    ///  which will delay the GUI postion updating and create a flash.)
    connect(this, &Game::cam_moved, this, &Game::update_GUI_positions);

    set_mouse_mode(MouseMode::Regular);
}

void Game::launch() {
    /// TODO: eventually should showFullscreen() (or parametrize to allow launching normal or full screen
    showNormal();
    setFixedSize(1280, 720);
    // showFullScreen();

    /// view->setSceneRect have higher priority over scene->setSceneRect
    setSceneRect(0, 0, width(), height());
}

void Game::set_current_map(Map *map) {
    assert(map != nullptr);

    /// The gui_layer_ should be moved out from the old map and moved into the new map.
    scene()->removeItem(gui_layer_);

    Map *old_map = current_map_;
    current_map_ = map;

    setScene(current_map_->scene());
    map->set_game(this);

    scene()->addItem(gui_layer_);

    /// emit map changed signal
    if (old_map != current_map_) {
        emit map_changed(old_map, map);
    }
}

void Game::set_center_cam_pos(QPointF position) {
    /// the size of `sceneRect` will always keep the same as the size of the view, so you can also use
    /// `width()` directly here to take the place of `sceneRect().width()`.
    double cam_width = sceneRect().width();
    double cam_height = sceneRect().height();
    setSceneRect(position.x() - cam_width / 2, position.y() - cam_height / 2, cam_width, cam_height);

    /// you can also set camera right in the following way.
    /// (any number less than view's width/height is okay, just make sure the offset change along with size.)
    /// but other things like rain whether will go wrong
    // setSceneRect(position.x() - 200 / 2, position.y() - 200 / 2, 200, 200);

    emit cam_moved(position);
}

QPointF Game::center_cam_pos() { return sceneRect().center(); }

QRectF Game::cam() { return sceneRect(); }

void Game::move_cam(QVector2D by_vector) {
    double new_x = sceneRect().topLeft().x() + by_vector.x();
    double new_y = sceneRect().topLeft().y() + by_vector.y();
    setSceneRect(new_x, new_y, sceneRect().width(), sceneRect().height());
    emit cam_moved(center_cam_pos());
}

void Game::move_cam_up(double by_amount) { move_cam(QVector2D(0, -1 * by_amount)); }
void Game::move_cam_down(double by_amount) { move_cam(QVector2D(0, by_amount)); }
void Game::move_cam_left(double by_amount) { move_cam(QVector2D(-1 * by_amount, 0)); }
void Game::move_cam_right(double by_amount) { move_cam(QVector2D(by_amount, 0)); }

void Game::mousePressEvent(QMouseEvent *event) {
    /// do normal QGraphicsView event handling
    /// (it will pass it to the QGraphicsScene, which will pass to QGraphicsItems at location).
    QGraphicsView::mousePressEvent(event);

    /// if event was handled (by one of the QGraphicsItems), we're done
    if (event->isAccepted()) {
        return;
    }

    if (event->button() == Qt::MouseButton::RightButton &&
        (mouse_mode_ == MouseMode::SelectPosition || mouse_mode_ == MouseMode::SelectEntity)) {
        emit select_canceled();
        return;
    }

    if (mouse_mode_ == MouseMode::SelectPosition) {
        emit position_selected(map_to_map(event->pos()));
        return;
    }
    if (mouse_mode_ == MouseMode::SelectEntity) {
        std::unordered_set<Entity *> entities = current_map()->entities(map_to_map(event->pos()));
        if (entities.size() > 0) {
            emit entity_selected(*entities.begin());
            return;
        }
    }

    /// if execution got to here, mouse_mode_ must be Regular, just emit mouse_pressed event
    emit mouse_pressed(event->pos(), event->button());
}

void Game::mouseMoveEvent(QMouseEvent *event) {
    /// do normal QGraphicsView event handling (it will pass the even to QGraphicsItems)
    QGraphicsView::mouseMoveEvent(event);

    /// if one of the QGraphicsItems handled the event, we're done
    if (event->isAccepted()) {
        return;
    }
    /// otherwise, simply emit mouseMove signal
    emit mouse_moved(event->pos());
}

void Game::mouseReleaseEvent(QMouseEvent *event) {
    /// do normal QGraphicsView event handling (passing to QGraphicsItems)
    QGraphicsView::mouseReleaseEvent(event);

    /// if one of the QGraphicsItems handled the event, we're done
    if (event->isAccepted()) {
        return;
    }
    /// otherwise, simply emit mouse_released event
    emit mouse_released(event->pos(), event->button());
}

bool Game::is_key_event_to_ignore(QKeyEvent *event) {
    return event->isAutoRepeat() || event->key() == Qt::Key_Alt || event->key() == Qt::Key_AltGr;
}

void Game::keyPressEvent(QKeyEvent *event) {
    if (is_key_event_to_ignore(event)) {
        return;
    }
    key_pressed_.insert(event->key());
    emit key_pressed(event->key());
}

void Game::keyReleaseEvent(QKeyEvent *event) {
    if (is_key_event_to_ignore(event)) {
        return;
    }
    key_pressed_.erase(event->key());
    emit key_released(event->key());
}

void Game::focusOutEvent(QFocusEvent *event) { key_pressed_.clear(); }

void Game::add_GUI(GUI *gui) {
    if (contains_GUI(gui)) {
        return;
    }
    gui->game_ = this;
    gui->get_graphics_item()->setParentItem(gui_layer_);
    gui_s_.insert(gui);

    /// is this necessary ?
    // gui->get_graphics_item()->setVisible(true);
}

void Game::remove_GUI(GUI *gui) {
    if (!contains_GUI(gui)) {
        return;
    }
    gui->set_parent_GUI(nullptr);
    scene()->removeItem(gui->get_graphics_item());
    gui_s_.erase(gui);
}

bool Game::contains_GUI(GUI *gui) { return stl_helper::contains(gui_s_, gui); }

QPointF Game::map_to_map(const QPoint &point) { return mapToScene(point); }
QPointF Game::map_from_map(const QPointF &point) { return mapFromScene(point); }

QPoint Game::get_mouse_pos() { return mapFromGlobal(cursor().pos()); }
QPointF Game::get_mouse_pos_in_map() { return map_to_map(get_mouse_pos()); }

void Game::set_mouse_mode(Game::MouseMode mode) {
    mouse_mode_ = mode;
    switch (mode) {
    case MouseMode::Regular:
        setCursor(Qt::ArrowCursor);
        break;
    case MouseMode::SelectPosition:
        setCursor(Qt::CrossCursor);
        break;
    case MouseMode::SelectEntity:
        QCursor cur(QPixmap(":/cute-engine-builtin/resources/graphics/misc/target_mouse_cursor.png"));
        setCursor(cur);
    }
}

void Game::add_watched_entity(Entity *watched, Entity *watching, double range) {
    assert(watched != nullptr && watching != nullptr);

    std::pair<Entity *, Entity *> watched_watching_pair = std::make_pair(watched, watching);
    auto watching_set = watched_to_watching_.find(watched);
    auto watched_set = watching_to_watched_.find(watching);

    /// simply update range if this watched-watching pair already exists
    if (watched_watching_pair_exists(watched, watching)) {
        watched_watching_pair_to_range_[watched_watching_pair] = range;
        return;
    }

    /// if this is the first time the "watched" entity is being watched, add a set for it
    if (watching_set == watched_to_watching_.end()) {
        watched_to_watching_[watched] = std::set<Entity *>();
    }

    /// if this is the first time the watching entity is watching anything, add a set for it
    if (watched_set == watching_to_watched_.end()) {
        watching_to_watched_[watching] = std::set<Entity *>();
    }

    watched_to_watching_[watched].insert(watching);
    watching_to_watched_[watching].insert(watched);

    /// add range data
    watched_watching_pair_to_range_[watched_watching_pair] = range;

    /// add emitted data (initially not emitted)
    watched_watching_pair_to_enter_range_emitted_[watched_watching_pair] = false;

    /// listen to when either the watched or watching entities die (so we can update book keeping)
    connect(watched, &QObject::destroyed, this, &Game::on_watched_entity_dies);
    connect(watching, &QObject::destroyed, this, &Game::on_watching_entity_dies);
}

bool Game::watched_watching_pair_exists(Entity *watched, Entity *watching) {
    assert(watched != nullptr && watching != nullptr);

    auto watching_set = watched_to_watching_.find(watched);
    if (watching_set != watched_to_watching_.end()) {
        if (watching_set->second.find(watching) != watching_set->second.end()) {
            return true;
        }
    }
    return false;
}

void Game::remove_watched_entity(Entity *watched, Entity *watching) {
    assert(watched != nullptr && watching != nullptr);
    assert(watched_watching_pair_exists(watched, watching));

    std::pair<Entity *, Entity *> watched_watching_pair = std::make_pair(watched, watching);
    auto watching_set = watched_to_watching_.find(watched)->second;
    auto watched_set = watching_to_watched_.find(watching)->second;
    auto watcher = watching_set.find(watching);
    auto watchee = watched_set.find(watched);

    watching_set.erase(watcher);
    watched_set.erase(watchee);
    watched_watching_pair_to_range_.erase(watched_watching_pair);
    watched_watching_pair_to_enter_range_emitted_.erase(watched_watching_pair);
}

void Game::remove_watched_entity(Entity *watched) {
    assert(watched != nullptr);
    std::set<Entity *> watching_set = watching_entities(watched);
    for (Entity *entity : watching_set) {
        remove_watched_entity(watched, entity);
    }
}

void Game::remove_watching_entity(Entity *watching) {
    std::set<Entity *> entities = watched_entities(watching);
    for (Entity *entity : entities) {
        remove_watched_entity(entity, watching);
    }
}

std::set<Entity *> Game::watched_entities() {
    std::set<Entity *> results;
    for (auto pair : watched_to_watching_) {
        results.insert(pair.first);
    }
    return results;
}

std::set<Entity *> Game::watched_entities(Entity *watching_entity) {
    assert(watching_entity != nullptr);
    auto the_set = watching_to_watched_.find(watching_entity);
    if (the_set == watching_to_watched_.end()) {
        return std::set<Entity *>();
    } else {
        return the_set->second;
    }
}

std::set<Entity *> Game::watching_entities() {
    std::set<Entity *> results;
    for (auto pair : watching_to_watched_) {
        results.insert(pair.first);
    }
    return results;
}

std::set<Entity *> Game::watching_entities(Entity *watched_entity) {
    assert(watched_entity != nullptr);
    auto the_set = watched_to_watching_.find(watched_entity);
    if (the_set == watched_to_watching_.end()) {
        return std::set<Entity *>();
    } else {
        return the_set->second;
    }
}

double Game::watched_watching_range(Entity *watched, Entity *watching) {
    assert(watched != nullptr && watching != nullptr);
    assert(watched_watching_pair_exists(watched, watching));
    return watched_watching_pair_to_range_[std::make_pair(watched, watching)];
}

void Game::set_watched_watching_range(Entity *watched, Entity *watching, double range) {
    assert(watched != nullptr && watching != nullptr);
    assert(range >= 0);
    assert(watched_watching_pair_exists(watched, watching));
    watched_watching_pair_to_range_[std::make_pair(watched, watching)] = range;
}

DiplomacyManager &Game::diplomacy_manager() { return diplomacy_manager_; }

void Game::update_GUI_positions() { gui_layer_->setPos(mapToScene(QPoint(0, 0))); }

bool Game::handle_watched_watching_pair(Entity *watched_entity, Entity *watching_entity) {
    double dist = QtUtils::distance(watched_entity->pos(), watching_entity->pos());
    double range = watched_watching_range(watched_entity, watching_entity);

    std::pair<Entity *, Entity *> watched_watching_pair = std::make_pair(watched_entity, watching_entity);
    bool already_in_range = watched_watching_pair_to_enter_range_emitted_[watched_watching_pair];

    bool emitted_something = false;
    if (dist < range && !already_in_range) {
        emit watched_entity_enters_range(watched_entity, watching_entity, range);
        watched_watching_pair_to_enter_range_emitted_[watched_watching_pair] = true;
        emitted_something = true;
    } else if (dist > range && already_in_range) {
        emit watched_entity_leaves_range(watched_entity, watching_entity, range);
        watched_watching_pair_to_enter_range_emitted_[watched_watching_pair] = false;
        emitted_something = true;
    }
    return emitted_something;
}

void Game::on_entity_moved(Entity *entity) {
    assert(entity != nullptr);
    /// if the entity that just moved is being watched, get all entites that are watching him,
    /// and see if he just entered or left within range of them.
    for (Entity *watching : watching_entities(entity)) {
        handle_watched_watching_pair(entity, watching);
    }

    /// if the entity that just moved is watching some other entities, get these entities,
    /// and see if he entered/left range of any of them.
    for (Entity *watched : watched_entities(entity)) {
        handle_watched_watching_pair(watched, entity);
    }
}

void Game::on_watched_entity_dies(QObject *watched_entity) {
    remove_watched_entity(static_cast<Entity *>(watched_entity));
}

void Game::on_watching_entity_dies(QObject *watching_entity) {
    remove_watching_entity(static_cast<Entity *>(watching_entity));
}
