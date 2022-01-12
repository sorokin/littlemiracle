// Copyright Ivan Sorokin 2022. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at https://www.boost.org/LICENSE_1_0.txt)

#pragma once
#include <QWidget>
#include <QElapsedTimer>
#include <QPainterPath>
#include "visibility_flags.h"
#include "enum_map.h"

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
    size_t get_actual_co_num() const;
    double get_actual_ratio() const;

    void set_visibility(visibility_flags visibility);
    void set_visibility(chart_element element, bool visible);
    void set_color(chart_element element, QColor color);
    void set_antialiasing(bool enabled);

    void enable_reset_to_default_action(bool enabled);

    bool is_running() const;

signals:
    void paused_state_changed();
    void reset_to_default_triggered();

private:
    void contextMenuEvent(QContextMenuEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

    void draw_scene(QPainter& p, int width, int height);

    void draw_static_circle(QPainter& p);
    void draw_star(QPainter& p);
    void draw_triangles(QPainter& p, std::vector<QPointF> const& points);
    void draw_squares(QPainter& p, std::vector<QPointF> const& points);
    void draw_rotating_circle(QPainter& p);
    void draw_dots(QPainter& p, std::vector<QPointF> const& points, double extra_scale);

    void draw_polygon(QPainter& p, QPointF const* vertices, size_t n, size_t permutator, size_t step);

    bool need_alpha_animation() const;
    QColor get_color(chart_element e) const;

    void toggle_paused_state();

    void update_actual_num_denom();
    void validate_star_path();
    double rotating_circle_r() const;
    void update_phi();
    void update_alpha();

    void copy_image_to_clipboard();

private:
    QAction* pause_resume_action;
    QAction* reset_to_default_action;
    QAction* copy_image_action;

    size_t desired_num = 3;
    size_t desired_denom = 7;
    size_t actual_num;
    size_t actual_denom;

    size_t triangles_permutator;
    size_t squares_permutator;

    double sharpness = 0.9;
    visibility_flags visibility = {};
    enum_map<QColor, chart_element> colors;
    bool enable_antialiasing = false;

    QElapsedTimer phi_timer;
    QElapsedTimer alpha_timer;

    double phi = 0.;
    enum_map<double, chart_element> current_alpha = {};

    QPainterPath star_path_cache;
};
