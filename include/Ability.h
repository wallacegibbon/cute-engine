#pragma once

#include "Entity.h"
#include "Sprite.h"
#include "Vendor.h"

namespace cute {

/// An abstract class that represents an ability.
/// An Ability has an owner and an icon.
/// The owner may be effected in some way when the ability is used, depending on the ability.
/// The Icon is used for GUI purposes (i.e.by AbilityCell).
///
/// All Abilities will throw assertion errors if they are used after the owner is dead
/// (this is automatically handled by NoTargetAbility, PointTargetAbility, and EntityTargetAbility).
/// Most abilities require the owner to be in a Map before being used, but not all,
/// so this is something that each Ability subclass should handle on its own.
/// The main difference between Abilities and Items is the fact that Items are Entities,
/// therefore they can be on the ground where as Abilities cannot.
///
/// How to create your own abilities
/// --------------------------------
/// Subclass NoTargetAbility, PointTargetAbility, or EntityTargetAbility depending on what type of
/// target your ability needs.

class Ability : public QObject {
    Q_OBJECT

public:
    Entity *owner() { return owner_; }
    void set_owner(Entity *owner) { owner_ = owner; }
    QPixmap icon() { return icon_; }
    void set_icon(const QPixmap &icon) { icon_ = icon; }
    std::string description() { return description_; }
    void set_description(const std::string &description) { description_ = description; }

protected:
    Ability(Entity *owner = nullptr, QPixmap icon = QPixmap())
            : owner_(owner), icon_(icon), description_("No description.") {}

    void assert_owner_is_alive() { assert(owner_ != nullptr); }

private:
    QPointer<Entity> owner_;
    QPixmap icon_;
    std::string description_;
};

} // namespace cute
