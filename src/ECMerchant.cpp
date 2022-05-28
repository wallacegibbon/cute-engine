#include "ECMerchant.h"
#include "ECGUIShower.h"
#include "ShopGUI.h"

using namespace cute;

ECMerchant::ECMerchant(Entity *seller, const QPoint &shop_GUI_pos) : EntityController(seller) {
    shop_GUI_ = new ShopGUI();
    shop_GUI_->set_seller(seller->inventory());
    shop_GUI_->set_GUI_pos(shop_GUI_pos);

    GUI_shower_ = new ECGUIShower(seller, shop_GUI_);
    connect(GUI_shower_, &ECGUIShower::GUI_shown, this, &ECMerchant::on_GUI_shown);
}

void ECMerchant::add_entity_of_interest(Entity *entity) { GUI_shower_->add_entity_of_interest(entity); }

void ECMerchant::set_distance(double distance) { GUI_shower_->set_distance(distance); }

void ECMerchant::on_GUI_shown(ECGUIShower *sender, Entity *activating_entity) {
    shop_GUI_->set_buyer(activating_entity->inventory());
}
