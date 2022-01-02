#ifndef STAR_WIDGET_H
#define STAR_WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include "visibility_flags.h"
#include "enum_map.h"

struct star_widget : QWidget
{
private:
    Q_OBJECT

public:
    star_widget(QWidget* parent);

    void timer_tick();

    void mousePressEvent(QMouseEvent*) override;
    void paintEvent(QPaintEvent*) override;

    void draw_circle(QPainter& p, QPointF center, double radius, double alpha);
    void draw_polygon(QPainter& p, QPointF const* vertices, size_t n, size_t step);

    QPointF from_polar(double alpha, double radius);
    double outer_angle(double static_r, double rotating_r, double phi);
    double inner_angle(double static_r, double rotating_r, double phi);
    QPointF poi(double static_r, double rotating_r, double alpha, double smoothness);

    static void adjust_alpha(double& alpha, bool visible, double dt);
    QColor get_color(chart_element_id e) const;

    static constexpr QPointF origin = QPointF();

    QTimer timer;
    QElapsedTimer etimer;
    double phi = 0.;
    size_t num = 3;
    size_t denom = 7;
    double sharpness = 0.9;

    visibility_flags visibility = {};
    enum_map<double, chart_element_id> current_alpha = {};
    enum_map<QColor, chart_element_id> colors;
};

#endif // STAR_WIDGET_H
