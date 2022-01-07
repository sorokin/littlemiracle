// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>

struct star_widget;

struct auto_visibility_controller : QWidget
{
private:
    Q_OBJECT

public:
    auto_visibility_controller(QWidget* parent, star_widget* controllable);

    void goto_star();

private:
    void state_changed();

    void sync_state();
    void timer_elapsed();

private:
    star_widget* controllable;
    size_t current_state;
    uint64_t time_left_ms;
    QTimer timer;
    QElapsedTimer etimer;
};
