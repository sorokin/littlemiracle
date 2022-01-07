#include "star_widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <cmath>
#include "douglas_peucker.h"

namespace
{
    constexpr QPointF ORIGIN = QPointF();
    constexpr double STATIC_CIRCLE_R = 0.48;

    QPointF from_polar(double phi, double radius)
    {
        return QPointF(cos(phi), sin(phi)) * radius;
    }

    double rotating_circle_angle(double static_r, double rotating_r, double phi)
    {
        return phi * (1. - static_r / rotating_r);
    }

    QPointF point_on_rotating_circle(double static_r, double rotating_r, double phi, double smoothness)
    {
        return ORIGIN + from_polar(phi, static_r - rotating_r) + smoothness * from_polar(rotating_circle_angle(static_r, rotating_r, phi), rotating_r);
    }
}

star_widget::star_widget(QWidget* parent)
    : QWidget(parent)
{
    phi_timer.start();
    alpha_timer.start();
    validate_star_path();
}

void star_widget::set_desired_num(size_t num)
{
    this->desired_num = num;
    update_actual_num_denom();
    validate_star_path();
}

void star_widget::set_desired_denom(size_t denom)
{
    this->desired_denom = denom;
    update_actual_num_denom();
    validate_star_path();
}

void star_widget::set_desired_num_denom(size_t num, size_t denom)
{
    this->desired_num = num;
    this->desired_denom = denom;
    update_actual_num_denom();
    validate_star_path();
}

void star_widget::set_sharpness(double sharpness)
{
    this->sharpness = sharpness;
    validate_star_path();
}

size_t star_widget::get_desired_num() const
{
    return desired_num;
}

size_t star_widget::get_desired_denom() const
{
    return desired_denom;
}

size_t star_widget::get_actual_num() const
{
    return actual_num;
}

size_t star_widget::get_actual_denom() const
{
    return actual_denom;
}

void star_widget::set_visibility(visibility_flags visibility)
{
    this->visibility = visibility;
    if (need_alpha_animation())
        alpha_timer.start();
    else
        alpha_timer.invalidate();
    update();
}

void star_widget::set_color(chart_element_id element, QColor color)
{
    colors[element] = color;
    update();
}

void star_widget::set_antialiasing(bool enabled)
{
    enable_antialiasing = enabled;
    update();
}

void star_widget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        e->accept();
        if (phi_timer.isValid())
            phi_timer.invalidate();
        else
            phi_timer.start();
        update();
        return;
    }
    
    QWidget::mousePressEvent(e);
}

void star_widget::paintEvent(QPaintEvent* event)
{
    QElapsedTimer paint_timer;
    {
        paint_timer.start();
        QPainter p(this);
        if (enable_antialiasing)
            p.setRenderHint(QPainter::Antialiasing, true);
        p.translate(QPointF(width(), height()) / 2.);
        double scale = std::min(width(), height());
        if (sharpness > 1.)
            scale *= 2 / (1 + sharpness);
        p.scale(scale, -scale);
        {
            QPen pen = p.pen();
            double w = 0.003;
            if (sharpness > 1.)
                w *= (1 + sharpness) / 2;
            pen.setWidthF(w);
            p.setPen(pen);
        }

        p.drawEllipse(ORIGIN, STATIC_CIRCLE_R, STATIC_CIRCLE_R);
    
        if (actual_num > actual_denom)
            return;

        update_alpha();

        if (current_alpha[chart_element_id::stars] != 0.)
        {
            {
                QPen pen = p.pen();
                pen.setColor(get_color(chart_element_id::stars));
                p.setPen(pen);
            }

            p.drawPath(star_path_cache);
        }

        update_phi();

        size_t co_num = actual_denom - actual_num;
        double small_r = rotating_circle_r();

        std::vector<QPointF> points;
        for (size_t i = 0; i != actual_num * co_num; ++i)
            points.push_back(point_on_rotating_circle(STATIC_CIRCLE_R, small_r, phi + ((double)i / (double)co_num) * 2 * M_PI, sharpness));
    
        if (current_alpha[chart_element_id::triangles] != 0.)
        {
            {
                QPen pen = p.pen();
                pen.setColor(get_color(chart_element_id::triangles));
                p.setPen(pen);
            }

            if (actual_num != 0)
                for (size_t i = 0; i != co_num; ++i)
                    draw_polygon(p, points.data() + i, actual_num, co_num);
        }
    
        if (current_alpha[chart_element_id::squares] != 0.)
        {
            {
                QPen pen = p.pen();
                pen.setColor(get_color(chart_element_id::squares));
                p.setPen(pen);
            }

            if (co_num != 0)
                for (size_t i = 0; i != actual_num; ++i)
                    draw_polygon(p, points.data() + i, co_num, actual_num);
        }
    
        if (current_alpha[chart_element_id::circles] != 0.)
        {
            QPen pen = p.pen();
            pen.setColor(get_color(chart_element_id::circles));
            p.setPen(pen);
            
            draw_circle(p, ORIGIN + from_polar(phi, STATIC_CIRCLE_R - small_r), small_r, rotating_circle_angle(STATIC_CIRCLE_R, small_r, phi));
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

    qint64 paint_time = paint_timer.nsecsElapsed();
    {
        QPainter p(this);
        if (!phi_timer.isValid())
            p.drawText(this->contentsRect(), Qt::AlignCenter, "Paused. Click to resume.");
        else
            p.drawText(this->contentsRect(), Qt::AlignBottom | Qt::AlignLeft, QString("Render Time: %1 ms").arg(paint_time / 1'000'000., 0, 'f', 2));
    }

    if (phi_timer.isValid() || alpha_timer.isValid())
        update();
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

void star_widget::draw_polygon(QPainter& p, QPointF const* vertices, size_t n, size_t step)
{
    QPainterPath path;
    path.moveTo(vertices[0]);
    for (size_t i = 1; i != n; ++i)
        path.lineTo(vertices[i * step]);
    path.closeSubpath();
    p.drawPath(path);
}

void star_widget::adjust_alpha(double& alpha, bool visible, double dt)
{
    if (!visible)
        dt = -dt;

    alpha += dt * 3e-9;
    if (alpha < 0.)
        alpha = 0.;
    else if (alpha > 1.)
        alpha = 1.;
}

bool star_widget::need_alpha_animation() const
{
    for (size_t i = 0; i != static_cast<size_t>(chart_element_id::max); ++i)
    {
        auto e = static_cast<chart_element_id>(i);
        if (visibility[e])
        {
            if (current_alpha[e] != 1.)
                return true;
        }
        else
        {
            if (current_alpha[e] != 0.)
                return true;
        }
    }

    return false;
}

QColor star_widget::get_color(chart_element_id e) const
{
    QColor c = colors[e];
    c.setAlphaF(current_alpha[e]);
    return c;
}

void star_widget::update_actual_num_denom()
{
    size_t d = std::gcd(desired_num, desired_denom);
    actual_num = desired_num / d;
    actual_denom = desired_denom / d;
}

void star_widget::validate_star_path()
{
    double rotating_r = rotating_circle_r();

    constexpr size_t initial_subdivisions = 20;
    std::vector<QPointF> v;
    v.reserve(initial_subdivisions + 1);
    for (size_t i = 0; i <= initial_subdivisions; ++i)
    {
        double phi = (i * (double)actual_num * 2. * M_PI) / ((double)actual_denom * initial_subdivisions);
        v.push_back(point_on_rotating_circle(STATIC_CIRCLE_R, rotating_r, phi, sharpness));
    }

    v = simplify_polyline(v, 0.001);

    star_path_cache.clear();
    star_path_cache.moveTo(v[0]);
    for (size_t j = 0; j != actual_denom; ++j)
    {
        double phi = (j * (double)actual_num * 2 * M_PI) / (actual_denom);
        QPointF row1 = QPointF(cos(phi), -sin(phi));
        QPointF row2 = QPointF(sin(phi), cos(phi));

        for (size_t i = 1; i != v.size(); ++i)
        {
            QPointF q = QPointF(QPointF::dotProduct(row1, v[i]),
                                QPointF::dotProduct(row2, v[i]));
            star_path_cache.lineTo(q);
        }
    }
    star_path_cache.closeSubpath();
}

double star_widget::rotating_circle_r() const
{
    return STATIC_CIRCLE_R * (double)actual_num / (double)actual_denom;
}

void star_widget::update_phi()
{
    if (!phi_timer.isValid())
        return;

    qint64 dt = phi_timer.nsecsElapsed();
    phi_timer.start();

    assert(phi >= 0.);
    phi += (5e-9 / actual_denom) * dt;
    if (phi >= actual_num * 2 * M_PI)
        phi -= actual_num * 2 * M_PI;
}

void star_widget::update_alpha()
{
    if (!alpha_timer.isValid())
        return;

    qint64 dt = alpha_timer.nsecsElapsed();
    alpha_timer.start();

    for (size_t i = 0; i != static_cast<size_t>(chart_element_id::max); ++i)
    {
        auto e = static_cast<chart_element_id>(i);
        adjust_alpha(current_alpha[e], visibility[e], dt);
    }

    if (!need_alpha_animation())
        alpha_timer.invalidate();
}
