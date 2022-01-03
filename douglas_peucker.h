#pragma once

#include <QPoint>
#include <vector>

std::vector<QPointF> simplify_polyline(std::vector<QPointF> const& polyline, double eps);
