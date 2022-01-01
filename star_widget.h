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

    void paintEvent(QPaintEvent*) override;

    void draw_circle(QPainter& p, QPointF center, double radius, double alpha);
    void draw_polygon(QPainter& p, std::vector<QPointF> const& vertices);

    QPointF from_polar(double alpha, double radius);
    double outer_beta(double static_r, double rotating_r, double alpha);
    double inner_beta(double static_r, double rotating_r, double alpha);
    QPointF poi(double static_r, double rotating_r, double alpha, double smoothness);

    static void adjust_alpha(double& alpha, bool visible, double dt);

    static constexpr QPointF origin = QPointF();

    QTimer timer;
    QElapsedTimer etimer;
    double alpha = 0.;
    size_t num = 3;
    size_t denom = 7;
    double sharpness = 0.9;

    visibility_flags visibility = {};
    enum_map<double, chart_element_id> current_alpha = {};
};

#endif // STAR_WIDGET_H
