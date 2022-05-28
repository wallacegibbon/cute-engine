#pragma once

#include "Entity.h"
#include "Vendor.h"

namespace cute {

class Entity;

/// An object that provides some sort of behavior for an Entity (i.e "controls" the Entity in some way).
///
/// As you can see some EntitiyControllers simply "watch" their controlled
/// entity and emit signals when they see something interesting (e.g.
/// ECFieldOFViewEmitter), other EntityControllers allow you to do something to
/// the controlled entity (e.g. ECRotater), and some EntityControllers make the
/// controlled entity do something in response to some external event (e.g.
/// ECKeyboardMover4Directional).
///
/// In summary, EntityControllers allow you to give a certain Entity some functionality.
///
/// Lifetime
/// ========
/// When an Entity is deconstructed, it will deconstruct all of the EntityControllers
/// that are operating on it. This stays consistent with our general policy of
///"container" objects owning the lifetime of their "contained" objects.
///
/// Implementation Note: We leverage the QObject parent/child system in order to enforce this policy.
///
class EntityController : public QObject {
    Q_OBJECT

public:
    EntityController(Entity *entity_to_control);
    virtual ~EntityController() {}

    Entity *entity_controlled() { return entity_; }
    void set_entity_controlled(Entity *entityControlled);

    virtual void on_controlled_entity_dying(Entity *controlled_entity) {}

private slots:
    void on_controlled_entity_dying_(Entity *sender) { on_controlled_entity_dying(sender); }

private:
    /// Returns nullptr if the EntityController isn't controlling an Entity
    /// (this can happen if the entity controlled dies).
    /// use QPointer to prevent dangling pointer issues
    QPointer<Entity> entity_;
};

} // namespace cute
