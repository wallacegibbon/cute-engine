#pragma once

#include "Vendor.h"

namespace cute {

/// Enumeration representing the status of a Quest.
/// clang-format off
enum class QuestStatus { IN_PROGRESS, COMPLETED, FAILED };
/// clang-format on

/// Represents a quest, which has a name, description, and a current status such
/// as in progress, completed, or failed.
/// @author Abdullah Aghazadah
/// @date 8-21-16
class Quest : public QObject {
    Q_OBJECT
public:
    Quest(const std::string &name, const std::string &description,
          QuestStatus initial_status = QuestStatus::IN_PROGRESS)
            : name_(name), description_(description), status_(initial_status) {}

    void set_status(QuestStatus new_status);

    QuestStatus status() { return status_; }
    std::string name() { return name_; }
    std::string description() { return description_; }

signals:
    void quest_status_changed(Quest *quest, QuestStatus new_status);

private:
    std::string name_;
    std::string description_;
    QuestStatus status_;
};

} // namespace cute
