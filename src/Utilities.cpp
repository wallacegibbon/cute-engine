#include "Utilities.h"
#include "AngledSprite.h"
#include "EntitySprite.h"
#include "Map.h"
#include "PathingMap.h"
#include "RandomGenerator.h"
#include "RandomImageEntity.h"
#include "SpriteSheet.h"

using namespace cute;

/// Adds the specified number of trees randomly scattered on the specified Map.
void cute::add_random_trees(Map *map, int nums_to_add, int num_images) {
    for (int i = 0; i < nums_to_add; i++) {
        add_random_tree(map, num_images);
    }
}

int trim_to_gap(int value, int gap) { return value - (value % gap); }

void cute::add_random_tree(Map *map, int num_images) {
    std::string full_path = ":/cute-engine-builtin/resources/graphics/tree/tree_pathing.png";
    PathingMap *pm = new PathingMap(QPixmap(full_path.c_str()), 32);
    RandomImageEntity *tree =
            new RandomImageEntity(":/cute-engine-builtin/resources/graphics/tree", "tree", num_images, *pm);
    tree->add_tag("scenery");
    map->add_entity(tree);
    double x = trim_to_gap(rand_int(100, map->width() - 100), map->cell_size());
    double y = trim_to_gap(rand_int(100, map->height() - 100), map->cell_size());
    tree->set_pos(QPointF(x, y));
}

quint32 cute::fast_random() {
    static quint32 num = 7;
    num ^= num << 13;
    num ^= num >> 17;
    num ^= num << 5;
    return num;
}

/// both angle1 and angle2 have to be between 0-360
inline int min_gap_between_2_angle(int angle1, int angle2) {
    int gap = abs(angle1 - angle2);
    return std::min(gap, 360 - gap);
}

double cute::closest_angle(const std::vector<int> &all_angles, double target_angle) {
    assert(!all_angles.empty());
    target_angle = fmod(target_angle, 360);
    if (target_angle < 0) {
        target_angle = 360 - abs(target_angle);
    }
    double closest = all_angles[0];
    double closest_distance = min_gap_between_2_angle(target_angle, closest);
    for (int angle : all_angles) {
        double distance = min_gap_between_2_angle(target_angle, angle);
        if (distance < closest_distance) {
            closest = angle;
            closest_distance = distance;
        }
    }
    return closest;
}

int cute::rand_int(int from, int to) {
    static RandomGenerator generator;
    return generator.rand_int(from, to);
}

double cute::rand_double(double from, double to) {
    static RandomGenerator generator;
    return generator.rand_double(from, to);
}

void cute::add_tag(const std::string &tag, std::initializer_list<Entity *> entities) {
    for (Entity *entity : entities) {
        entity->add_tag(tag);
    }
}

/// 8 directions (180, 225, 270, 315, 0, 45, 90, 135) in order.
void for_each_angle(std::function<void(int angle, int row)> handler) {
    for (int row = 0; row < 8; row++) {
        handler((row * 45 + 180) % 360, row);
    }
}

Entity *cute::get_minotaur_entity() {
    Entity *entity = new Entity();
    std::string url = ":/cute-engine-builtin/resources/graphics/character_sprite_sheets/minotaur_alpha.png";
    SpriteSheet sprite_sheet(url, 24, 8, 128, 128);
    AngledSprite *sprite = new AngledSprite();

    for_each_angle([&](int angle, int row) {
        sprite->add_frames(angle, "stand", sprite_sheet, Node(0, row), Node(3, row));
        sprite->add_frame(sprite_sheet.tile_at(Node(2, row)), "stand", angle);
        sprite->add_frame(sprite_sheet.tile_at(Node(1, row)), "stand", angle);
        sprite->add_frames(angle, "walk", sprite_sheet, Node(4, row), Node(11, row));
        sprite->add_frames(angle, "attack", sprite_sheet, Node(12, row), Node(18, row));
    });

    entity->set_sprite(sprite);
    entity->set_bounding_box_and_update_origin(QRectF(QPointF(32, 32), QPointF(96, 96)));
    entity->pathing_map().fill(QRectF(QPointF(32, 32), QPointF(96, 96)));
    // entity->pathing_map().fill(QRectF(70, 70, 30, 30));
    return entity;
}

Entity *cute::get_spider_entity() {
    Entity *entity = new Entity();
    std::string url = ":/cute-engine-builtin/resources/graphics/character_sprite_sheets/spider.png";
    SpriteSheet sprite_sheet(url, 48, 8, 128, 128);
    AngledSprite *sprite = new AngledSprite();

    for_each_angle([&](int angle, int row) {
        sprite->add_frames(angle, "stand", sprite_sheet, Node(0, row), Node(3, row));
        sprite->add_frame(sprite_sheet.tile_at(Node(2, row)), "stand", angle);
        sprite->add_frame(sprite_sheet.tile_at(Node(1, row)), "stand", angle);
        sprite->add_frames(angle, "walk", sprite_sheet, Node(4, row), Node(11, row));
        sprite->add_frames(angle, "attack", sprite_sheet, Node(12, row), Node(15, row));
        sprite->add_frames(angle, "hitten", sprite_sheet, Node(18, row), Node(20, row));
        // sprite->add_frames(angle, "die", sprite_sheet, Node(18, row), Node(23, row));
        sprite->add_frames(angle, "die", sprite_sheet, Node(24, row), Node(31, row));
    });

    entity->set_sprite(sprite);
    entity->set_bounding_box_and_update_origin(QRectF(QPointF(32, 32), QPointF(96, 96)));
    entity->sprite()->play("stand", -1, 15, 0);

    return entity;
}
