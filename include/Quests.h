#pragma once

#include "Quest.h"
#include "Vendor.h"

namespace cute {

/// Represents a collection of Quests. Will emit events whenever a quest
/// is added, removed, or if the status of any quest is changed.
class Quests : public QObject {
    Q_OBJECT

public:
    Quests();

    void add_quest(Quest *quest);
    void remove_quest(Quest *quest);
    int num_of_quests();

    Quest *quest(int i);

signals:
    /// Emitted whenever a Quest is added to the Quests.
    void quest_added(Quest *newQuest);

    /// Emitted whenever a Quest is removed from the Quests.
    /// This can happen via remove_quest() or if one the quest gets deconstructed.
    void quest_removed(Quest *removedQuest);

    /// Emitted whenever the status of one of the quests in the collection changes.
    void quest_status_changed(Quest *forQuest, QuestStatus new_status);

public slots:
    void on_quest_status_changed(Quest *quest, QuestStatus new_status);
    void on_quest_destructed(QObject *quest);

private:
    std::vector<Quest *> quests_;
};

} // namespace cute
