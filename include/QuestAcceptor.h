#pragma once

#include "GUI.h"
#include "QuestViewer.h"
#include "Vendor.h"

namespace cute {

class Button;
class Quest;
class Quests;

/// Represents a GUI that allows you to select/accept quests.
/// Similar to a QuestViewer, the user is allowed to browse through some quests,
/// however the QuestAccepter allows you to "accept" a quest by clicking its
/// "Accept Selected Quest" button. Each time a Quest is accepted, a signal is emitted
/// telling you which quest was just accepted.
/// QuestAcceptor also has a close button, which simply closes the GUI (i.e. removes
/// it from the game).
///
/// The recommended way to use the QuestAcceptor is:
/// - create some Quest objects, put them in a Quests object
/// - call QuestAcceptor::set_quests() passing in the quests object
/// - listen to the quest accept signals to be notified of which quests the user accepts
/// @author Abdullah Aghazadah
/// @date 11/9/16
class QuestAcceptor : public GUI {
    Q_OBJECT

public:
    QuestAcceptor(Game *game);

    void set_quests(Quests *quests);

    /// TODO: create delegetory veneer functions

    QGraphicsItem *get_graphics_item() override;

public slots:
    void on_accept_button_clicked();
    void on_close_button_clicked();

signals:
    void quest_accepted(Quest *quest);

private:
    std::unique_ptr<QuestViewer> quest_viewer_;
    Button *close_button_;
    Button *accept_button_;

    /// needed for close button to remove the GUI
    Game *game_;
};

} // namespace cute
