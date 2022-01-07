// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <QWidget>
#include <QElapsedTimer>
#include <QPainterPath>
#include "visibility_flags.h"
#include "enum_map.h"
#include <optional>

struct star_widget : QWidget
{
private:
    Q_OBJECT

public:
    star_widget(QWidget* parent);

    void set_desired_num(size_t num);
    void set_desired_denom(size_t denom);
    void set_desired_num_denom(size_t num, size_t denom);
    void set_sharpness(double sharpness);

    size_t get_desired_num() const;
    size_t get_desired_denom() const;
    size_t get_actual_num() const;
    size_t get_actual_denom() const;

    void set_visibility(visibility_flags visibility);
    void set_visibility(chart_element_id element, bool visible);
    void set_color(chart_element_id element, QColor color);
    void set_antialiasing(bool enabled);

    bool is_running() const;

signals:
    void state_changed();

private:
    void keyPressEvent(QKeyEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

    void draw_circle(QPainter& p, QPointF center, double radius, double alpha);
    void draw_polygon(QPainter& p, QPointF const* vertices, size_t n, size_t step);

    static void adjust_alpha(double& alpha, bool visible, double dt);
    bool need_alpha_animation() const;
    QColor get_color(chart_element_id e) const;

    void toggle_state();

    void update_actual_num_denom();
    void validate_star_path();
    double rotating_circle_r() const;
    void update_phi();
    void update_alpha();

private:
    size_t desired_num = 3;
    size_t desired_denom = 7;
    size_t actual_num = 3;
    size_t actual_denom = 7;

    double sharpness = 0.9;
    visibility_flags visibility = {};
    enum_map<QColor, chart_element_id> colors;
    bool enable_antialiasing = false;

    QElapsedTimer phi_timer;
    QElapsedTimer alpha_timer;

    double phi = 0.;
    enum_map<double, chart_element_id> current_alpha = {};

    QPainterPath star_path_cache;
};
