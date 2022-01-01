#pragma once
#include <QWidget>
#include <QTimer>

struct star_widget;

struct auto_visibility_controller : QWidget
{
private:
    Q_OBJECT

public:
    auto_visibility_controller(QWidget* parent, star_widget* controllable);

    void goto_star();

    void sync_state();
    void timer_elapsed();

private:
    star_widget* controllable;
    size_t current_state;
    QTimer timer;
};
