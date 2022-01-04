#include "auto_visibility_controller.h"
#include "visibility_flags.h"
#include "star_widget.h"

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

auto_visibility_controller::auto_visibility_controller(QWidget* parent, star_widget* controllable)
    : QWidget(parent)
    , controllable(controllable)
    , current_state(0)
{
    connect(&timer, &QTimer::timeout, this, &auto_visibility_controller::timer_elapsed);
    sync_state();
}

void auto_visibility_controller::goto_star()
{
    current_state = std::size(states) - 1;
    sync_state();
}

void auto_visibility_controller::sync_state()
{
    assert(current_state < std::size(states));
    controllable->set_visibility(states[current_state].visibility);
    timer.start(states[current_state].duration_ms);
}

void auto_visibility_controller::timer_elapsed()
{
    ++current_state;
    if (current_state == std::size(states))
        current_state = 0;
    sync_state();
}
