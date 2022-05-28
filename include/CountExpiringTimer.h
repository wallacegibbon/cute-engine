#pragma once

#include "Vendor.h"

namespace cute {

/// Represents a timer that will fire a specific number of times and then
/// deletes itself.
/// @author Abdullah Aghazadah
/// @date 6-8-15
class CountExpiringTimer : public QObject {
    Q_OBJECT

public:
    CountExpiringTimer(QObject *parent = nullptr);

    void start(int freqInMs, int num_times_to_fire);
    bool started() const { return started_; }
    void disconnect() { timer_->disconnect(); }

public slots:
    void fired_();

signals:
    void timeout();

private:
    int num_times_to_fire_ = 0;
    int num_times_fired_ = 0;
    bool started_ = false;
    QTimer *timer_;
};

} // namespace cute
