#pragma once

#include "Entity.h"
#include "EntityController.h"
#include "Vendor.h"

class QPointF;

namespace cute {

/// A EntityController that can somehow move an Entity from its current position to a
/// specified position. *How* the object moves depends on the concrete Mover
/// class.
///
/// Example usage
/// =============
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// Mover* mover = new SomeConcreteMover(entityToMove);
/// mover->move_entity(toSomePosition);
/// bool isMoving = mover->is_moving_entity();
/// mover->stop_moving_entity();
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
///
/// Lets go over the above line by line.
///
/// `Mover* mover = new Mover(entityToMove);`
/// Creates a Mover that can move the Entity passed in it's ctor. Alternatively
/// you can use set_entity() to set the Entity rather than passing it into the
/// ctor.
///
/// `mover->move_entity(toSomePosition);`
/// Tells the Mover to move the Entity to some specified position. After the
/// above call, the Entity should start moving to the specified position in
/// whatever manner the concrete Mover class decides.
///
/// `bool isMoving = mover->is_moving_entity();`
/// Checks to see if the Mover is currently moving the Entity.
///
/// `mover->stop_moving_entity();`
/// Tells the Mover to stop moving the Entity.
///
/// @warning
/// You cannot call set_entity() while the Mover is already busy moving an
/// Entity. If you do so, an assertion will be thrown. You can use
/// is_moving_entity() to determine if the Mover is currently moving its Entity.
///
/// How to implement your own concrete Mover class
/// ==============================================
/// 1. Implement the pure virtual function move_entity_(). This function should
/// start moving the Entity in some way.
///
/// 2. When you have determined that the entity has succesfully been moved,
/// emit the entity_succesfully_moved() signal.
///
/// 3. Implement the pure virtual function stop_moving_entity_(). This function
/// should stop moving the Entity.
///
/// See some of the provided concrete Movers for examples of how to create your
/// own Movers.
///
/// @author Abdullah Aghazadah

class ECMover : public EntityController {
    Q_OBJECT

public:
    ECMover(Entity *entity = nullptr) : EntityController(entity) { set_entity(entity); }

    virtual Entity *entity() { return entity_controlled(); }
    virtual void set_entity(Entity *entity);

    virtual void move_entity(const QPointF &to_pos);
    virtual bool is_moving_entity();
    virtual void stop_moving_entity();

public slots:
    void on_entity_died(Entity *entity) { stop_moving_entity(); }

signals:
    void entity_succesfully_moved(ECMover *byMover);

protected:
    virtual void move_entity_(const QPointF &to_pos) = 0;
    virtual void stop_moving_entity_() = 0;

private:
    QPointer<Entity> entity_;
    bool is_moving_entity_ = false;
};

} // namespace cute
