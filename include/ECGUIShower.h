#pragma once

#include "EntityController.h"
#include "Vendor.h"

namespace cute {

class GUI;
class Entity;

/// An EntityController that will pop up a specified gui when entities of interst
/// come within a specified distance of the controlled entity.
///
/// Example
/// =======
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~.cpp
/// ECGUIShower* gs = new ECGUIShower(controlled_entity, GUI_to_show); /// specify the controlled entity and the gui
/// gs->add_entity_of_interest(e1);
/// gs->add_entity_of_interest(e2); /// specify the 'entities of interst'
/// gs->set_distance(100);
///
/// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
/// anytime an 'entity of interest' (e1 or e2 in this case) come within 100 of controlled_entity,
/// the gui 'GUI_to_show' will be shown.

class ECGUIShower : public EntityController {
    Q_OBJECT

public:
    ECGUIShower(Entity *controlled_entity, GUI *gui);

    void add_entity_of_interest(Entity *entity);
    void set_distance(double distance) { distance_ = distance; }
    double distance() const { return distance_; }

public slots:
    void on_entity_moved();

signals:
    /// Emitted when the gui is shown due to an entity of interest coming within range of the controlled entity.
    /// @param entityOfInterst The entity that triggered the gui to show.
    void GUI_shown(ECGUIShower *sender, Entity *entity_of_interest);

    /// Emitted when the gui is hidden due to no entities of interest being within range of the controlled entity.
    void GUI_hidden(ECGUIShower *sender);

private:
    void on_controlled_entity_dying(Entity *controlled_entity) override;

private:
    double distance_ = 150;
    QPointer<GUI> gui_;
    std::unordered_set<QPointer<Entity>> entities_of_interest_;
};

} // namespace cute
