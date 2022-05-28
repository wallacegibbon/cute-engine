#include "QuestViewer.h"
#include "Button.h"
#include "Label.h"
#include "Panel.h"
#include "Quest.h"
#include "Quests.h"
#include "ScrollWindow.h"

using namespace cute;

QuestViewer::QuestViewer(Quests *quests)
        : quests_(quests), quest_labels_(), label_to_quest_(), outter_panel_(new Panel()) {
    scroll_window_ = new ScrollWindow();
    scroll_window_->set_parent_GUI(outter_panel_.get());
    selected_quest_description_->set_parent_GUI(outter_panel_.get());

    /// defaults
    outter_panel_->set_background_pixmap(QPixmap(":/cute-engine-builtin/resources/graphics/misc/paper.png"));
    scroll_window_->show_background(false);
    scroll_window_->show_border(false);

    selected_quest_description_ = new Label();
    selected_quest_description_->set_text("Select a quest to see its description.");

    draw();
}

void QuestViewer::set_quests(Quests *quests) {
    /// stop listening to old quests
    if (quests_) {
        quests_->disconnect();
    }
    connect(quests, &Quests::quest_added, this, &QuestViewer::draw);
    connect(quests, &Quests::quest_removed, this, &QuestViewer::draw);
    connect(quests, &Quests::quest_status_changed, this, &QuestViewer::draw);
    quests_ = quests;
    draw();
}

QGraphicsItem *QuestViewer::get_graphics_item() { return outter_panel_->get_graphics_item(); }

void QuestViewer::draw() {
    /// remove old quest labels from the scroll window
    /// (in case there are any new quests - we want to create labels for all quests from scratch)
    for (Label *label : quest_labels_) {
        scroll_window_->remove(label);
    }

    /// size outter panel
    outter_panel_->set_width(width_);
    outter_panel_->set_height(height_);

    /// position/size scroll window
    scroll_window_->set_GUI_pos(QPointF(scroll_window_x_, scroll_window_y_));
    scroll_window_->set_width(scroll_window_width_);
    scroll_window_->set_height(scroll_window_height_);

    /// position/size description label properly
    selected_quest_description_->set_GUI_pos(
            QPointF(desc_label_x_, scroll_window_y_ + scroll_window_height_ + desc_offset_y_));
    selected_quest_description_->set_width(desc_width_);

    /// create a quest label for each quest
    if (quests_ != nullptr) {
        for (int i = 0, n = quests_->num_of_quests(); i < n; i++) {
            Label *label = new Label();
            label->set_text(quests_->quest(i)->name());
            label_to_quest_[label] = quests_->quest(i);
            connect(label, &Label::clicked, this, &QuestViewer::on_label_clicked);
            quest_labels_.push_back(label);
        }
    }

    /// place quest labels in scroll window
    for (int i = 0, n = quest_labels_.size(); i < n; i++) {
        Label *label = quest_labels_[i];
        scroll_window_->add(label, QPointF(25, i * 40 + 25));
    }
}

/// Will find corrisponding quest and show its text in the description area.
/// Will also marked the quest as the currently selected one.
void QuestViewer::on_label_clicked(Label *label) {
    Quest *quest = label_to_quest_[label];
    selected_quest_ = quest;
    selected_quest_description_->set_text(quest->description());
}
