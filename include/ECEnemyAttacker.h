#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

namespace cute {

class ECChaser;

/// An EntityController that makes the controlled entity attack certain other entities within its field of view.
///
/// Currently the attack policy is:
/// The controlled Entity will use the Weapon in its first WeaponSlot.
/// If the controlled entity does not have any WeaponSlots with a Weapon, it will simply not attack.

class ECAttacker : public EntityController {
    Q_OBJECT

public:
    ECAttacker(Entity *entity);

    void add_attackee(Entity *entity);
    void remove_attackee(Entity *entity);
    std::unordered_set<Entity *> attackies() const;

public slots:
    void on_enemy_chase_continued(Entity *entity_chased, double distance);

signals:
    /// Emitted each time the controlled entity attacks an enemy.
    /// Note that this is only emitted when the controlled entity actually
    /// tries to *attack* an entity (and not simply chase it).
    void attacked(Entity *entityAttacked);

private:
    ECChaser *controller_chase_enemies_;
};

} // namespace cute
