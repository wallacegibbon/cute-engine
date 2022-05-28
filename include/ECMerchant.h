#pragma once

#include "EntityController.h"

namespace cute {

class ECGUIShower;
class ShopGUI;

/// An EntityController that adds merchant-like behavior to an entity.
///
/// When an 'entity of interest' gets near the controlled entity, a ShopGUI will pop up.
/// Use add_entity_of_interest() to add an entity that will trigger the ShopGUI popping up.
/// The ShopGUI will transfer items between the controlled entity and the trigger entity (an entity of interest).

class ECMerchant : public EntityController {
    Q_OBJECT

public:
    ECMerchant(Entity *seller, const QPoint &shop_GUI_pos = QPoint(0, 0));

    void add_entity_of_interest(Entity *entity);
    void set_distance(double distance);

public slots:
    void on_GUI_shown(ECGUIShower *sender, Entity *activating_entity);

private:
    ECGUIShower *GUI_shower_;
    ShopGUI *shop_GUI_;
};

} // namespace cute
