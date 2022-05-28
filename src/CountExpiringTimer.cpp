#include "CountExpiringTimer.h"

using namespace cute;

CountExpiringTimer::CountExpiringTimer(QObject *parent) : QObject(parent) {
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &CountExpiringTimer::fired_);
}

void CountExpiringTimer::start(int freqInMs, int num_times_to_fire) {
    assert(!started());

    started_ = true;
    num_times_fired_ = 0;
    num_times_to_fire_ = num_times_to_fire;
    timer_->start(freqInMs);
}

void CountExpiringTimer::fired_() {
    emit timeout();

    num_times_fired_++;
    if (num_times_fired_ > num_times_to_fire_) {
        deleteLater();
    }
}
