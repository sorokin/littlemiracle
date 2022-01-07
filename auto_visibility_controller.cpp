// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#include "auto_visibility_controller.h"
#include "visibility_flags.h"
#include "star_widget.h"
#include <cassert>

namespace
{
    struct state
    {
        visibility_flags visibility;
        uint64_t duration_ms;
    };

    state states[] =
    {
        {{false, false, false, false, true}, 4000},
        {{false, true, false, false, true},  4000},
        {{false, false, false, false, true}, 4000},
        {{false, false, true, false, true},  4000},
        {{false, false, false, false, true}, 4000},
        {{true, false, false, false, true},  4000},
        {{true, false, false, true, true},   5000},
        {{true, false, false, false, true},  3000},
    };
}

auto_visibility_controller::auto_visibility_controller(QObject* parent, star_widget* controllable)
    : QObject(parent)
    , controllable(controllable)
    , current_state(0)
{
    connect(&timer, &QTimer::timeout, this, &auto_visibility_controller::timer_elapsed);
    connect(controllable, &star_widget::paused_state_changed, this, &auto_visibility_controller::paused_state_changed);
    sync_state();
}

void auto_visibility_controller::goto_star()
{
    current_state = std::size(states) - 1;
    sync_state();
}

void auto_visibility_controller::paused_state_changed()
{
    if (controllable->is_running())
    {
        if (timer.isActive())
            return;

        timer.start(time_left_ms);
        etimer.start();
    }
    else
    {
        if (!timer.isActive())
            return;

        assert(etimer.isValid());
        qint64 e = etimer.elapsed();
        assert(e >= 0);
        if (e <= time_left_ms)
            time_left_ms -= e;
        else
            time_left_ms = 0;

        timer.stop();
        etimer.invalidate();
    }
}

void auto_visibility_controller::sync_state()
{
    assert(current_state < std::size(states));
    controllable->set_visibility(states[current_state].visibility);
    time_left_ms = states[current_state].duration_ms;

    if (controllable->is_running())
    {
        timer.start(time_left_ms);
        etimer.start();
    }
}

void auto_visibility_controller::timer_elapsed()
{
    ++current_state;
    if (current_state == std::size(states))
        current_state = 0;
    sync_state();
}
