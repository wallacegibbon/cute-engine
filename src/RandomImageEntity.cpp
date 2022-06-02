#include "RandomImageEntity.h"
#include "RandomGenerator.h"
#include "Sprite.h"
#include "TopDownSprite.h"
#include "Utilities.h"
#include "stdlib.h"
#include <QDebug>

using namespace cute;

/// Constructs an entity whos display image will be randomly chosen from a set of images in a folder.
/// @param num_of_images how many images of the specified prefix are in the folder (must start with number 0)
/// @param pathing_map the pathing map to use for the entity.
RandomImageEntity::RandomImageEntity(std::string resource_foler_path, std::string image_prefix, int num_of_images,
                                     PathingMap &pathing_map) {
    std::vector<QPixmap> pixmaps;
    for (int i = 0, n = num_of_images; i < n; i++) {
        std::string full_path = resource_foler_path + "/" + image_prefix + "_" + std::to_string(i) + ".png";
        pixmaps.push_back(QPixmap(full_path.c_str()));
    }

    int index = common_random_generator.rand_int(0, pixmaps.size() - 1);
    TopDownSprite *spr = new TopDownSprite(pixmaps[index]);
    set_sprite(spr);
    set_pathing_map(pathing_map);
    set_invulnerable(true);
}
