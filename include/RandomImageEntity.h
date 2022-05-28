#pragma once

#include "Entity.h"
#include "PathingMap.h"
#include "Vendor.h"

namespace cute {

/// An Entity whos displayed image is randomly picked from a set of images in a folder.
/// No matter which image is picked, the same pathing map that is specified will be used for the entity.
/// This class is useful for generating eye-candy that needs a little variation in imagery,
/// such as trees, bushes, rocks, etc...
/// @author Abdullah Aghazadah
/// @date 8-16-16
class RandomImageEntity : public Entity {
public:
    RandomImageEntity(std::string resource_foler_path, std::string image_prefix, int num_of_images,
                      PathingMap &pathing_map);
};

} // namespace cute
