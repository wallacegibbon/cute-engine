#include "QuestAcceptor.h"
#include "Button.h"
#include "Game.h"
#include "QuestViewer.h"

using namespace cute;

QuestAcceptor::QuestAcceptor(Game *game) : quest_viewer_(new QuestViewer()), game_(game) {
    close_button_ = new Button();
    close_button_->set_parent_GUI(quest_viewer_.get());
    accept_button_ = new Button();
    accept_button_->set_parent_GUI(quest_viewer_.get());

    /// set initial look
    close_button_->set_GUI_pos(QPointF(0, 360));
    accept_button_->set_GUI_pos(QPointF(145, 360));
    close_button_->set_text("Close");
    accept_button_->set_text("Accept Selected Quest");

    /// listen to button
    connect(close_button_, &Button::clicked, this, &QuestAcceptor::on_close_button_clicked);
    connect(accept_button_, &Button::clicked, this, &QuestAcceptor::on_accept_button_clicked);
}

/// Sets the Quests that the QuestAcceptor should show.
void QuestAcceptor::set_quests(Quests *quests) { quest_viewer_->set_quests(quests); }

QGraphicsItem *QuestAcceptor::get_graphics_item() { return quest_viewer_->get_graphics_item(); }

void QuestAcceptor::on_accept_button_clicked() {
    Quest *selected_quest = quest_viewer_->selected_quest();
    if (selected_quest != nullptr) {
        emit quest_accepted(selected_quest);
    }
}

void QuestAcceptor::on_close_button_clicked() { game_->remove_GUI(this); }
