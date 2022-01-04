#ifndef STAR_WIDGET_H
#define STAR_WIDGET_H

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

    size_t get_num() const;
    size_t get_denom() const;
    size_t get_actual_num() const;
    size_t get_actual_denom() const;

    void set_visibility(visibility_flags visibility);

    void mousePressEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

    void draw_circle(QPainter& p, QPointF center, double radius, double alpha);
    void draw_polygon(QPainter& p, QPointF const* vertices, size_t n, size_t step);

    QPointF from_polar(double alpha, double radius);
    double outer_angle(double static_r, double rotating_r, double phi);
    double inner_angle(double static_r, double rotating_r, double phi);
    QPointF poi(double static_r, double rotating_r, double alpha, double smoothness);

    static void adjust_alpha(double& alpha, bool visible, double dt);
    bool need_alpha_animation() const;
    QColor get_color(chart_element_id e) const;

    static constexpr QPointF origin = QPointF();

private:
    void update_actual_num_denom();
    void validate_star_path();
    double small_r() const;
    void update_phi();
    void update_alpha();

private:
    size_t desired_num = 3;
    size_t desired_denom = 7;
    size_t actual_num = 3;
    size_t actual_denom = 7;

    QElapsedTimer phi_timer;
    QElapsedTimer alpha_timer;

    double phi = 0.;
    double sharpness = 0.9;

    QPainterPath star_path;

    visibility_flags visibility = {};
    enum_map<double, chart_element_id> current_alpha = {};

public:
    enum_map<QColor, chart_element_id> colors;

    bool enable_antialiasing = false;

    static constexpr double big_r = 0.48;
};

#endif // STAR_WIDGET_H
