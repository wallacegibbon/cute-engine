#pragma once

#include "GUI.h"
#include "Panel.h"
#include "Vendor.h"

namespace cute {

class Quest;
class Quests;
class Button;
class Label;
class ScrollWindow;

/// GUI that can visualize a bunch of quests (Quests class).
/// Click on a quest to view its description.
/// @author Abdullah Aghazadah
/// @date 9/5/16
class QuestViewer : public GUI {
    Q_OBJECT

public:
    QuestViewer(Quests *quests = nullptr);

    void set_quests(Quests *quests);
    Quest *selected_quest() { return selected_quest_; }

    QGraphicsItem *get_graphics_item() override;

public slots:
    void draw();
    void on_label_clicked(Label *label);

private:
    Quests *quests_;
    Quest *selected_quest_ = nullptr;
    std::vector<Label *> quest_labels_;
    std::unordered_map<Label *, Quest *> label_to_quest_;

    double width_ = 360;
    double height_ = 360;

    double scroll_window_width_ = 340;
    double scroll_window_height_ = 170;

    double scroll_window_x_ = 20;
    double scroll_window_y_ = 20;

    double desc_width_ = 320;
    double desc_label_x_ = 30;

    /// distance between the bottom of the scrollWindow and the start of description label
    double desc_offset_y_ = 20;

    double close_btn_x_;
    double close_btn_y_;

    std::unique_ptr<Panel> outter_panel_;
    ScrollWindow *scroll_window_;
    Label *selected_quest_description_;
};

} // namespace cute
