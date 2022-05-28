#pragma once

#include "Vendor.h"

class QTimer;
class QPixmap;

namespace cute {

class SpriteSheet;
class Node;

class Animation : public QObject, public QGraphicsItem {
    Q_OBJECT

public:
    Animation(QGraphicsItem *parent = nullptr);
    Animation(const QPixmap &pixmap, QGraphicsItem *parent = nullptr);
    Animation(const SpriteSheet &sprite_sheet, Node from, Node to, QGraphicsItem *parent = nullptr);

    void add_frame(const QPixmap &pixmap);
    void add_frames(const SpriteSheet &sprite_sheet, const Node &start_cell, const Node &end_cell);
    void add_frames(std::string folder_path, int num_of_images, std::string image_prefix, std::string file_extension);
    void play(int num_times_to_play, double FPS_to_play_at);
    void pause();

    virtual QRectF bounding_rect() const;

    QPixmap current_frame();
    bool is_playing();

public slots:
    void on_animation_step();

signals:
    /// Emitted when the Animation finished playing the last frame.
    /// If the Animation was asked to play an infinte number of times,
    /// this signal will emit each and every time it plays the last frame.
    void finished(Animation *animation);

private:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {}

private:
    std::vector<QPixmap> pixmaps_;

    /// this is constantly changed when play() is called.
    QGraphicsPixmapItem *current_pixmap_;

    QTimer *timer_;
    bool is_playing_;
    int current_frame_;
    int times_played_;
    int times_to_play_;
    double frames_per_second_;
};

} // namespace cute
