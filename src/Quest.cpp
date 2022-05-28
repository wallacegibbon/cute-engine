#include "Quest.h"

using namespace cute;

void Quest::set_status(QuestStatus new_status) {
    bool status_changed = new_status != status_;
    status_ = new_status;
    if (status_changed) {
        emit quest_status_changed(this, new_status);
    }
}
