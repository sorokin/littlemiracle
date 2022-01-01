#include "star_widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
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
    assert(etimer.isValid());
    assert(phi >= 0.);
    qint64 dt = etimer.restart();
    phi += (0.007 / denom) * dt;
    if (phi >= num * 2 * M_PI)
        phi -= num * 2 * M_PI;

    for (auto e : all_enumerators<chart_element_id>())
        adjust_alpha(current_alpha[e], visibility[e], dt);

    update();
}

void star_widget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        e->accept();
        if (timer.isActive())
        {
            etimer.invalidate();
            timer.stop();
        }
        else
        {
            etimer.start();
            timer.start();
        }
        update();
        return;
    }
    
    QWidget::mousePressEvent(e);
}

void star_widget::paintEvent(QPaintEvent* event)
{
    {
        QPainter p(this);
        p.setRenderHint(QPainter::Antialiasing, true);
        p.translate(QPointF(width(), height()) / 2.);
        double scale = std::min(width(), height());
        if (sharpness > 1.)
            scale *= 2 / (1 + sharpness);
        p.scale(scale, -scale);
        {
            QPen pen = p.pen();
            double w = 0.007;
            if (sharpness > 1.)
                w *= (1 + sharpness) / 2;
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
                pen.setColor(get_color(chart_element_id::stars));
                p.setPen(pen);
            }
    
            QPainterPath path;
            for (double alpha = 0; alpha < num * 2 * M_PI; alpha += 0.03)
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
            points.push_back(poi(big_r, small_r, phi + ((double)i / (double)co_num) * 2 * M_PI, sharpness));
    
        if (current_alpha[chart_element_id::triangles] != 0.)
        {
            {
                QPen pen = p.pen();
                pen.setColor(get_color(chart_element_id::triangles));
                p.setPen(pen);
            }

            if (num != 0)
            {
                for (size_t i = 0; i != co_num; ++i)
                {
                    std::vector<QPointF> poly;
                    for (size_t j = 0; j != num; ++j)
                        poly.push_back(points[i + co_num * j]);
                    draw_polygon(p, poly);
                }
            }
        }
    
        if (current_alpha[chart_element_id::squares] != 0.)
        {
            {
                QPen pen = p.pen();
                pen.setColor(get_color(chart_element_id::squares));
                p.setPen(pen);
            }

            if (co_num != 0)
            {
                for (size_t i = 0; i != num; ++i)
                {
                    std::vector<QPointF> poly;
                    for (size_t j = 0; j != co_num; ++j)
                        poly.push_back(points[i + num * j]);
                    draw_polygon(p, poly);
                }
            }
        }
    
        if (current_alpha[chart_element_id::circles] != 0.)
        {
            QPen pen = p.pen();
            pen.setColor(get_color(chart_element_id::circles));
            p.setPen(pen);
            
            draw_circle(p, origin + from_polar(phi, big_r - small_r), small_r, inner_angle(big_r, small_r, phi));
        }
    
        if (current_alpha[chart_element_id::dots] != 0.)
        {
            {
                QPen pen = p.pen();
                pen.setStyle(Qt::NoPen);
                p.setPen(pen);
                
                QBrush brush = p.brush();
                brush.setColor(get_color(chart_element_id::dots));
                brush.setStyle(Qt::SolidPattern);
                p.setBrush(brush);
            }
    
            for (size_t i = 0; i != points.size(); ++i)
                p.drawEllipse(points[i], 0.01, 0.01);
        }
    }
    
    if (!timer.isActive())
    {
        QPainter p(this);
        p.drawText(this->contentsRect(), Qt::AlignCenter, "Paused. Click to resume.");
    }
}

void star_widget::draw_circle(QPainter& p, QPointF center, double radius, double alpha)
{
    p.drawEllipse(center, radius, radius);
    // workaround: sometimes qt draw a horizontal line instead of point
    if (sharpness > 0.001)
    {
        QPointF tip = center + sharpness * from_polar(alpha, radius);
        p.drawLine(center, tip);
    }
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

double star_widget::outer_angle(double static_r, double rotating_r, double phi)
{
    return phi * (1. + static_r / rotating_r);
}

double star_widget::inner_angle(double static_r, double rotating_r, double phi)
{
    return phi * (1. - static_r / rotating_r);
}

QPointF star_widget::poi(double static_r, double rotating_r, double alpha, double smoothness)
{
    return origin + from_polar(alpha, static_r - rotating_r) + smoothness * from_polar(inner_angle(static_r, rotating_r, alpha), rotating_r);
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

QColor star_widget::get_color(chart_element_id e) const
{
    QColor c = colors[e];
    c.setAlphaF(current_alpha[e]);
    return c;
}
