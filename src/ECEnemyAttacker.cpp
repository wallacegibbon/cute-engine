#include "ECEnemyAttacker.h"
#include "ECChaser.h"
#include "Slot.h"
#include "Utilities.h"
#include "Weapon.h"
#include "WeaponSlot.h"

using namespace cute;

ECAttacker::ECAttacker(Entity *entity) : EntityController(entity) {
    controller_chase_enemies_ = new ECChaser(entity);
    connect(controller_chase_enemies_, &ECChaser::entity_chase_continued, this, &ECAttacker::on_enemy_chase_continued);
    connect(controller_chase_enemies_, &ECChaser::entity_chase_started, this, &ECAttacker::on_enemy_chase_continued);
}

void ECAttacker::add_attackee(Entity *entity) { controller_chase_enemies_->add_chasee(entity); }

void ECAttacker::remove_attackee(Entity *entity) { controller_chase_enemies_->remove_chasee(entity); }

std::unordered_set<Entity *> ECAttacker::attackies() const { return controller_chase_enemies_->chasees(); }

/// Executed whenever the controlled entity takes one step closer to chased entity.
/// Will see if close enough to use weapon. If so, will use it.
void ECAttacker::on_enemy_chase_continued(Entity *entity_chased, double distance) {
    /// TODO: only attack if close to range of weapon
    /// pick first weapon in controlled enities slots and use it to attack chased entity
    for (Slot *slot : entity_controlled()->get_slots()) {
        WeaponSlot *as_weapon_slot = dynamic_cast<WeaponSlot *>(slot);
        if (as_weapon_slot) {
            if (as_weapon_slot->is_filled()) {
                Weapon *weapon = dynamic_cast<Weapon *>(as_weapon_slot->item());
                if (weapon) {
                    if (distance < weapon->cast_range()) {
                        weapon->attack(entity_chased->pos());
                        emit attacked(entity_chased);
                    }
                }
            }
        }
    }
}
