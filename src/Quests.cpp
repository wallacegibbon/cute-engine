#include "Quests.h"

using namespace cute;

Quests::Quests() {}

void Quests::add_quest(Quest *quest) {
    assert(quest != nullptr);

    /// add the quest
    quests_.push_back(quest);

    /// listen to when the status of the added quest changes
    connect(quest, &Quest::quest_status_changed, this, &Quests::on_quest_status_changed);

    /// listen to when the quest is destructed
    connect(quest, &Quest::destroyed, this, &Quests::on_quest_destructed);

    emit quest_added(quest);
}

void Quests::remove_quest(Quest *quest) {
    assert(quest != nullptr);

    std::vector<Quest *>::iterator position = std::find(quests_.begin(), quests_.end(), quest);
    if (position != quests_.end()) {
        quests_.erase(position);

        /// stop listening to quest events
        disconnect(quest, &Quest::quest_status_changed, this, &Quests::on_quest_status_changed);
        disconnect(quest, &Quest::destroyed, this, &Quests::on_quest_destructed);

        emit quest_removed(quest);
    }
}

int Quests::num_of_quests() { return quests_.size(); }

Quest *Quests::quest(int i) { return quests_[i]; }

void Quests::on_quest_status_changed(Quest *quest, QuestStatus new_status) {
    emit quest_status_changed(quest, new_status);
}

void Quests::on_quest_destructed(QObject *quest) {
    Quest *as_quest = dynamic_cast<Quest *>(quest);
    assert(as_quest);

    remove_quest(as_quest);
}
