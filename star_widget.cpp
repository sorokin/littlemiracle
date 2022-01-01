#include "star_widget.h"
#include <QPainter>
#include <QPainterPath>
#include <cmath>

star_widget::star_widget(QWidget* parent)
    : QWidget(parent)
{
    connect(&timer, &QTimer::timeout, this, &star_widget::timer_tick);
    timer.start(1);
    etimer.start();
}

void star_widget::timer_tick()
{
    assert(alpha >= 0.);
    qint64 dt = etimer.restart();
    alpha += 0.001 * dt;
    if (alpha >= num * 2 * M_PI)
        alpha -= num * 2 * M_PI;

    for (auto e : all_enumerators<chart_element_id>())
        adjust_alpha(current_alpha[e], visibility[e], dt);

    update();
}

void star_widget::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.translate(QPointF(width(), height()) / 2.);
    double scale = std::min(width(), height());
    if (sharpness > 1.)
        scale *= 2 / (1 + sharpness);
    p.scale(scale, -scale);
    {
        QPen pen = p.pen();
        double w = 0.007;
        if (sharpness > 1.)
            w *= 2 / (1 + sharpness);
        pen.setWidthF(w);
        p.setPen(pen);
    }

    double big_r = 0.48;
    double small_r = big_r * (double)num / (double)denom;

    p.drawEllipse(origin, big_r, big_r);

    if (num > denom)
        return;

    if (current_alpha[chart_element_id::stars] != 0.)
    {
        {
            QPen pen = p.pen();
            pen.setColor(QColor::fromRgbF(0.25, 0.6, 0.225, current_alpha[chart_element_id::stars]));
            p.setPen(pen);
        }

        QPainterPath path;
        for (double alpha = 0; alpha < num * 2 * M_PI; alpha += 0.01)
        {
            QPointF pnt = poi(big_r, small_r, alpha, sharpness);
            if (alpha == 0)
                path.moveTo(pnt);
            else
                path.lineTo(pnt);
        }
        p.drawPath(path);
    }

    size_t co_num = denom - num;

    std::vector<QPointF> points;
    for (size_t i = 0; i != num * co_num; ++i)
        points.push_back(poi(big_r, small_r, alpha + ((double)i / (double)co_num) * 2 * M_PI, sharpness));

    if (current_alpha[chart_element_id::triangles] != 0.)
    {
        {
            QPen pen = p.pen();
            pen.setColor(QColor::fromRgbF(0.9, 0.6, 0.1, current_alpha[chart_element_id::triangles]));
            p.setPen(pen);
        }

        for (size_t i = 0; i != co_num; ++i)
        {
            std::vector<QPointF> poly;
            for (size_t j = 0; j != num; ++j)
                poly.push_back(points[i + co_num * j]);
            draw_polygon(p, poly);
        }
    }

    if (current_alpha[chart_element_id::squares] != 0.)
    {
        {
            QPen pen = p.pen();
            pen.setColor(QColor::fromRgbF(0.9, 0.3, 0.45, current_alpha[chart_element_id::squares]));
            p.setPen(pen);
        }

        for (size_t i = 0; i != num; ++i)
        {
            std::vector<QPointF> poly;
            for (size_t j = 0; j != co_num; ++j)
                poly.push_back(points[i + num * j]);
            draw_polygon(p, poly);
        }
    }

    if (current_alpha[chart_element_id::circles] != 0.)
    {
        QPen pen = p.pen();
        pen.setColor(QColor::fromRgbF(64. / 255., 163. / 255., 199. / 255., current_alpha[chart_element_id::circles]));
        p.setPen(pen);
        
        draw_circle(p, origin + from_polar(alpha, big_r - small_r), small_r, inner_beta(big_r, small_r, alpha));
    }

    if (current_alpha[chart_element_id::dots] != 0.)
    {
        {
            QPen pen = p.pen();
            //pen.setColor(QColor::fromRgbF(0.1, 0.2, 0.9, current_alpha[chart_element_id::dots]));
            //, 64, 199
            pen.setColor(QColor::fromRgbF(71. / 255., 64. / 255., 199. / 255., current_alpha[chart_element_id::dots]));
            p.setPen(pen);
            
            QBrush brush = p.brush();
            brush.setColor(QColor::fromRgbF(0, 0, 0, current_alpha[chart_element_id::dots]));
            brush.setStyle(Qt::SolidPattern);
            p.setBrush(brush);
        }

        for (size_t i = 0; i != points.size(); ++i)
            p.drawEllipse(points[i], 0.005, 0.005);
    }
}

void star_widget::draw_circle(QPainter& p, QPointF center, double radius, double alpha)
{
    p.drawEllipse(center, radius, radius);
    QPointF tip = center + sharpness * from_polar(alpha, radius);
    p.drawLine(center, tip);
}

void star_widget::draw_polygon(QPainter& p, std::vector<QPointF> const& vertices)
{
    QPainterPath path;
    path.moveTo(vertices[0]);
    for (size_t i = 1; i != vertices.size(); ++i)
        path.lineTo(vertices[i]);
    path.closeSubpath();
    p.drawPath(path);
}

QPointF star_widget::from_polar(double alpha, double radius)
{
    return QPointF(cos(alpha), sin(alpha)) * radius;
}

double star_widget::outer_beta(double static_r, double rotating_r, double alpha)
{
    return alpha * (1. + static_r / rotating_r);
}

double star_widget::inner_beta(double static_r, double rotating_r, double alpha)
{
    return alpha * (1. - static_r / rotating_r);
}

QPointF star_widget::poi(double static_r, double rotating_r, double alpha, double smoothness)
{
    return origin + from_polar(alpha, static_r - rotating_r) + smoothness * from_polar(inner_beta(static_r, rotating_r, alpha), rotating_r);
}

void star_widget::adjust_alpha(double& alpha, bool visible, double dt)
{
    if (!visible)
        dt = -dt;

    alpha += dt * 0.003;
    if (alpha < 0.)
        alpha = 0.;
    else if (alpha > 1.)
        alpha = 1.;
}
