#include "star_widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMouseEvent>
#include <cmath>
#include "douglas_peucker.h"

star_widget::star_widget(QWidget* parent)
    : QWidget(parent)
{
    etimer.start();
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

size_t star_widget::get_num() const
{
    return desired_num;
}

size_t star_widget::get_denom() const
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

void star_widget::mousePressEvent(QMouseEvent* e)
{
    if (e->button() == Qt::LeftButton)
    {
        e->accept();
        if (etimer.isValid())
            etimer.invalidate();
        else
            etimer.start();
        update();
        return;
    }
    
    QWidget::mousePressEvent(e);
}

void star_widget::paintEvent(QPaintEvent* event)
{
    qint64 dt = 0;
    if (etimer.isValid())
        dt = etimer.restart();

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

        p.drawEllipse(origin, big_r, big_r);
    
        if (actual_num > actual_denom)
            return;

        update_alpha(dt);

        if (current_alpha[chart_element_id::stars] != 0.)
        {
            {
                QPen pen = p.pen();
                pen.setColor(get_color(chart_element_id::stars));
                p.setPen(pen);
            }

            p.drawPath(star_path);
        }

        update_phi(dt);

        size_t co_num = actual_denom - actual_num;
        double small_r = this->small_r();

        std::vector<QPointF> points;
        for (size_t i = 0; i != actual_num * co_num; ++i)
            points.push_back(poi(big_r, small_r, phi + ((double)i / (double)co_num) * 2 * M_PI, sharpness));
    
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

    qint64 paint_time = paint_timer.nsecsElapsed();
    {
        QPainter p(this);
        if (!etimer.isValid())
            p.drawText(this->contentsRect(), Qt::AlignCenter, "Paused. Click to resume.");
        else
            p.drawText(this->contentsRect(), Qt::AlignBottom | Qt::AlignLeft, QString("Render Time: %1 ms").arg(paint_time / 1'000'000., 0, 'f', 2));
    }

    if (etimer.isValid())
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

void star_widget::update_actual_num_denom()
{
    size_t d = std::gcd(desired_num, desired_denom);
    actual_num = desired_num / d;
    actual_denom = desired_denom / d;
}

void star_widget::validate_star_path()
{
    double small_r = this->small_r();

    constexpr size_t initial_subdivisions = 20;
    std::vector<QPointF> v;
    v.reserve(initial_subdivisions + 1);
    for (size_t i = 0; i <= initial_subdivisions; ++i)
    {
        double phi = (i * (double)actual_num * 2. * M_PI) / ((double)actual_denom * initial_subdivisions);
        v.push_back(poi(big_r, small_r, phi, sharpness));
    }

    v = simplify_polyline(v, 0.001);

    star_path.clear();
    star_path.moveTo(v[0]);
    for (size_t j = 0; j != actual_denom; ++j)
    {
        double phi = (j * (double)actual_num * 2 * M_PI) / (actual_denom);
        QPointF row1 = QPointF(cos(phi), -sin(phi));
        QPointF row2 = QPointF(sin(phi), cos(phi));

        for (size_t i = 1; i != v.size(); ++i)
        {
            QPointF q = QPointF(QPointF::dotProduct(row1, v[i]),
                                QPointF::dotProduct(row2, v[i]));
            star_path.lineTo(q);
        }
    }
    star_path.closeSubpath();
}

double star_widget::small_r() const
{
    return big_r * (double)actual_num / (double)actual_denom;
}

void star_widget::update_phi(qint64 dt)
{
    assert(phi >= 0.);
    phi += (0.005 / actual_denom) * dt;
    if (phi >= actual_num * 2 * M_PI)
        phi -= actual_num * 2 * M_PI;
}

void star_widget::update_alpha(qint64 dt)
{
    for (size_t i = 0; i != static_cast<size_t>(chart_element_id::max); ++i)
    {
        auto e = static_cast<chart_element_id>(i);
        adjust_alpha(current_alpha[e], visibility[e], dt);
    }
}
